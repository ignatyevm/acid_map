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
    tree_verifier(const Tree& tree, std::ostream& fails_ostream) : tree_(tree), fails_ostream_(fails_ostream) {}
    bool verify() {
        return verify_node(tree_.root_);
    }
    int deep_height(node_ptr node) {
        if (node == nullptr) {
            return -1;
        }
        return std::max(deep_height(node->left_), deep_height(node->right_)) + 1;
    }
    bool verify_node(node_ptr node) {
        if (node == nullptr) {
            return true;
        }
        node_ptr parent = node->parent_;
        node_ptr left = node->left_;
        node_ptr right = node->right_;
        if (parent != nullptr) {
            if (node.is_left_child() && parent->left_ != node) {
                fails_ostream_ << "parent left node: " << parent->value_.first << " ";
                if (parent->left_ == nullptr) {
                    fails_ostream_ << parent->left_->value_.first << " ";
                }
                fails_ostream_ << node->value_.first << std::endl;
                return false;
            }
            if (node.is_right_child() && parent->right_ != node) {
                fails_ostream_ << "parent right node: " << parent->value_.first << " ";
                if (parent->right_ == nullptr) {
                    fails_ostream_ << parent->right_->value_.first << " ";
                }
                fails_ostream_ << node->value_.first << std::endl;
                return false;
            }
        }
        if (left != nullptr && left->parent_ != node) {
            fails_ostream_ << "left left->parent node: " << left->value_.first << " " << left->parent_->value_.first
                           << " " << node->value_.first << std::endl;
            return false;
        }
        if (right != nullptr && right->parent_ != node) {
            fails_ostream_ << "right right->parent node: " << right->value_.first << " " << right->parent_->value_.first
                           << " " << node->value_.first << std::endl;
            return false;
        }
        int lheight = deep_height(node->left_);
        int rheight = deep_height(node->right_);
        int bf = lheight - rheight;
        if (bf > 1 || bf < -1) {
            fails_ostream_ << "node lh rh " << node->value_.first << " " << lheight << " " << rheight << std::endl;
            return false;
        }
        return verify_node(left) && verify_node(right);
    }
    const Tree& tree_;
    std::ostream& fails_ostream_;
};

template <class Tree>
bool verify_tree(const Tree& tree, std::ostream& fails_ostream) {
    tree_verifier verifier(tree, fails_ostream);
    return verifier.verify();
}

}