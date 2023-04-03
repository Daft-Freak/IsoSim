#include "entity-info.hpp"

const EntityInfo entity_bathroom_shower{1, 1, 44, {0.0f, 0.0f, 0.0006f, 0.0f, 0.0f}, 0, Action_Bathe};
const EntityInfo entity_bathroom_sink{1, 1, 48, {}, 1, 0};
const EntityInfo entity_bathroom_toilet{1, 1, 52, {0.0f, 0.0f, 0.0f, 0.005f, 0.0f}, 0, Action_Toilet};

const EntityInfo entity_bedroom_bed{1, 2, 24, {0.000025f, 0.0f, 0.0f, 0.0f, 0.0f}, 0, Action_Sleep};

const EntityInfo entity_kitchen_fridge{1, 1, 40, {0.0f, 0.001f, 0.0f, 0.0f, 0.0f}, 1, Action_StoreFood};
const EntityInfo entity_kitchen_oven{1, 1, 36, {}, 1, 0};
const EntityInfo entity_kitchen_sink{1, 1, 32, {}, 1, 0};

const EntityInfo entity_livingroom_tv{2, 1, 28, {0.0f, 0.001f, 0.0f, 0.0f, 0.0001f}, 2, Action_HaveFun};