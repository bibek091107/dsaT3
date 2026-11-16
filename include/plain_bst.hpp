// plain_bst.hpp
// Unbalanced binary search tree keyed on Item::sku.
// Used as the "no balancing" baseline.
#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "product.hpp"

class PlainBST {
    struct Node {
        Item item;
        std::unique_ptr<Node> lo, hi;
        explicit Node(const Item& it) : item(it) {}
    };

    std::unique_ptr<Node> root_;
    int count_ = 0;

    static int heightOf(const Node* n) {
        if (!n) return 0;
        // explicit stack avoids deep recursion when the tree degenerates into a list
        int best = 0;
        std::vector<std::pair<const Node*, int>> st{{n, 1}};
        while (!st.empty()) {
            auto [cur, d] = st.back();
            st.pop_back();
            if (d > best) best = d;
            if (cur->lo) st.push_back({cur->lo.get(), d + 1});
            if (cur->hi) st.push_back({cur->hi.get(), d + 1});
        }
        return best;
    }

public:
    // Walks down from the root; every visited node costs one key check.
    OpCost add(const Item& it) {
        OpCost c;
        std::unique_ptr<Node>* slot = &root_;
        while (*slot) {
            ++c.keyChecks;
            long k = (*slot)->item.sku;
            if (it.sku == k) return c;                 // duplicate -> rejected
            slot = (it.sku < k) ? &(*slot)->lo : &(*slot)->hi;
        }
        *slot = std::make_unique<Node>(it);
        ++count_;
        c.success = true;
        return c;
    }

    const Item* find(long sku, OpCost& c) const {
        c = OpCost{};
        const Node* cur = root_.get();
        while (cur) {
            ++c.keyChecks;
            if (sku == cur->item.sku) { c.success = true; return &cur->item; }
            cur = (sku < cur->item.sku) ? cur->lo.get() : cur->hi.get();
        }
        return nullptr;
    }

    int height() const { return heightOf(root_.get()); }
    int size() const { return count_; }

    void inOrder(const std::function<void(const Item&)>& visit) const {
        std::vector<const Node*> st;
        const Node* cur = root_.get();
        while (cur || !st.empty()) {
            while (cur) { st.push_back(cur); cur = cur->lo.get(); }
            cur = st.back(); st.pop_back();
            visit(cur->item);
            cur = cur->hi.get();
        }
    }
};
