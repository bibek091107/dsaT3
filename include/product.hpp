// product.hpp
// Record type shared by every index in this project.
#pragma once

#include <string>

struct Item {
    long        sku = 0;        // unique key that every structure is ordered / hashed on
    std::string title;          // what the shelf label says
    std::string aisle;          // store section
    double      mrp = 0.0;      // price in rupees
    int         stock = 0;      // units on hand
};

// Cost counters filled in by every operation so that the three
// structures can be compared on the same footing.
struct OpCost {
    int keyChecks = 0;   // trees: nodes whose key was compared; hash: chain entries inspected
    bool success = false;
};
