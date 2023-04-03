#pragma once

#include <any>
#include <cstdint>
#include <forward_list>
#include <unordered_map>

namespace behaviour_tree {
    class Node;
}

class BehaviourTreeState {
public:
    using Node = behaviour_tree::Node;

    BehaviourTreeState(const Node *root);

    void update();

    void push_active(const Node *node);
    void pop_active();
    bool is_active(const Node *node) const;

    const char *get_attive_node_label() const;

    bool has_variable(uint32_t id) const;
    std::any &get_variable(uint32_t id);
    void set_variable(uint32_t id, std::any val);
    void remove_variable(uint32_t id);

    std::any &get_node_state(const Node *node);
    void create_node_state(const Node *node, std::any val);
    void destroy_node_state(const Node *node);

private:
    std::forward_list<const Node *> active_nodes;

    std::unordered_map<uint32_t, std::any> variables;
    std::unordered_map<const Node *, std::any> node_state;
};

namespace behaviour_tree {
    enum class Status {
        Success,
        Failed,
        Running,
    };

    enum VariableID {
        Var_TempStatus = 0, // used for passing status back to composite node

        Var_User
    };

    class Node {
    public:
        virtual void init(BehaviourTreeState &state) const = 0;
        virtual void deinit(BehaviourTreeState &state) const = 0;

        virtual Status update(BehaviourTreeState &state) const = 0;

        virtual bool interrupt(BehaviourTreeState &state) const = 0;

        virtual const char *get_label() const = 0;
    };

    template<std::size_t N>
    class CompositeNode : public Node {
    public:
        constexpr CompositeNode(std::array<const Node *, N> &&children) : children(std::move(children)) {}

        std::array<const Node *, N> children;
    };

    class DecoratorNode : public Node {
    public:
        constexpr DecoratorNode(const Node *child) : child(child){}

        const Node *child;
    };

    template<std::size_t N>
    class SequenceNode final : public CompositeNode<N> {
    public:
        using CompositeNode<N>::CompositeNode;

        void init(BehaviourTreeState &state) const override {
            state.create_node_state(this, this->children.begin());
        }
        void deinit(BehaviourTreeState &state) const override {
            state.destroy_node_state(this);
        }

        Status update(BehaviourTreeState &state) const override {
            // run children in order, fail if any fails
            using ChildIterator = typename decltype(this->children)::const_iterator;

            auto &it = std::any_cast<ChildIterator &>(state.get_node_state(this));

            for(; it != this->children.end(); ++it) {

                bool is_resume = state.is_active(*it);
                Status status = Status::Success;
                if(is_resume) {
                    status = std::any_cast<Status>(state.get_variable(Var_TempStatus));
                } else {
                    state.push_active(*it);
                    status = (*it)->update(state);

                    if(status == Status::Running)
                        return status; // in progress, exit and resume later
                }

                state.pop_active();
                
                if(status == Status::Failed)
                    return status;
            }
            return Status::Success;
        }

        bool interrupt(BehaviourTreeState &state) const override {
            return true; // child failure will cause this to stop anyway
        }

        const char *get_label() const override {
            return "Sequence";
        }
    };

    template<std::size_t N>
    class SelectorNode : public CompositeNode<N> {
    public:
        using CompositeNode<N>::CompositeNode;

        using ChildIterator = typename decltype(SelectorNode::children)::const_iterator;
        struct State {
            ChildIterator it;
            bool interrupted;
        };

        void init(BehaviourTreeState &state) const override {
            state.create_node_state(this, State{this->children.begin(), false});
        }

        void deinit(BehaviourTreeState &state) const override {
            state.destroy_node_state(this);
        }

        Status update(BehaviourTreeState &state) const override {
            // run children in order, stop at first success
            auto &node_state = std::any_cast<State &>(state.get_node_state(this));
            auto &it = node_state.it;

            for(; it != this->children.end(); ++it) {

                bool is_resume = state.is_active(*it);
                Status status = Status::Success;
                if(is_resume) {
                    status = std::any_cast<Status>(state.get_variable(Var_TempStatus));
                } else {
                    state.push_active(*it);
                    status = (*it)->update(state);

                    if(status == Status::Running)
                        return status; // in progress, exit and resume later
                }

                state.pop_active();
                
                if(status == Status::Success)
                    return status;

                if(node_state.interrupted)
                    return Status::Failed;
            }
            return Status::Failed;
        }

        bool interrupt(BehaviourTreeState &state) const override {
            auto &node_state = std::any_cast<State &>(state.get_node_state(this));
            node_state.interrupted = true;

            return true;
        }

        const char *get_label() const override {
            return "Selector";
        }
    };

    class RepeaterNode final : public DecoratorNode {
    public:
        using DecoratorNode::DecoratorNode;

        void init(BehaviourTreeState &state) const override {}
        void deinit(BehaviourTreeState &state) const override {}

        Status update(BehaviourTreeState &state) const override;

        bool interrupt(BehaviourTreeState &state) const override {
            return false; // this node does not understand failure
        }

        const char *get_label() const override {
            return "Repeater";
        }
    };
}