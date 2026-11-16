// catalog.cpp
#include "catalog.hpp"

#include <algorithm>
#include <random>

namespace catalog {

std::vector<Item> deliveryOrder() {
    return {
        // ---- dairy supplier delivery: consecutive SKUs ----
        {20001, "Toned Milk 1L",            "Dairy",        56.0, 120},
        {20002, "Full Cream Milk 1L",       "Dairy",        68.0,  90},
        {20003, "Curd 400g",                "Dairy",        45.0,  75},
        {20004, "Paneer 200g",              "Dairy",        95.0,  40},
        {20005, "Salted Butter 100g",       "Dairy",        58.0,  60},
        {20006, "Cheese Slices 10pc",       "Dairy",       145.0,  35},
        {20007, "Buttermilk 500ml",         "Dairy",        25.0,  80},
        {20008, "Ghee 500ml",               "Dairy",       340.0,  25},
        {20009, "Flavoured Yogurt 100g",    "Dairy",        35.0,  70},
        {20010, "Fresh Cream 200ml",        "Dairy",        72.0,  30},
        {20011, "Lassi 200ml",              "Dairy",        30.0,  65},
        {20012, "Milk Powder 500g",         "Dairy",       265.0,  20},
        {20013, "Condensed Milk 400g",      "Dairy",       135.0,  28},
        {20014, "Greek Yogurt 400g",        "Dairy",       180.0,  18},
        {20015, "Mozzarella 200g",          "Dairy",       175.0,  22},
        {20016, "Probiotic Drink 65ml",     "Dairy",        20.0, 150},
        // ---- regular stock: scattered SKUs ----
        {48213, "Basmati Rice 5kg",         "Staples",     649.0,  30},
        {31570, "Toor Dal 1kg",             "Staples",     165.0,  55},
        {76402, "Sunflower Oil 1L",         "Staples",     155.0,  48},
        {12988, "Iodised Salt 1kg",         "Staples",      28.0, 200},
        {59341, "Wheat Atta 10kg",          "Staples",     489.0,  26},
        {84127, "Green Tea 100 bags",       "Beverages",   299.0,  33},
        {23756, "Instant Coffee 100g",      "Beverages",   310.0,  27},
        {67015, "Orange Juice 1L",          "Beverages",   120.0,  44},
        {91834, "Mineral Water 1L",         "Beverages",    20.0, 300},
        {40562, "Digestive Biscuits",       "Snacks",       45.0, 110},
        {15309, "Potato Chips 90g",         "Snacks",       30.0, 140},
        {72688, "Roasted Almonds 250g",     "Snacks",      349.0,  19},
        {35941, "Dark Chocolate 100g",      "Snacks",      175.0,  52},
        {98450, "Dishwash Liquid 750ml",    "Household",   199.0,  38},
        {54207, "Detergent Powder 2kg",     "Household",   385.0,  24},
        {27833, "Floor Cleaner 1L",         "Household",   215.0,  31},
        {63579, "Garbage Bags 30pc",        "Household",    99.0,  60},
        {81296, "Toothpaste 150g",          "Personal Care",105.0, 85},
        {44718, "Shampoo 340ml",            "Personal Care",265.0, 42},
        {19064, "Bath Soap 4pc",            "Personal Care",160.0, 73},
        {69921, "Hand Wash Refill",         "Personal Care", 99.0, 58},
        {38485, "Paracetamol Strip",        "Pharmacy",     30.0,  95},
        {87352, "Antiseptic Liquid 250ml",  "Pharmacy",    120.0,  36},
        {52690, "ORS Sachet",               "Pharmacy",     22.0, 130},
        {11427, "Bandage Roll",             "Pharmacy",     40.0,  64},
        {95173, "Hand Sanitizer 500ml",     "Pharmacy",    185.0,  29},
        {33019, "Frozen Peas 500g",         "Frozen",       95.0,  37},
        {78564, "Veg Nuggets 400g",         "Frozen",      199.0,  21},
        {46830, "Ice Cream Tub 1L",         "Frozen",      249.0,  17},
        {25147, "Bread Loaf 400g",          "Bakery",       45.0,  50},
        {61908, "Multigrain Rusk 300g",     "Bakery",       70.0,  46},
        {89736, "Fruit Cake 250g",          "Bakery",      110.0,  23},
    };
}

std::vector<Item> sortedOrder() {
    auto v = deliveryOrder();
    std::sort(v.begin(), v.end(), [](const Item& a, const Item& b) { return a.sku < b.sku; });
    return v;
}

std::vector<Item> shuffledOrder(unsigned seed) {
    // Hand-written Fisher-Yates so the order is identical on every compiler
    // (std::shuffle's algorithm is implementation-defined).
    auto v = deliveryOrder();
    std::mt19937 rng(seed);
    for (std::size_t i = v.size() - 1; i > 0; --i) {
        std::size_t j = rng() % (i + 1);
        std::swap(v[i], v[j]);
    }
    return v;
}

std::vector<long> presentQueries() { return {20001, 20009, 20016, 11427, 52690, 98450}; }
std::vector<long> absentQueries()  { return {20017, 19999, 10000, 50000, 99999, 33333}; }

}  // namespace catalog
