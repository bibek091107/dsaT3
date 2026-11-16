// chained_hash.hpp
// Hash table with separate chaining (each bucket is a singly linked list).
// Key -> bucket uses the multiplicative (Knuth) method so that runs of
// consecutive SKUs are spread over the table instead of landing side by side.
#pragma once

#include <cstdint>
#include <forward_list>
#include <vector>
#include "product.hpp"

class ChainedHash {
    std::vector<std::forward_list<Item>> buckets_;
    int count_ = 0;
    int collisions_ = 0;    // inserts that landed in a bucket that was already occupied

public:
    explicit ChainedHash(std::size_t bucketCount = 61) : buckets_(bucketCount) {}

    std::size_t bucketOf(long sku) const {
        // Knuth multiplicative hashing: (k * A) mod 2^32, then scale to the table size.
        const std::uint32_t A = 2654435761u;
        std::uint32_t mixed = static_cast<std::uint32_t>(sku) * A;
        return static_cast<std::size_t>((static_cast<std::uint64_t>(mixed) * buckets_.size()) >> 32);
    }

    OpCost add(const Item& it) {
        OpCost c;
        auto& chain = buckets_[bucketOf(it.sku)];
        for (const Item& e : chain) {
            ++c.keyChecks;
            if (e.sku == it.sku) return c;                // duplicate -> rejected
        }
        if (!chain.empty()) ++collisions_;
        chain.push_front(it);
        ++count_;
        c.success = true;
        return c;
    }

    // Every chain entry inspected counts as one key check.
    // An empty bucket still costs 1 (reading the bucket head).
    const Item* find(long sku, OpCost& c) const {
        c = OpCost{};
        const auto& chain = buckets_[bucketOf(sku)];
        if (chain.empty()) { c.keyChecks = 1; return nullptr; }
        for (const Item& e : chain) {
            ++c.keyChecks;
            if (e.sku == sku) { c.success = true; return &e; }
        }
        return nullptr;
    }

    int size() const { return count_; }
    int collisions() const { return collisions_; }
    std::size_t bucketCount() const { return buckets_.size(); }
    double loadFactor() const { return static_cast<double>(count_) / buckets_.size(); }

    int longestChain() const {
        int best = 0;
        for (const auto& b : buckets_) {
            int len = 0;
            for (auto it = b.begin(); it != b.end(); ++it) ++len;
            if (len > best) best = len;
        }
        return best;
    }

    int emptyBuckets() const {
        int n = 0;
        for (const auto& b : buckets_) if (b.empty()) ++n;
        return n;
    }
};
