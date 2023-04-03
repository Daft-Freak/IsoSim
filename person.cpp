#include "engine/engine.hpp"

#include "person.hpp"
#include "world.hpp"


enum PersonBTVariables {
    PersonVar_EntIndex = behaviour_tree::Var_User,
    PersonVar_WorldPtr,
    PersonVar_Position,
    PersonVar_TargetRange,
};

class RandomPositionNode final : public behaviour_tree::Node {
public:

    void init(BehaviourTreeState &state) const override {}

    void deinit(BehaviourTreeState &state) const override {}

    behaviour_tree::Status update(BehaviourTreeState &state) const override {
        auto world = std::any_cast<World *>(state.get_variable(PersonVar_WorldPtr));

        auto bounds = world->get_bounds();
        blit::Point target_pos(blit::random() % bounds.w, blit::random() % bounds.h);

        if(!world->get_entities_on_tile(target_pos).empty())
            return behaviour_tree::Status::Failed;

        state.set_variable(PersonVar_Position, target_pos);
        state.remove_variable(PersonVar_TargetRange);

        return behaviour_tree::Status::Success;
    }

    const char *get_label() const override {
        return "Get random position";
    }
};

class FindEntityPositionForNeed final : public behaviour_tree::Node {
public:
    constexpr FindEntityPositionForNeed(Person::Need need) : need(need){} 

    void init(BehaviourTreeState &state) const override {}

    void deinit(BehaviourTreeState &state) const override {}

    behaviour_tree::Status update(BehaviourTreeState &state) const override {
        using behaviour_tree::Status;
        auto world = std::any_cast<World *>(state.get_variable(PersonVar_WorldPtr));

        auto ent_ids = world->get_entities_for_need(need);

        if(ent_ids.empty())
            return Status::Failed;

        // TODO: sort by effect/distance?
        // TODO: split returning the entities and getting the position

        auto &ent = world->get_entity(ent_ids[0]);
        auto target_pos = ent.get_tile_position();

        state.set_variable(PersonVar_Position, target_pos);

        int range = std::max(1, static_cast<int>(ent.get_info().use_range));
        state.set_variable(PersonVar_TargetRange, range);

        return Status::Success;
    }

    const char *get_label() const override {
        return "Find entity";
    }

private:
    Person::Need need;
};

class MoveToNode final : public behaviour_tree::Node {
public:
    struct State {
        unsigned int path_id;
        PathFinder::Path path;
        unsigned int cur_path_point = 1;
    };

    void init(BehaviourTreeState &state) const override {
        auto node_state = new State;
        state.create_node_state(this, node_state);

        auto entity_index = std::any_cast<uint16_t>(state.get_variable(PersonVar_EntIndex));
        auto world = std::any_cast<World *>(state.get_variable(PersonVar_WorldPtr));

        // start path find
        auto start_pos = world->get_entity(entity_index).get_tile_position();

        blit::Point target_pos;
        if(state.has_variable(PersonVar_Position)) // TODO: fail if not set?
            target_pos = std::any_cast<blit::Point>(state.get_variable(PersonVar_Position));

        int range = 0;
        if(state.has_variable(PersonVar_TargetRange))
            range = std::any_cast<int>(state.get_variable(PersonVar_TargetRange));

        node_state->path_id = world->get_path_finder().start_path_find(start_pos, target_pos, {1, 1}, range);
    }

    void deinit(BehaviourTreeState &state) const override {
        auto node_state = std::any_cast<State *>(state.get_node_state(this));
        auto world = std::any_cast<World *>(state.get_variable(PersonVar_WorldPtr));

        // clean up
        world->get_path_finder().remove_path_find(node_state->path_id);

        delete node_state;
        state.destroy_node_state(this);
    }

    behaviour_tree::Status update(BehaviourTreeState &state) const override {
        using behaviour_tree::Status;

        auto node_state = std::any_cast<State *>(state.get_node_state(this));
        auto entity_index = std::any_cast<uint16_t>(state.get_variable(PersonVar_EntIndex));

        auto world = std::any_cast<World *>(state.get_variable(PersonVar_WorldPtr));
        
        auto &path_finder = world->get_path_finder();

        // don't have the path yet
        if(!path_finder.is_path_find_done(node_state->path_id))
            return Status::Running;

        if(node_state->path.path.empty()) {
            path_finder.get_found_path(node_state->path_id, node_state->path);

            // if the path is still empty the path find failed
            if(node_state->path.path.empty())
                return Status::Failed;
        }

        // get person
        auto person = world->find_person(entity_index);

        if(!person)
            return Status::Failed;

        // start next move if not moving
        if(!person->is_moving()) {
            if(node_state->cur_path_point < node_state->path.path.size())
                person->move_to_tile(node_state->path.path[node_state->cur_path_point++]);
            else // no more points, done
                return Status::Success;
        }

        return Status::Running;
    }

