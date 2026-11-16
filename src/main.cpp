// main.cpp
// Product ID Search Arena: BST vs AVL tree vs chained hash table.
//
//   ./arena           run the full experiment and write CSV files to results/
//   ./arena --menu    interactive mode for live demonstration
//
// 21CS201J Data Structures and Algorithms - Algorithm Battle

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "avl_index.hpp"
#include "catalog.hpp"
#include "chained_hash.hpp"
#include "plain_bst.hpp"

namespace {

constexpr std::size_t kBuckets = 61;   // prime; 48 items -> load factor ~0.79

struct Arena {
    PlainBST    bst;
    AVLIndex    avl;
    ChainedHash hash{kBuckets};
    long bstBuildChecks = 0, avlBuildChecks = 0, hashBuildChecks = 0;

    // Returns how many structures accepted the item (3 = new, 0 = duplicate).
    int add(const Item& it) {
        OpCost a = bst.add(it), b = avl.add(it), c = hash.add(it);
        bstBuildChecks += a.keyChecks;
        avlBuildChecks += b.keyChecks;
        hashBuildChecks += c.keyChecks;
        return a.success + b.success + c.success;
    }
};

struct QueryRow {
    long sku;
    bool found;
    int bst, avl, hash;
};

std::string rule(int w = 72, char ch = '-') { return std::string(w, ch); }

int idealHeight(int n) { return static_cast<int>(std::ceil(std::log2(n + 1.0))); }

std::vector<QueryRow> runQueries(const Arena& a, const std::vector<long>& keys) {
    std::vector<QueryRow> rows;
    for (long k : keys) {
        OpCost cb, ca, ch;
        const Item* hit = a.bst.find(k, cb);
        a.avl.find(k, ca);
        a.hash.find(k, ch);
        rows.push_back({k, hit != nullptr, cb.keyChecks, ca.keyChecks, ch.keyChecks});
    }
    return rows;
}

void printQueryTable(const std::string& heading, const std::vector<QueryRow>& rows) {
    std::cout << "  " << heading << "\n";
    std::cout << "  " << std::left << std::setw(10) << "SKU" << std::setw(10) << "Result" << std::right
              << std::setw(8) << "BST" << std::setw(8) << "AVL" << std::setw(8) << "Hash" << "\n";
    double sb = 0, sa = 0, sh = 0;
    for (const auto& r : rows) {
        std::cout << "  " << std::left << std::setw(10) << r.sku << std::setw(10) << (r.found ? "found" : "absent")
                  << std::right << std::setw(8) << r.bst << std::setw(8) << r.avl << std::setw(8) << r.hash << "\n";
        sb += r.bst; sa += r.avl; sh += r.hash;
    }
    double n = static_cast<double>(rows.size());
    std::cout << "  " << std::left << std::setw(20) << "average" << std::right << std::fixed << std::setprecision(2)
              << std::setw(8) << sb / n << std::setw(8) << sa / n << std::setw(8) << sh / n << "\n\n";
    std::cout.unsetf(std::ios::fixed);
}

void runScenario(const std::string& tag, const std::string& title, const std::vector<Item>& items,
                 std::ofstream& buildCsv, std::ofstream& searchCsv) {
    Arena a;
    for (const auto& it : items) a.add(it);

    std::cout << rule(72, '=') << "\n Scenario " << tag << " : " << title << "\n" << rule(72, '=') << "\n";
    std::cout << "  First 6 SKUs inserted: ";
    for (std::size_t i = 0; i < 6 && i < items.size(); ++i) std::cout << items[i].sku << ' ';
    std::cout << "...\n\n";

    const auto& r = a.avl.rotations();
    std::cout << "  Build statistics (" << a.avl.size() << " items, ideal tree height "
              << idealHeight(a.avl.size()) << ")\n";
    std::cout << "    BST  : height " << std::setw(2) << a.bst.height() << "   key checks while inserting "
              << a.bstBuildChecks << "\n";
    std::cout << "    AVL  : height " << std::setw(2) << a.avl.height() << "   key checks while inserting "
              << a.avlBuildChecks << "   rotations " << r.total() << " (LL " << r.ll << ", RR " << r.rr
              << ", LR " << r.lr << ", RL " << r.rl << ")\n";
    std::cout << "    Hash : " << a.hash.bucketCount() << " buckets, load factor " << std::fixed
              << std::setprecision(2) << a.hash.loadFactor() << ", collisions " << a.hash.collisions()
              << ", longest chain " << a.hash.longestChain() << ", empty buckets " << a.hash.emptyBuckets()
              << "\n\n";
    std::cout.unsetf(std::ios::fixed);

    buildCsv << tag << ',' << a.bst.height() << ',' << a.bstBuildChecks << ',' << a.avl.height() << ','
             << a.avlBuildChecks << ',' << r.total() << ',' << a.hash.collisions() << ',' << a.hash.longestChain()
             << ',' << a.hashBuildChecks << '\n';

    auto hits = runQueries(a, catalog::presentQueries());
    auto misses = runQueries(a, catalog::absentQueries());
    printQueryTable("Successful searches (key checks)", hits);
    printQueryTable("Unsuccessful searches (key checks)", misses);
    for (const auto* set : {&hits, &misses})
        for (const auto& q : *set)
            searchCsv << tag << ',' << q.sku << ',' << (q.found ? "found" : "absent") << ',' << q.bst << ','
                      << q.avl << ',' << q.hash << '\n';
}

void rangeDemo() {
    Arena a;
    for (const auto& it : catalog::deliveryOrder()) a.add(it);
    std::cout << rule(72, '=') << "\n Extra: range query  \"all SKUs from 20005 to 20012\"\n" << rule(72, '=') << "\n";
    int listed = 0;
    int visited = a.avl.range(20005, 20012, [&](const Item& it) {
        std::cout << "    " << it.sku << "  " << it.title << "\n";
        ++listed;
    });
    std::cout << "  AVL tree : " << listed << " items listed after visiting " << visited << " nodes\n";
    std::cout << "  Hash     : no key order, every one of the " << a.hash.size()
              << " entries in " << a.hash.bucketCount() << " buckets must be scanned\n\n";
}

int runExperiment() {
    std::filesystem::create_directories("results");
    std::ofstream buildCsv("results/build_stats.csv"), searchCsv("results/search_costs.csv");
    buildCsv << "scenario,bst_height,bst_insert_checks,avl_height,avl_insert_checks,avl_rotations,"
                "hash_collisions,hash_longest_chain,hash_insert_checks\n";
    searchCsv << "scenario,sku,result,bst_checks,avl_checks,hash_checks\n";

    std::cout << "\nPRODUCT ID SEARCH ARENA  -  BST vs AVL vs Chained Hashing\n";
    std::cout << "Catalogue: FreshKart supermarket, " << catalog::deliveryOrder().size() << " items\n\n";

    runScenario("A", "delivery order (16 consecutive dairy SKUs first)", catalog::deliveryOrder(), buildCsv, searchCsv);
    runScenario("B", "sorted import (ascending SKUs)", catalog::sortedOrder(), buildCsv, searchCsv);
    runScenario("C", "shuffled arrival (seed 2026)", catalog::shuffledOrder(), buildCsv, searchCsv);
    rangeDemo();
    std::cout << "CSV files written: results/build_stats.csv, results/search_costs.csv\n";
    return 0;
}

// ---------------------------------------------------------------- menu mode
long readLong(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        long v;
        if (std::cin >> v) { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); return v; }
        if (std::cin.eof()) std::exit(0);
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  please enter a number\n";
    }
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

