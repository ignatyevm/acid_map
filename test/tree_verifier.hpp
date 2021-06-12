#pragma once

#include "acid_map.hpp"

#include <iostream>

namespace polyndrom {

template <class Tree>
bool verify_tree(const Tree& tree, std::ostream& fails_ostream = std::cout);

template <class Tree>
class tree_verifier {
public:
    using node_ptr = typename Tree::node_ptr;
    tree_verifier(const Tree& tree, std::ostream& fails_ostream) : tree(tree), fails_ostream(fails_ostream) {}
    bool verify() {
        return verify_node(tree.root);
    }
    int deep_height(node_ptr node) {
        if (node == nullptr) {
            return -1;
        }
        return std::max(deep_height(node->left), deep_height(node->right)) + 1;
    }
    bool verify_node(node_ptr node) {
        if (node == nullptr) {
            return true;
        }
        node_ptr parent = node->parent;
        node_ptr left = node->left;
        node_ptr right = node->right;
        if (parent != nullptr) {
            if (node->parent->left == node && parent->left != node) {
                fails_ostream << "parent left node: " << parent->value.first << " ";
                if (parent->left == nullptr) {
                    fails_ostream << parent->left->value.first << " ";
                }
                fails_ostream << node->value.first << std::endl;
                return false;
            }
            if (node->parent->right == node && parent->right != node) {
                fails_ostream << "parent right node: " << parent->value.first << " ";
                if (parent->right == nullptr) {
                    fails_ostream << parent->right->value.first << " ";
                }
                fails_ostream << node->value.first << std::endl;
                return false;
            }
        }
        if (left != nullptr && left->parent != node) {
            fails_ostream << "left left->parent node: " << left->value.first << " " << left->parent->value.first
                           << " " << node->value.first << std::endl;
            return false;
        }
        if (right != nullptr && right->parent != node) {
            fails_ostream << "right right->parent node: " << right->value.first << " " << right->parent->value.first
                           << " " << node->value.first << std::endl;
            return false;
        }
        int lheight = deep_height(node->left);
        int rheight = deep_height(node->right);
        int bf = lheight - rheight;
        if (bf > 1 || bf < -1) {
            fails_ostream << "node lh rh " << node->value.first << " " << lheight << " " << rheight << std::endl;
            return false;
        }
        return verify_node(left) && verify_node(right);
    }
    const Tree& tree;
    std::ostream& fails_ostream;
};

template <class Tree>
bool verify_tree(const Tree& tree, std::ostream& fails_ostream) {
    tree_verifier verifier(tree, fails_ostream);
    return verifier.verify();
}

}