    const char *get_label() const override {
        return "Move to";
    }
};

// sit there and think for a bit
class ThinkNode final : public behaviour_tree::Node {
public:
    void init(BehaviourTreeState &state) const override {
        int timer = blit::random() % 1000 + 1;
        state.create_node_state(this, timer);
    }

    void deinit(BehaviourTreeState &state) const override {
        state.destroy_node_state(this);
    }

    behaviour_tree::Status update(BehaviourTreeState &state) const override {
        using behaviour_tree::Status;

        auto &timer = std::any_cast<int &>(state.get_node_state(this));

        if(--timer == 0)
            return Status::Success;

        return Status::Running;
    }

    const char *get_label() const override {
        return "Think";
    }
};

// check need
class IsNeedLowNode final : public behaviour_tree::Node {
public:
    constexpr IsNeedLowNode(Person::Need need) : need(need){}

    void init(BehaviourTreeState &state) const override {}

    void deinit(BehaviourTreeState &state) const override {}

    behaviour_tree::Status update(BehaviourTreeState &state) const override {
        using behaviour_tree::Status;

        auto entity_index = std::any_cast<uint16_t>(state.get_variable(PersonVar_EntIndex));
        auto world = std::any_cast<World *>(state.get_variable(PersonVar_WorldPtr));

        // get person
        auto person = world->find_person(entity_index);

        if(!person)
            return Status::Failed;

        // do something if really low, or low and another need isn't really low
        auto need_val = person->get_need(need);
        if(need_val < 0.2f || (person->get_need(person->get_lowest_need()) >= 0.2f && need_val < 0.5f)) {
            return Status::Success;
        }

        return Status::Failed;
    }

    const char *get_label() const override {
        return "Check need";
    }

private:
    Person::Need need;
};

class ShouldSleepNode final : public behaviour_tree::Node {
public:
    void init(BehaviourTreeState &state) const override {}

    void deinit(BehaviourTreeState &state) const override {}

    behaviour_tree::Status update(BehaviourTreeState &state) const override {
        using behaviour_tree::Status;

        auto entity_index = std::any_cast<uint16_t>(state.get_variable(PersonVar_EntIndex));
        auto world = std::any_cast<World *>(state.get_variable(PersonVar_WorldPtr));

        // get person
        auto person = world->find_person(entity_index);

        if(!person)
            return Status::Failed;

        auto ent_ids = world->get_entities_for_need(Person::Need::Sleep);

        if(ent_ids.empty())
            return Status::Failed; // can't sleep if there are no beds

        // get effect on need, assume all beds are the same
        // FIXME: hardcoded decay
        auto need_decay = 0.00001f;
        auto bed_effect = world->get_entity(ent_ids[0]).get_info().need_effect[0] - need_decay;

        // figure out when we would wake up
        float sleep_duration = (1.0f - person->get_need(Person::Need::Sleep)) / bed_effect;
        float time_to_empty = person->get_need(Person::Need::Sleep) / need_decay;
        auto wake_time = world->get_time(sleep_duration);

        auto now_time = world->get_time();

        // don't sleep if we would get up at some silly time the next day... or the same day
        // sleep anyway if we'll pass out soon (in an hour)
        // TODO: adjust for work start time
        if((wake_time.hours < 6 || (now_time.hours > 6 && wake_time.days == now_time.days)) && time_to_empty > 100 * 60)
            return Status::Failed;

        return Status::Success;
    }

    const char *get_label() const override {
        return "Should sleep";
    }
};

class UseUntilNeedRestoredNode final : public behaviour_tree::Node {
public:
    constexpr UseUntilNeedRestoredNode(Person::Need need) : need(need){}

    struct State {
        bool started_use = false;
        unsigned int ent_id;
    };

    void init(BehaviourTreeState &state) const override {
        State node_state;

        state.create_node_state(this, node_state);
    }

    void deinit(BehaviourTreeState &state) const override {
        state.destroy_node_state(this);
    }

