#pragma once

#include <list>
#include <functional>
namespace LitchiRuntime
{

    class Tree {
    public:
        class Node {
        public:
            virtual ~Node() {}

            Node* GetParentNode() { return m_parent; }
            void SetParentNode(Node* parent) { m_parent = parent; }

            std::list<Node*>& GetChildrenList() { return m_childrenList; }

            void AddChildNode(Node* child) {
                //先从之前的父节点移除
                if (child->GetParentNode() != nullptr) {
                    child->GetParentNode()->RemoveChildNode(child);
                }
                m_childrenList.push_back(child);
                child->SetParentNode(this);
            }

            void RemoveChildNode(Node* child) {
                m_childrenList.remove(child);
            }

            bool IsEmpty() {
                return m_childrenList.size() > 0;
            }
        private:
            Node* m_parent = nullptr;//父节点
            std::list<Node*> m_childrenList;//子节点
        };

    public:
        Tree();
        ~Tree();

        Node* GetRootNode() { return m_rootNode; }

        void Post(Node* node, std::function<void(Node*)> func);

        void Find(Node* node_parent, std::function<bool(Node*)> function_check, Node** node_result);
    private:
        Node* m_rootNode;
    };
}
