

#include "network/socket.h"
#include "network/metrics.h"
#include "network/socket_handler.h"
#include <functional>

namespace network {

    socket::socket(
        uint32_t id,
        tcp::socket&& socket,
        std::shared_ptr<socket_handler> socket_handler
        ) : id_(id)
        , fbuffer_()
        , ws_(std::move(socket))
        , writing_(false)
        , wbuffer_()
        , closed_(false)
        , socket_handler_(socket_handler)
    {
    }

    socket::~socket()
    {
        metrics::disconnection();
        socket_handler_->on_close(id_);
    }

    void socket::open()
    {
        ws_.set_option(
            beast::websocket::stream_base::timeout::suggested(
                beast::role_type::server));

        ws_.set_option(beast::websocket::stream_base::decorator(
            [](beast::websocket::response_type& res)
        {
            res.set(http::field::server,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " websocket-server");
        }));

        ws_.read_message_max(MAX_MESSAGE_SIZE);

        ws_.binary(true);

        ws_.async_accept(
            beast::bind_front_handler(
                &socket::on_accept,
                shared_from_this()));
    }

    void socket::close()
    {
        ws_.async_close(
            beast::websocket::close_code::normal,
            beast::bind_front_handler(
                &socket::on_close,
                shared_from_this()));
    }

    void socket::send(std::vector<uint8_t> data)
    {
        net::post(
            ws_.get_executor(),
            beast::bind_front_handler(
                &socket::on_send,
                shared_from_this(),
                std::move(data)));
    }

    bool socket::closed() const {
        return closed_.load(std::memory_order_acquire);
    }

    /* BOOST.BEAST CALLBACKS */

    void socket::on_accept(beast::error_code ec) {
        if (ec) return socket_handler_->on_error(id_, "SOCKET::ACCEPT", ec);

        metrics::connection();
        socket_handler_->on_open(id_, weak_from_this());

        ws_.async_read(
            fbuffer_,
            beast::bind_front_handler(
                &socket::on_read,
                shared_from_this()));
    }

    void socket::on_close(beast::error_code ec) {
        closed_.store(true, std::memory_order_release);
        if (ec) return socket_handler_->on_error(id_, "SOCKET::CLOSE", ec);
    }


    void socket::on_read(beast::error_code ec, std::size_t read_bytes) {
        if (ec) return socket_handler_->on_error(id_, "SOCKET::READ", ec);

        metrics::read(read_bytes);

        // min message size is 2 bytes (sizeof opcode)
        if (read_bytes > 1) {
            std::vector<uint8_t> buf(read_bytes);
            boost::asio::buffer_copy(
                boost::asio::buffer(buf.data(), read_bytes),
                fbuffer_.data());

            socket_handler_->on_message(id_, std::move(buf));
        }
        fbuffer_.consume(read_bytes);

        ws_.async_read(
            fbuffer_,
            beast::bind_front_handler(
                &socket::on_read,
                shared_from_this()));
    }

    void socket::on_send(std::vector<uint8_t>&& data) {
        wbuffer_.push_back(std::move(data));

        if (writing_)
            return;

        writing_ = true;

        ws_.async_write(
            net::buffer(wbuffer_.front()),
            beast::bind_front_handler(
                &socket::on_write,
                shared_from_this()));
    }

    void socket::on_write(beast::error_code ec, std::size_t written_bytes) {
        if (ec) return socket_handler_->on_error(id_, "SOCKET::WRITE", ec);

        metrics::written(written_bytes);

        wbuffer_.erase(wbuffer_.begin());

        if (wbuffer_.empty()) {
            writing_ = false;
            return;
        }

        ws_.async_write(
            net::buffer(wbuffer_.front()),
            beast::bind_front_handler(
                &socket::on_write,
                shared_from_this()));
    }

} // namespace network

bool operator==(const network::socket& lhs, const network::socket& rhs) {
    if (&lhs == &rhs) return true;
    return false;
}
