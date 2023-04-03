#pragma once

#include <cstdint>

enum EntityAction {
    Action_Sleep     = 1 << 0, // beds
    Action_StoreFood = 1 << 1, // fridge
    Action_Bathe     = 1 << 2, // bath/shower
    Action_Toilet    = 1 << 3,
    Action_HaveFun   = 1 << 4, // things that increase fun as main function
};

class EntityInfo final {
public:
    uint8_t w, h;
    uint16_t base_sprite;

    float need_effect[5]; // sleep, hunger, hygiene, toilet, fun
    uint8_t use_range;
    uint32_t basic_actions;
};

extern const EntityInfo entity_bathroom_shower;
extern const EntityInfo entity_bathroom_sink;
extern const EntityInfo entity_bathroom_toilet;

extern const EntityInfo entity_bedroom_bed;

extern const EntityInfo entity_kitchen_fridge;
extern const EntityInfo entity_kitchen_oven;
extern const EntityInfo entity_kitchen_sink;

extern const EntityInfo entity_livingroom_tv;