void showItem(const Item& it) {
    std::cout << "  " << it.sku << " | " << it.title << " | " << it.aisle << " | Rs " << std::fixed
              << std::setprecision(2) << it.mrp << " | stock " << it.stock << "\n";
    std::cout.unsetf(std::ios::fixed);
}

int runMenu() {
    Arena a;
    for (const auto& it : catalog::deliveryOrder()) a.add(it);
    std::cout << "Loaded " << a.avl.size() << " items into BST, AVL and hash table.\n";
    while (true) {
        std::cout << "\n[1] search SKU  [2] add item  [3] list sorted  [4] range query  [5] stats  [0] quit\n";
        long choice = readLong("> ");
        if (choice == 0) return 0;
        if (choice == 1) {
            long k = readLong("SKU to search: ");
            OpCost cb, ca, ch;
            const Item* p = a.bst.find(k, cb);
            a.avl.find(k, ca);
            a.hash.find(k, ch);
            if (p) showItem(*p); else std::cout << "  SKU " << k << " is not in the catalogue\n";
            std::cout << "  key checks -> BST " << cb.keyChecks << ", AVL " << ca.keyChecks << ", Hash "
                      << ch.keyChecks << " (bucket " << a.hash.bucketOf(k) << ")\n";
        } else if (choice == 2) {
            Item it;
            it.sku = readLong("New SKU: ");
            OpCost probe;
            if (a.avl.find(it.sku, probe)) { std::cout << "  SKU " << it.sku << " already exists - not added\n"; continue; }
            it.title = readLine("Name   : ");
            it.aisle = readLine("Aisle  : ");
            it.mrp = static_cast<double>(readLong("Price (whole rupees): "));
            it.stock = static_cast<int>(readLong("Stock  : "));
            a.add(it);
            std::cout << "  added. BST height " << a.bst.height() << ", AVL height " << a.avl.height()
                      << ", hash collisions " << a.hash.collisions() << "\n";
        } else if (choice == 3) {
            a.avl.inOrder(showItem);
        } else if (choice == 4) {
            long lo = readLong("From SKU: "), hi = readLong("To SKU  : ");
            int visited = a.avl.range(lo, hi, showItem);
            std::cout << "  (AVL visited " << visited << " nodes)\n";
        } else if (choice == 5) {
            std::cout << "  items " << a.avl.size() << " | BST height " << a.bst.height() << " | AVL height "
                      << a.avl.height() << " | rotations " << a.avl.rotations().total() << " | load factor "
                      << std::fixed << std::setprecision(2) << a.hash.loadFactor() << " | collisions "
                      << a.hash.collisions() << "\n";
            std::cout.unsetf(std::ios::fixed);
        } else {
            std::cout << "  unknown option\n";
        }
    }
}

}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--menu") return runMenu();
    return runExperiment();
}
