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

static const RandomPositionNode random_pos;
static const MoveToNode move_to;
static const ThinkNode think;

static const behaviour_tree::SequenceNode<3> move_sequence({&random_pos, &move_to, &think});

static const behaviour_tree::RepeaterNode tree_root(&move_sequence);

Person::Person(World &world, uint16_t entity_index) : world(world), entity_index(entity_index), behaviour_tree(&tree_root) {
    behaviour_tree.set_variable(PersonVar_EntIndex, entity_index);
    behaviour_tree.set_variable(PersonVar_WorldPtr, &world);
}

void Person::update(uint32_t time) {
    auto &entity = world.get_entity(entity_index);

    // update needs
    get_need(Need::Sleep) -= 0.00001f;

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