#pragma once

#include <common/message_format.h>

namespace atto {

    namespace bst_api {

        struct Node {
            uint64_t key = 0;
            MessageFormat value;
            Node* left = nullptr;
            Node* right = nullptr;
        };

        // A utility function to create a new BST node
        inline Node * makeNode(uint64_t key, const MessageFormat & value)
        {
            Node * temp = new Node;
            temp->key = key;
            temp->value = value;
            temp->left = nullptr;
            temp->right = nullptr;
            return temp;
        }

        // A utility function to do inorder traversal of BST
        inline void toString(Node * root, std::string & s)
        {
            if (root != nullptr) {
                toString(root->left, s);
                {
                    s += "[";
                    s += std::to_string(root->key);
                    s += "] = ";
                    s += atto::toStringCompact(root->value);
                    s += "\n";
                }
                toString(root->right, s);
            }
        }

        inline std::string toString(Node * node)
        {
            std::string str;
            toString(node, str);
            return str;
        }

        // C++ function to search a given key in a given BST
        inline Node * search(Node * root, uint64_t key)
        {
            // Base Cases: root is null or key is present at root
            if (root == nullptr || root->key == key) {
               return root;
            }

            // Key is greater than root's key
            if (root->key < key) {
               return search(root->right, key);
            }

            // Key is smaller than root's key
            return search(root->left, key);
        }

        // A utility function to insert a new node with given key in BST
        inline Node * insert(Node * node, uint64_t key, const MessageFormat & msg)
        {
            // If the tree is empty, return a new node
            if (node == nullptr) {
                return makeNode(key, msg);
            }

            // Otherwise, recur down the tre
            if (key < node->key) {
                node->left = insert(node->left, key, msg);
            } else if (key > node->key) {
                node->right = insert(node->right, key, msg);
            }

            // return the (unchanged) node pointer
            return node;
        }

        // Look for a leftmost node
        Node * minValueNode(Node * node)
        {
            Node * current = node;

            while (current && current->left != nullptr) {
                current = current->left;
            }

            return current;
        }

        // Given a binary search tree and a key, this function deletes the key and returns the new root
        Node * deleteNode(Node * root, uint64_t key)
        {
            // base case
            if (root == nullptr) {
                return root;
            }

            // If the key to be deleted is smaller than the root's key,
            // then it lies in left subtree
            if (key < root->key) {
                root->left = deleteNode(root->left, key);
            }

            // If the key to be deleted is greater than the root's key,
            // then it lies in right subtree
            else if (key > root->key) {
                root->right = deleteNode(root->right, key);
            }

            // if key is same as root's key, then This is the node to be deleted
            else {
                // node with only one child or no child
                if (root->left == nullptr) {
                    Node *temp = root->right;
                    delete root;
                    return temp;
                } else if (root->right == nullptr) {
                    Node *temp = root->left;
                    delete root;
                    return temp;
                }

                // node with two children: Get the inorder successor (smallest in the right subtree)
                Node* temp = minValueNode(root->right);

                // Copy the inorder successor's content to this node
                root->key = temp->key;

                // Delete the inorder successor
                root->right = deleteNode(root->right, temp->key);
            }
            return root;
        }
    }
}
