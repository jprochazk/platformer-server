#pragma once

#include "game/component.h"
#include "game/system/system_base.h"
#include "network/handler.h"
#include "network/packet.h"
#include "network/socket_handler.h"
#include <cstddef>
#include <cstdint>
#include <entt/entt.hpp>
#include <memory>
#include <moodycamel/concurrentqueue.h>

namespace game {
class world;
namespace system {

class session
  : public system_base
  , public network::socket_handler
{
  public:
    template<typename T>
    using queue = moodycamel::ConcurrentQueue<T>;

    struct message
    {
        uint32_t id;
        std::vector<uint8_t> data;
    };

    session(std::shared_ptr<game::world> world,
            std::shared_ptr<network::packet::handler_table> handler_table =
              std::make_shared<network::packet::default_handler_table>());

    void update() override;

    void on_open(uint32_t id, std::weak_ptr<network::socket_base> socket) override;
    void on_close(uint32_t id) override;
    void on_message(uint32_t id, std::vector<uint8_t>&& data) override;
    void on_error(uint32_t id, std::string_view what, beast::error_code error) override;

  private:
    void handle_connections();
    void handle_messages();
    void handle_disconnections();

    std::atomic<size_t> connection_counter_;
    queue<component::session> connection_queue_;

    std::atomic<size_t> message_counter_;
    queue<message> message_queue_;

    std::atomic<size_t> disconnection_counter_;
    queue<uint32_t> disconnection_queue_;

    std::shared_ptr<network::packet::handler_table> handler_table_;
    std::shared_ptr<game::world> world;
}; // class session

} // namespace system
} // namespace game
