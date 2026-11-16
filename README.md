# Product ID Search Arena
### BST vs AVL Tree vs Chained Hashing: who finds a SKU first?

| | |
|---|---|
| **Student** | Rahul Mahato |
| **Register No.** | RA2511026050321 |
| **Programme** | B.Tech – Artificial Intelligence and Machine Learning, II Year / III Semester (Section AIML-B) |
| **Course** | 21CS201J – Data Structures and Algorithms (Algorithm Battle) |
| **Institute** | SRM Institute of Science and Technology, Tiruchirappalli |
| **Language / Build** | C++17, GNU Make, no external libraries |
| **SDG link** | SDG 12 – Responsible Consumption and Production (with support for SDG 9) |

---

## 1. The problem I picked

Picture the billing desk of a small supermarket, which I call **FreshKart**. Every time a cashier scans a packet, the software receives a five-digit SKU and has to return the item's name, aisle, price and stock level before the next packet is on the counter. The same lookup runs again when shelves are restocked and when expired items are pulled out.

Two features of real stock data make this harder than it looks:

* **Supplier batches arrive with back-to-back SKUs.** In my catalogue a dairy vendor's delivery uses `20001`–`20016` with no gaps.
* **Catalogues are often imported from a sorted spreadsheet.** That means keys can enter the program in strictly increasing order.

The question for this battle is: **which index structure gives the cheapest and most predictable SKU lookup under these conditions?**

### Why it matters (SDG 12 / SDG 9)
Quick, reliable stock lookups let a shop track what is on its shelves, reorder in time and spot items close to expiry. Fewer items are over-stocked or thrown away, which is the goal of SDG 12. Software that does this efficiently on modest hardware also supports the small-business digital infrastructure targeted by SDG 9.

---

## 2. The three contenders

| Structure | Idea in one line | What I expected |
|---|---|---|
| **Plain BST** | Smaller keys go left, larger go right, and nothing is ever rebalanced | Fine with random keys, but it should collapse into a chain on sorted input |
| **AVL tree** | A BST that rotates nodes whenever the two subtree heights differ by more than 1 | Height stays close to log₂ n whatever the input order |
| **Hash table (separate chaining)** | `bucket = Knuth multiplicative hash(SKU)` over 61 buckets, with colliding items kept in a linked list | About one check per lookup, but no ordering of keys |