    behaviour_tree::Status update(BehaviourTreeState &state) const override {
        using behaviour_tree::Status;

        auto &node_state = std::any_cast<State &>(state.get_node_state(this));

        auto entity_index = std::any_cast<uint16_t>(state.get_variable(PersonVar_EntIndex));
        auto world = std::any_cast<World *>(state.get_variable(PersonVar_WorldPtr));

        // get person
        auto person = world->find_person(entity_index);

        if(!person)
            return Status::Failed;

        if(person->is_moving())
            return Status::Running;

        // should be next to or on the object
        auto &ent = world->get_entity(entity_index);
        bool in_use = node_state.started_use && person->is_using_entity(node_state.ent_id);

        if(!node_state.started_use) {
            // find the object
            // asuume range used for path find is object use_range
            int range = 1;
            if(state.has_variable(PersonVar_TargetRange))
                range = std::any_cast<int>(state.get_variable(PersonVar_TargetRange));

            for(int y = -range; y <= range && !node_state.started_use; y++) {
                for(int x = -range; x <= range; x++) {
                    blit::Point off(x, y);
                    if(world->has_entity_for_need(ent.get_tile_position() + off, need)) {
                        // assume it's the first one
                        node_state.ent_id = world->get_entities_on_tile(ent.get_tile_position() + off)[0];
                        // get in/start use
                        if(!person->start_using_entity(node_state.ent_id))
                            return Status::Failed;

                        node_state.started_use = true;
                        break;
                    }
                }
            }

            // couldn't find it, fail
            if(!node_state.started_use)
                return Status::Failed;
            
            return Status::Running;
        } else if(in_use && person->get_need(need) == 1.0f) {
            // get out
            if(!person->stop_using_entity(node_state.ent_id))
                return Status::Failed;

            return Status::Running;
        }

        return in_use ? Status::Running : Status::Success;
    }

    const char *get_label() const override {
        return "Use until restored";
    }

private:
    Person::Need need;
};

static const RandomPositionNode random_pos;
static const MoveToNode move_to;
static const ThinkNode think;

static const behaviour_tree::SequenceNode<3> move_sequence({&random_pos, &move_to, &think});

// hunger
static const IsNeedLowNode check_hungry(Person::Need::Hunger);
static const FindEntityPositionForNeed find_food(Person::Need::Hunger);
static const UseUntilNeedRestoredNode eat(Person::Need::Hunger);

// TODO: get/make, then eat food
static const behaviour_tree::SequenceNode<4> eat_sequence({&check_hungry, &find_food, &move_to, &eat});

// toilet
static const IsNeedLowNode check_toilet(Person::Need::Toilet);
static const FindEntityPositionForNeed find_toilet(Person::Need::Toilet);
static const UseUntilNeedRestoredNode use_toilet(Person::Need::Toilet);

// TODO: use sink after
static const behaviour_tree::SequenceNode<4> toilet_sequence({&check_toilet, &find_toilet, &move_to, &use_toilet});

// sleep
static const IsNeedLowNode check_tired(Person::Need::Sleep);
static const ShouldSleepNode should_sleep; // TODO: switch order and pass the bed here?
static const FindEntityPositionForNeed find_bed(Person::Need::Sleep);
static const UseUntilNeedRestoredNode sleep(Person::Need::Sleep);

static const behaviour_tree::SequenceNode<5> sleep_sequence({&check_tired, &should_sleep, &find_bed, &move_to, &sleep});

// hygiene
static const IsNeedLowNode check_hygiene(Person::Need::Hygiene);
static const FindEntityPositionForNeed find_shower(Person::Need::Hygiene); // ...or bath
static const UseUntilNeedRestoredNode use_shower(Person::Need::Hygiene);

static const behaviour_tree::SequenceNode<4> shower_sequence({&check_hygiene, &find_shower, &move_to, &use_shower});

// fun
static const IsNeedLowNode check_fun(Person::Need::Fun);
static const FindEntityPositionForNeed find_fun(Person::Need::Fun);
static const UseUntilNeedRestoredNode have_fun(Person::Need::Fun);

static const behaviour_tree::SequenceNode<4> fun_sequence({&check_fun, &find_fun, &move_to, &have_fun});

// main selector, attempt to improve a need
static const behaviour_tree::SelectorNode<6> idle_selector({&eat_sequence, &toilet_sequence, &sleep_sequence, &shower_sequence, &fun_sequence, &move_sequence});

static const behaviour_tree::RepeaterNode tree_root(&idle_selector);

