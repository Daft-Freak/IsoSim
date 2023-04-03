#include "behaviour-tree.hpp"

BehaviourTreeState::BehaviourTreeState(const Node *root) {
    push_active(root);
}

void BehaviourTreeState::update() {
    for(auto it = active_nodes.begin(); it != active_nodes.end(); ++it) {
        auto status = (*it)->update(*this);

        if(status == behaviour_tree::Status::Running)
            return;

        // return status to parent
        set_variable(behaviour_tree::Var_TempStatus, status);
    }

    // root node finished
    pop_active();
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