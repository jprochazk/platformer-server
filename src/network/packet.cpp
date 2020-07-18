
#include "packet.h"

namespace network {
namespace packet {
void
to_json(json& json, const base& base)
{
    json = { { "o", base.opcode }, { "d", base.data } };
}
void
from_json(const json& json, base& base)
{
    try_get_to(json, "o", base.opcode);
    try_get_to(json, "d", base.data);
}
base
deserialize(const std::vector<uint8_t>& packet)
{
    return static_cast<base>(json::from_cbor(packet, true, false));
}

namespace server {
void
to_json(json& json, const id& id)
{
    json = { { "v", id.value } };
}
void
to_json(json& json, const position& pos)
{
    json = { { "x", pos.x }, { "y", pos.y } };
}
void
to_json(json& json, const component_storage& cs)
{
    json = { { "p", cs.pos } };
}
void
to_json(json& json, const entity& entity)
{
    json = { { "i", entity.id }, { "c", entity.components } };
}
void
to_json(json& json, const state& state)
{
    json = { { "e", state.entities } };
}

void
from_json(const json& json, id& id)
{
    json["v"].get_to(id.value);
}
void
from_json(const json& json, position& pos)
{
    json["x"].get_to(pos.x);
    json["y"].get_to(pos.y);
}
void
from_json(const json& json, component_storage& cs)
{
    try_get_to(json, "p", cs.pos);
}
void
from_json(const json& json, entity& entity)
{
    json["i"].get_to(entity.id);
    try_get_to(json, "c", entity.components);
}
void
from_json(const json& json, state& state)
{
    json["e"].get_to(state.entities);
}
} // namespace server
} // namespace packet
} // namespace network