Person::Person(World &world, uint16_t entity_index) : world(world), entity_index(entity_index), behaviour_tree(&tree_root), needs{0.2f, 0.5f, 0.5f, 0.5f, 0.5f} {
    behaviour_tree.set_variable(PersonVar_EntIndex, entity_index);
    behaviour_tree.set_variable(PersonVar_WorldPtr, &world);
}

void Person::update(uint32_t time) {
    auto &entity = world.get_entity(entity_index);

    // update needs

    bool is_sleeping = false;

    // apply affects of entity we're "using"
    if(entity_in_use != ~0u && !is_moving()) {
        auto &ent_info = world.get_entity(entity_in_use).get_info();

        is_sleeping = is_sleeping || ent_info.need_effect[0] > 0.0f; // assume in bed == sleeping

        for(size_t i = 0; i < std::size(needs); i++) {
            needs[i] += ent_info.need_effect[i];
        }
    }

    float sleep_adjust = is_sleeping ? 0.25f : 1.0f;

    get_need(Need::Sleep) -= 0.00001f;
    get_need(Need::Hunger) -= 0.00001f * sleep_adjust;
    get_need(Need::Hygiene) -= 0.000004f * sleep_adjust;
    get_need(Need::Toilet) -= 0.000008f * sleep_adjust;
    get_need(Need::Fun) -= 0.000008f * sleep_adjust;

    // clamp
    for(auto &need : needs)
        need = std::max(0.0f, std::min(1.0f, need));

    // run behaviour
    behaviour_tree.update();

    // move
    if(is_moving()) {
        move_progress++;
        entity.set_position(move_start_pos + (move_target_pos - move_start_pos) * move_progress / 32);
    }
}

bool Person::is_moving() const {
    return move_start_pos != move_target_pos && world.get_entity(entity_index).get_position() != move_target_pos;
}

void Person::move_to_tile(blit::Point tile_pos) {
    move_target_pos = tile_pos * 16 + blit::Point(8, 8);
    move_start_pos = world.get_entity(entity_index).get_position();

    move_progress = 0;
}


const char *Person::get_action_label() const {
    return behaviour_tree.get_attive_node_label();
}

uint16_t Person::get_entity_index() const {
    return entity_index;
}

float &Person::get_need(Need need) {
    return needs[static_cast<int>(need)];
}

Person::Need Person::get_lowest_need() const {
    auto ret = Need::Sleep;
    auto lowest = needs[0];

    for(size_t i = 1; i < std::size(needs); i++) {
        if(needs[i] < lowest) {
            lowest = needs[i];
            ret = static_cast<Need>(i);
        }
    }

    return ret;
}

bool Person::start_using_entity(unsigned int entity) {
    if(entity_in_use != ~0u)
        return false;

    auto &ent = world.get_entity(entity);
    auto ent_pos = ent.get_tile_position();
    auto dist = ent_pos - world.get_entity(entity_index).get_tile_position();
    int use_range = ent.get_info().use_range;
    
    if(dist.x + dist.y > std::max(1, use_range))
        return false;

    // move into/on top of entity
    if(use_range == 0)
        move_to_tile(ent_pos);

    entity_in_use = entity;
    
    return true;
}

bool Person::stop_using_entity(unsigned int entity) {

    if(entity_in_use != entity)
        return false;

    auto &ent = world.get_entity(entity);
    auto ent_pos = ent.get_tile_position();
    auto self_pos = world.get_entity(entity_index).get_tile_position();

    if(self_pos == ent_pos) {
        // get out of entity
        auto target_pos = ent_pos;

        switch(ent.get_rotation()) {
            case 0: target_pos.y++; break;
            case 1: target_pos.x++; break;
            case 2: target_pos.y--; break;
            case 3: target_pos.x--; break;
        }

        // TODO: check for empty adjacent tile
        move_to_tile(target_pos);
    }

    entity_in_use = ~0u;

    return true;
}

bool Person::is_using_entity(unsigned int entity) const {
    return entity_in_use == entity;
}

unsigned int Person::get_entity_in_use() const {
    return entity_in_use;
}

bool Person::is_sleeping() const {
    if(entity_in_use == ~0u || is_moving())
        return false;

    auto &ent_info = world.get_entity(entity_in_use).get_info();

    // assume in bed == sleeping
    // also assume using an object that restores sleep need == in a bed
    return ent_info.need_effect[0] > 0.0f;
}