#include "engine/engine.hpp"

#include "person.hpp"
#include "world.hpp"


enum PersonBTVariables {
    PersonVar_EntIndex = behaviour_tree::Var_User,
    PersonVar_WorldPtr,
    PersonVar_Position
};

class RandomPositionNode final : public behaviour_tree::Node {
public:

    void init(BehaviourTreeState &state) const override {}

    void deinit(BehaviourTreeState &state) const override {}

    behaviour_tree::Status update(BehaviourTreeState &state) const override {
        auto world = std::any_cast<World *>(state.get_variable(PersonVar_WorldPtr));

        auto bounds = world->get_bounds();
        blit::Point target_pos(blit::random() % bounds.w, blit::random() % bounds.h);

        state.set_variable(PersonVar_Position, target_pos);

        return behaviour_tree::Status::Success;
    };
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

        // adjust to the tile in front
        switch(ent.get_rotation()) {
            case 0: target_pos.y++; break;
            case 1: target_pos.x++; break;
            case 2: target_pos.y--; break;
            case 3: target_pos.x--; break;
        }

        state.set_variable(PersonVar_Position, target_pos);

        return Status::Success;
    };

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

        node_state->path_id = world->get_path_finder().start_path_find(start_pos, target_pos, {1, 1});
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
    };
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
    };
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

        // TODO: fall back to lowest need?

        return person->get_need(need) < 0.2f ? Status::Success : Status::Failed;
    };

private:
    Person::Need need;
};

class UseUntilNeedRestoredNode final : public behaviour_tree::Node {
public:
    constexpr UseUntilNeedRestoredNode(Person::Need need) : need(need){}

    struct State {
        blit::Point orig_pos;
        bool started_use = false;
    };

    void init(BehaviourTreeState &state) const override {
        auto entity_index = std::any_cast<uint16_t>(state.get_variable(PersonVar_EntIndex));
        auto world = std::any_cast<World *>(state.get_variable(PersonVar_WorldPtr));

        State node_state;
        node_state.orig_pos = world->get_entity(entity_index).get_tile_position();

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
        bool in_object = world->has_entity_for_need(ent.get_tile_position(), need);

        if(!in_object && !node_state.started_use) {
            // find the object
            static const blit::Point offsets[]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

            for(auto &off : offsets) {
                if(world->has_entity_for_need(ent.get_tile_position() + off, need)) {
                    // get in
                    person->move_to_tile(ent.get_tile_position() + off);
                    break;
                }
            }

            // couldn't find it, fail
            if(!person->is_moving())
                return Status::Failed;

            node_state.started_use = true;
            return Status::Running;
        } else if(in_object && person->get_need(need) == 1.0f) {
            // get out
            person->move_to_tile(node_state.orig_pos);
            return Status::Running;
        }

        return in_object ? Status::Running : Status::Success;
    };

private:
    Person::Need need;
};

static const RandomPositionNode random_pos;
static const MoveToNode move_to;
static const ThinkNode think;

static const behaviour_tree::SequenceNode<3> move_sequence({&random_pos, &move_to, &think});

static const IsNeedLowNode check_tired(Person::Need::Sleep);
static const FindEntityPositionForNeed find_bed(Person::Need::Sleep);
static const UseUntilNeedRestoredNode sleep(Person::Need::Sleep);

// TODO: also check if it's late?
static const behaviour_tree::SequenceNode<4> sleep_sequence({&check_tired, &find_bed, &move_to, &sleep});

static const behaviour_tree::SelectorNode<2> idle_selector({&sleep_sequence, &move_sequence});

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
    auto ents = world.get_entities_on_tile(entity.get_tile_position());

    for(auto ent_id : ents) {
        auto &ent_info = world.get_entity(ent_id).get_info();

        is_sleeping = is_sleeping || ent_info.need_effect[0] > 0.0f; // assume in bed == sleeping

        for(size_t i = 0; i < std::size(needs); i++) {
            needs[i] += ent_info.need_effect[i];
        }
    }

    float sleep_adjust = is_sleeping ? 0.5f : 1.0f;

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

uint16_t Person::get_entity_index() const {
    return entity_index;
}

float &Person::get_need(Need need) {
    return needs[static_cast<int>(need)];
}