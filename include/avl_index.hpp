// avl_index.hpp
// Height-balanced (AVL) search tree keyed on Item::sku.
// Keeps |height(left) - height(right)| <= 1 at every node and records
// how many rotations of each kind were needed.
#pragma once

#include <algorithm>
#include <functional>
#include "product.hpp"

struct RotationLog {
    int ll = 0, rr = 0, lr = 0, rl = 0;
    int total() const { return ll + rr + lr + rl; }
};

class AVLIndex {
    struct Node {
        Item item;
        Node* lo = nullptr;
        Node* hi = nullptr;
        int ht = 1;
        explicit Node(const Item& it) : item(it) {}
    };

    Node* root_ = nullptr;
    int count_ = 0;
    RotationLog rot_;

    static int h(const Node* n) { return n ? n->ht : 0; }
    static void refresh(Node* n) { n->ht = 1 + std::max(h(n->lo), h(n->hi)); }
    static int tilt(const Node* n) { return h(n->lo) - h(n->hi); }

    static Node* turnRight(Node* top) {        // fixes a left-heavy subtree
        Node* pivot = top->lo;
        top->lo = pivot->hi;
        pivot->hi = top;
        refresh(top);
        refresh(pivot);
        return pivot;
    }
    static Node* turnLeft(Node* top) {         // fixes a right-heavy subtree
        Node* pivot = top->hi;
        top->hi = pivot->lo;
        pivot->lo = top;
        refresh(top);
        refresh(pivot);
        return pivot;
    }

    Node* rebalance(Node* n) {
        refresh(n);
        int t = tilt(n);
        if (t > 1) {
            if (tilt(n->lo) < 0) { n->lo = turnLeft(n->lo); ++rot_.lr; }
            else                 { ++rot_.ll; }
            return turnRight(n);
        }
        if (t < -1) {
            if (tilt(n->hi) > 0) { n->hi = turnRight(n->hi); ++rot_.rl; }
            else                 { ++rot_.rr; }
            return turnLeft(n);
        }
        return n;
    }

    Node* place(Node* n, const Item& it, OpCost& c) {
        if (!n) { c.success = true; ++count_; return new Node(it); }
        ++c.keyChecks;
        if (it.sku == n->item.sku) return n;          // duplicate -> unchanged
        if (it.sku < n->item.sku) n->lo = place(n->lo, it, c);
        else                      n->hi = place(n->hi, it, c);
        return c.success ? rebalance(n) : n;
    }

    static void destroy(Node* n) {
        if (!n) return;
        destroy(n->lo);
        destroy(n->hi);
        delete n;
    }

    static void walk(const Node* n, const std::function<void(const Item&)>& f) {
        if (!n) return;
        walk(n->lo, f);
        f(n->item);
        walk(n->hi, f);
    }

    static void rangeWalk(const Node* n, long a, long b, const std::function<void(const Item&)>& f, int& visited) {
        if (!n) return;
        ++visited;
        if (a < n->item.sku) rangeWalk(n->lo, a, b, f, visited);
        if (a <= n->item.sku && n->item.sku <= b) f(n->item);
        if (n->item.sku < b) rangeWalk(n->hi, a, b, f, visited);
    }

public:
    AVLIndex() = default;
    AVLIndex(const AVLIndex&) = delete;
    AVLIndex& operator=(const AVLIndex&) = delete;
    ~AVLIndex() { destroy(root_); }

    OpCost add(const Item& it) {
        OpCost c;
        root_ = place(root_, it, c);
        return c;
    }

    const Item* find(long sku, OpCost& c) const {
        c = OpCost{};
        const Node* cur = root_;
        while (cur) {
            ++c.keyChecks;
            if (sku == cur->item.sku) { c.success = true; return &cur->item; }
            cur = (sku < cur->item.sku) ? cur->lo : cur->hi;
        }
        return nullptr;
    }

    // Lists every SKU in [a, b] in ascending order; returns nodes visited.
    int range(long a, long b, const std::function<void(const Item&)>& f) const {
        int visited = 0;
        rangeWalk(root_, a, b, f, visited);
        return visited;
    }

    void inOrder(const std::function<void(const Item&)>& f) const { walk(root_, f); }
    int height() const { return h(root_); }
    int size() const { return count_; }
    const RotationLog& rotations() const { return rot_; }
};
