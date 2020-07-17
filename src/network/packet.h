#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <string_view>
#include "common/json.h"

namespace network {
    namespace packet {
        struct base {
            std::optional<uint16_t> opcode;
            std::optional<json> data;
        };

        void to_json(json& json, const base& base);
        void from_json(const json& json, base& base);

        base deserialize(const std::vector<uint8_t>& packet);

        template<typename T>
        std::vector<uint8_t> serialize(uint16_t opcode, const T& data) {
            return json::to_cbor(base{ opcode, data });
        }

        namespace server {
            // ID
            struct id {
                uint32_t value;
            };

            // POSITION COMPONENT
            struct position {
                float x, y;
            };

            // COMPONENT STORAGE
            struct component_storage {
                std::optional<position> position;
            };

            // ENTITY
            struct entity {
                uint32_t id;
                std::optional<component_storage> components;
            };

            // STATE
            struct state {
                std::vector<entity> entities;
            };

            void to_json(json& json, const id& id);
            void to_json(json& json, const position& pos);
            void to_json(json& json, const component_storage& cs);
            void to_json(json& json, const entity& entity);
            void to_json(json& json, const state& state);
            void from_json(const json& json, id& id);
            void from_json(const json& json, position& pos);
            void from_json(const json& json, component_storage& cs);
            void from_json(const json& json, entity& entity);
            void from_json(const json& json, state& state);
        } // namespace server
    } // namespace packet
} // namespace network