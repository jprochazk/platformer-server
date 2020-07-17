#pragma once

#include "network/boost.h"
#include <memory>
#include <atomic>
#include <vector>
#include <cstdint>
#include <boost/core/noncopyable.hpp>

namespace network {

    class socket_handler;

    class socket_base {
    public:
        virtual ~socket_base() = default;
        virtual void open() = 0;
        virtual void close() = 0;
        virtual void send(std::vector<uint8_t> data) = 0;
        virtual bool closed() const = 0;
    };

    class socket : public std::enable_shared_from_this<socket>, private boost::noncopyable, public socket_base {
    public:
        static const size_t MAX_MESSAGE_SIZE = 1024;

        socket(uint32_t id, tcp::socket&& tcp_socket, std::shared_ptr<socket_handler> socket_handler);
        ~socket();

        void open() override;
        void close() override;
        void send(std::vector<uint8_t> data) override;
        bool closed() const override;

    private:
        void on_accept(beast::error_code ec);
        void on_close(beast::error_code ec);
        void on_read(beast::error_code ec, std::size_t rb);
        void on_send(std::vector<uint8_t>&& data);
        void on_write(beast::error_code ec, std::size_t wb);

    private:
        uint32_t id_;
        beast::flat_buffer fbuffer_;
        beast::websocket::stream<beast::tcp_stream> ws_;
        bool writing_;
        std::vector<std::vector<uint8_t>> wbuffer_;

        std::atomic<bool> closed_;
        std::shared_ptr<socket_handler> socket_handler_;
    }; // class socket

} // namespace network

bool operator==(const network::socket& lhs, const network::socket& rhs);

