// catalog.hpp
// Sample inventory of a neighbourhood supermarket ("FreshKart") and the
// three arrival orders used in the experiment.
#pragma once

#include <string>
#include <vector>
#include "product.hpp"

namespace catalog {

// 48 items: a 16-item dairy delivery with back-to-back SKUs (20001-20016)
// followed by 32 items whose SKUs are spread across 10000-99999.
std::vector<Item> deliveryOrder();

// Same items sorted by SKU (what an exported spreadsheet usually looks like).
std::vector<Item> sortedOrder();

// Same items in a reproducible random order (Fisher-Yates, mt19937, seed 2026).
std::vector<Item> shuffledOrder(unsigned seed = 2026);

// SKUs searched in every scenario.
std::vector<long> presentQueries();
std::vector<long> absentQueries();

}  // namespace catalog
