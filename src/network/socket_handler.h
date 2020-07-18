#pragma once

#include "network/socket.h"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace network {

class socket_handler
{
  public:
    virtual ~socket_handler() = default;

    // Called when a socket is fully opened
    virtual void on_open(uint32_t id, std::weak_ptr<socket_base> socket) = 0;

    // Called when a socket is fully closed
    virtual void on_close(uint32_t id) = 0;

    // Called when a message is received from a socket
    virtual void on_message(uint32_t id, std::vector<uint8_t>&& data) = 0;

    // Called when the socket encounters an error
    // Sockets that got an error shouldn't be considered close until you receive
    // an `on_close` event from it
    virtual void on_error(uint32_t id, std::string_view what, beast::error_code error) = 0;
};

} // namespace network