**Why chaining with a multiplicative hash?** With `SKU mod m` and open addressing, a run of consecutive SKUs fills neighbouring slots and forms a long cluster. Multiplying the key by 2654435761 (Knuth's constant) scatters neighbouring SKUs across the table. Chaining also means a collision only lengthens one short list and never spills into other buckets.

---

## 3. How cost is measured

Wall-clock time for 48 items is a few nanoseconds and depends heavily on the machine, so I count **key checks** instead. The counts are deterministic, so anyone who runs the program gets exactly the numbers below.

* **BST / AVL:** one key check for every node whose SKU is compared.
* **Hash table:** one key check for every chain entry inspected. Reading an empty bucket counts as 1.
* **Build cost:** the total key checks spent inserting all 48 items. For the AVL tree I also log the rotations by type (LL, RR, LR, RL).
* **Hash health:** load factor, collisions (inserts into a non-empty bucket), longest chain and empty buckets.

---

## 4. Project layout

```text
ProductID-Search-Arena/
├── include/
│   ├── product.hpp        # Item record and OpCost counter
│   ├── plain_bst.hpp      # unbalanced BST (iterative, no recursion depth issues)
│   ├── avl_index.hpp      # AVL tree with rotation log and range query
│   ├── chained_hash.hpp   # separate-chaining hash table, Knuth hashing
│   └── catalog.hpp        # dataset and query declarations
├── src/
│   ├── catalog.cpp        # 48-item FreshKart catalogue, 3 arrival orders
│   └── main.cpp           # experiment driver and interactive menu
├── results/
│   ├── build_stats.csv    # generated: one row per scenario
│   ├── search_costs.csv   # generated: one row per query per scenario
│   └── sample_output.txt  # console output of a full run
├── Makefile
└── README.md
```

---

## 5. Building and running

```bash
make            # builds ./arena  (needs g++ or clang++ with C++17)
make run        # runs the experiment, saves results/sample_output.txt and the CSVs
make menu       # interactive mode for the viva demo
```

Without `make` (for example in Windows PowerShell with MinGW):

```bash
g++ -std=c++17 -Wall -O2 -Iinclude -o arena src/main.cpp src/catalog.cpp
./arena            # experiment
./arena --menu     # interactive mode
```

**Menu options:** `1` search a SKU (shows key checks for all three structures and the hash bucket), `2` add an item (duplicates are refused), `3` list the catalogue in SKU order, `4` range query on the AVL tree, `5` live statistics, `0` quit.

---

## 6. Experiment design

The **same 48 items** are loaded in three different orders. Each order is a separate scenario with freshly built structures:

| Scenario | Insertion order | Purpose |
|---|---|---|
| **A** | Delivery order: the 16 consecutive dairy SKUs first, then 32 scattered SKUs | Realistic mixed arrival |
| **B** | Sorted ascending | Worst case for a BST |
| **C** | Shuffled with a fixed seed (Fisher–Yates, `mt19937`, seed 2026) | Typical random case |

In every scenario I searched **6 SKUs that exist** (`20001, 20009, 20016, 11427, 52690, 98450`) and **6 that do not** (`20017, 19999, 10000, 50000, 99999, 33333`). The absent keys include values just outside the dairy run and values beyond both ends of the key range, which are where unbalanced trees suffer most.

---

## 7. Results

The figures below are copied from `results/sample_output.txt`. The ideal height for 48 keys is ⌈log₂ 49⌉ = 6.

### 7.1 Build statistics

| Scenario | BST height | BST insert checks | AVL height | AVL insert checks | AVL rotations (LL/RR/LR/RL) | Hash collisions | Longest chain |
|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| A – delivery | 22 | 667 | 7 | 213 | 22 (1/17/2/2) | 17 | 4 |
| B – sorted | **48** | **1128** | 6 | 225 | 42 (0/42/0/0) | 17 | 4 |
| C – shuffled | 10 | 250 | 7 | 210 | 21 (4/8/5/4) | 17 | 4 |

The hash table uses 61 buckets for 48 items (load factor 0.79). Exactly 30 buckets are empty and the longest chain holds 4 items. These figures are the same in every scenario because a key's bucket does not depend on the order in which keys arrive.

### 7.2 Average key checks per search

| Scenario | BST hit | AVL hit | Hash hit | BST miss | AVL miss | Hash miss |
|---|:-:|:-:|:-:|:-:|:-:|:-:|
| A – delivery | 11.83 | 4.50 | **1.17** | 14.83 | 5.67 | **1.50** |
| B – sorted | 19.83 | 4.83 | **1.33** | 22.17 | 6.00 | **1.50** |
| C – shuffled | 5.83 | 4.50 | **1.33** | 5.33 | 5.17 | **1.50** |

### 7.3 Worst single search seen

| Scenario | BST | AVL | Hash |
|---|:-:|:-:|:-:|
| A | 21 | 7 | 3 |
| B | **48** (SKU 98450 and absent 99999) | 6 | 3 |
| C | 10 | 6 | 3 |

### 7.4 Range query (scenario A)
I asked for "every SKU from 20005 to 20012". The AVL tree returned the 8 matching items **in sorted order after visiting only 11 nodes**. The hash table has no notion of order, so it would have to scan all 48 stored entries and then sort the matches.

---

## 8. What the numbers tell me

1. **The order of arrival decides a plain BST's fate.** The same 48 keys gave a BST of height 10 when shuffled, 22 for the delivery order and 48 when sorted. In the sorted case the tree is just a linked list: finding SKU `98450` took 48 checks, the same as a linear scan.
2. **AVL rotations are cheap insurance.** The AVL tree stayed at height 6 or 7 in all three scenarios, so no search ever needed more than 7 checks. In the sorted import it performed 42 rotations, all of the RR type, because every new key landed in the right subtree of the right child. That one-time cost cut build effort from 1128 to 225 key checks.
3. **A height of 7 is still "balanced".** In scenarios A and C the AVL height was 7 rather than 6. This is allowed: the AVL rule only promises a height below about 1.44 log₂ n (≈ 8 for 48 keys), not a perfect tree.
4. **Hashing hardly cares about key patterns.** The consecutive dairy SKUs did not cluster, because the multiplicative hash spread them across the table. Successful searches averaged 1.17–1.33 checks and unsuccessful ones 1.50; no search took more than 3.
5. **Misses are where trees pay most.** A BST or AVL miss must travel all the way to a null link, so misses always cost at least as much as a typical hit. A hash miss only scans one short chain. This matters at a billing desk, where mistyped or unregistered codes are common.
6. **Order is the hash table's blind spot.** Listing the catalogue by SKU or answering "all items in this SKU range" is natural for the AVL tree and awkward for the hash table.

---

## 9. Verdict

**Winner for point lookups: the hash table with separate chaining.**
For the billing-desk task, where each scan is an exact "find this SKU" query, it needed about 1.2–1.5 key checks on average. That is 3–4× fewer than the AVL tree and up to 15× fewer than the plain BST. Its cost also stayed the same across all three arrival orders.

**Runner-up, and the right choice when order matters: the AVL tree.**
If the shop also needs sorted stock reports or range queries (for example "show every item in the dairy SKU block"), the AVL tree's guaranteed height of at most 7 and its in-order traversal make it the better all-round index.

**Not recommended: the unbalanced BST.**
It works well only when keys arrive in random order, which supplier batches and sorted imports never guarantee. In the sorted scenario its worst search was 48 checks, 8 times worse than the AVL tree's worst search (6).

A practical system could keep **both** indexes: the hash table for scanning at the counter and the AVL tree for reports. The memory cost for a catalogue of a few thousand items is small.

---

## 10. Limitations and possible extensions

* The catalogue has only 48 items. A larger, randomly generated catalogue (10⁴–10⁶ SKUs) with real timing measurements would show cache and memory effects that key counts cannot capture.
* The hash table has a fixed size. Resizing when the load factor goes above about 0.75 would keep chains short as the catalogue grows.
* Deletion (removing discontinued items) is not implemented. AVL deletion needs rebalancing on the way back up; chained deletion is a simple list removal.
* A red-black tree or a B-tree could be added as further contenders.

---

## 11. References

1. T. H. Cormen, C. E. Leiserson, R. L. Rivest and C. Stein, *Introduction to Algorithms*, 4th ed., MIT Press, 2022, chapters on binary search trees and hash tables.
2. G. M. Adelson-Velsky and E. M. Landis, "An algorithm for the organization of information," *Soviet Mathematics Doklady*, vol. 3, 1962.
3. D. E. Knuth, *The Art of Computer Programming, Vol. 3: Sorting and Searching*, 2nd ed., Addison-Wesley, 1998, section 6.4 (multiplicative hashing).
4. United Nations, "Goal 12: Ensure sustainable consumption and production patterns," Sustainable Development Goals.
