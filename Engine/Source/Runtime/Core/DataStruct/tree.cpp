
#include "tree.h"
namespace LitchiRuntime
{
    Tree::Tree() {
        m_rootNode = new Node();
    }

    void Tree::Post(Node* node, std::function<void(Node*)> func) {
        for (auto child : node->GetChildrenList()) {
            Post(child, func);
        }
        if (node != m_rootNode) {
            func(node);
        }
    }

    Tree::~Tree() = default;

    void Tree::Find(Node* node_parent, std::function<bool(Node*)> function_check, Node** node_result = nullptr) {
        for (auto child : node_parent->GetChildrenList()) {
            Find(child, function_check, node_result);
            if (*node_result != nullptr) {
                break;
            }
        }
        if (*node_result != nullptr) {
            return;
        }
        if (function_check(node_parent)) {
            (*node_result) = node_parent;
        }
    }
}
