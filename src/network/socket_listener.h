#pragma once

#include "common/sequence.h"
#include "network/boost.h"
#include <memory>

namespace network {

class socket_handler;

class socket_listener : public std::enable_shared_from_this<socket_listener>
{
  public:
    socket_listener(net::io_context& ioc,
                    tcp::endpoint endpoint,
                    std::shared_ptr<socket_handler> socket_handler = {});

    ~socket_listener() = default;

    void open();

  private:
    void on_accept(beast::error_code ec, tcp::socket socket);

  private:
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

    common::sequence<uint32_t> socket_id_sequence_;
    std::shared_ptr<socket_handler> socket_handler_;
}; // class socket_listener

} // namespace network
