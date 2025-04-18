#include "behaviour-tree.hpp"

BehaviourTreeState::BehaviourTreeState(const Node *root) {
    push_active(root);
}

void BehaviourTreeState::update() {
    // push override node if we have returned to the root
    // assumes root node is something non-interruptible
    if(override_node) {
        auto tmp = active_nodes.begin();
        bool root_active = ++tmp == active_nodes.end();

        if(root_active)
            push_active(override_node);
    }

    bool was_override = false;

    for(auto it = active_nodes.begin(); it != active_nodes.end(); ++it) {
        // remove after returning to previous
        if(was_override) {
            was_override = false;
            pop_active();
        }

        auto status = (*it)->update(*this);

        if(status == behaviour_tree::Status::Running)
            return;

        // finished override node
        if(*it == override_node) {
            was_override = true;
            override_node = nullptr;
        }

        // return status to parent
        set_variable(behaviour_tree::Var_TempStatus, status);
    }

    // root node finished
    pop_active();
}

bool BehaviourTreeState::interrupt(const Node *override_node) {
    // don't interrupt an interruption
    if(this->override_node)
        return false;

    // interrupt nodes until we hit something non-interruptible
    for(auto it = active_nodes.begin(); it != active_nodes.end(); ++it) {
        if(!(*it)->interrupt(*this)) {
            // ignore result for root node
            if(++it == active_nodes.end())
                break;
            else
                return false;
            
        }
    }

    this->override_node = override_node;

    return true;
}

void BehaviourTreeState::push_active(const Node *node) {
    active_nodes.push_front(node);
    node->init(*this);
}

void BehaviourTreeState::pop_active(){
    auto node = active_nodes.front();
    active_nodes.pop_front();
    node->deinit(*this);
}

bool BehaviourTreeState::is_active(const Node *node) const {
    return active_nodes.front() == node;
}

const char *BehaviourTreeState::get_attive_node_label() const {
    if(active_nodes.empty())
        return "";

    return active_nodes.front()->get_label();
}

bool BehaviourTreeState::has_variable(uint32_t id) const {
    auto it = variables.find(id);

    return it != variables.end() && it->second.has_value();
}

std::any &BehaviourTreeState::get_variable(uint32_t id) {
    return variables.at(id);
}

void BehaviourTreeState::set_variable(uint32_t id, std::any val) {
    variables[id] = val;
}

void BehaviourTreeState::remove_variable(uint32_t id) {
    variables.erase(id);
}

std::any &BehaviourTreeState::get_node_state(const Node *node) {
    return node_state.at(node);
}

void BehaviourTreeState::create_node_state(const Node *node, std::any val) {
    node_state.emplace(node, val);
}

void BehaviourTreeState::destroy_node_state(const Node *node) {
    node_state.erase(node);
}

namespace behaviour_tree {

    Status RepeaterNode:: update(BehaviourTreeState &state) const {
        if(!state.is_active(child)) {
            state.push_active(child);

            if(child->update(state) == Status::Running)
                return Status::Running;
        }

        state.pop_active();

        return Status::Running;
    }
}