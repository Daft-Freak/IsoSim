#pragma once

#include <cstdint>

class EntityInfo final {
public:
    uint8_t w, h;
    uint16_t base_sprite;
};

extern const EntityInfo entity_bathroom_shower;
extern const EntityInfo entity_bathroom_sink;
extern const EntityInfo entity_bathroom_toilet;

extern const EntityInfo entity_bedroom_bed;

extern const EntityInfo entity_kitchen_fridge;
extern const EntityInfo entity_kitchen_oven;
extern const EntityInfo entity_kitchen_sink;

extern const EntityInfo entity_livingroom_tv;