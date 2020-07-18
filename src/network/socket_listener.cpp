
#include "network/socket_listener.h"
#include "common/log.h"
#include "network/socket.h"
#include "network/socket_handler.h"
#include <string_view>

inline void
fail(std::string_view what, beast::error_code ec)
{
    if (ec == net::error::operation_aborted || ec == net::error::connection_aborted ||
        ec == beast::websocket::error::closed)
        return;

    WARNF("SOCKET_LISTENER", "{}: {}", what, ec.message());
}

struct default_socket_handler : public network::socket_handler
{
    std::mutex socket_mutex;
    std::map<uint32_t, std::weak_ptr<network::socket_base>> sockets;

    virtual void on_open(uint32_t id, std::weak_ptr<network::socket_base> socket) override
    {
        std::lock_guard<std::mutex> lock(socket_mutex);
        INFOF("SOCKET_LISTENER", "Socket ID {} -> opened", id);
        sockets.insert(std::make_pair(id, socket));
    }

    virtual void on_close(uint32_t id) override
    {
        std::lock_guard<std::mutex> lock(socket_mutex);
        INFOF("SOCKET_LISTENER", "Socket ID {} -> closed", id);
        sockets.erase(id);
    }

    virtual void on_message(uint32_t id, std::vector<uint8_t>&& data) override
    {
        std::lock_guard<std::mutex> lock(socket_mutex);
        INFOF("SOCKET_LISTENER", "Socket ID {} -> message size {}", id, data.size());
        // echo the message
        if (auto socket = sockets.find(id); socket != sockets.end()) {
            if (auto ptr = socket->second.lock()) {
                ptr->send(data);
            }
        }
    }

    virtual void on_error(uint32_t id, std::string_view what, beast::error_code error) override
    {
        if (error == net::error::operation_aborted || error == net::error::connection_aborted ||
            error == beast::websocket::error::closed)
            return;

        // just log the error
        ERRF("SOCKET_LISTENER", "Socket ID {} -> error: {}, {}", id, what, error.message());
    }
};

static default_socket_handler default_handler;

namespace network {

socket_listener::socket_listener(net::io_context& ioc,
                                 tcp::endpoint endpoint,
                                 std::shared_ptr<socket_handler> socket_handler)
  : ioc_(ioc)
  , acceptor_(ioc)
  , socket_id_sequence_(0, 1)
  , socket_handler_(socket_handler)
{
    if (!socket_handler_) {
        socket_handler_ = std::make_shared<default_socket_handler>();
    }

    beast::error_code ec;

    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        fail("socket_listener::open", ec);
        return;
    }

    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
        fail("socket_listener::set_option", ec);
        return;
    }

    acceptor_.bind(endpoint, ec);
    if (ec) {
        fail("socket_listener::bind", ec);
        return;
    }

    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
        fail("socket_listener::listen", ec);
        return;
    }
}

void
socket_listener::open()
{
    acceptor_.async_accept(net::make_strand(ioc_),
                           beast::bind_front_handler(&socket_listener::on_accept, shared_from_this()));
}

void
socket_listener::on_accept(beast::error_code ec, tcp::socket socket)
{
    if (ec) {
        return fail("socket_listener::on_accept", ec);
    } else {
        std::make_shared<network::socket>(socket_id_sequence_.get(), std::move(socket), socket_handler_)->open();
    }

    acceptor_.async_accept(net::make_strand(ioc_),
                           beast::bind_front_handler(&socket_listener::on_accept, shared_from_this()));
}

} // namespace network
