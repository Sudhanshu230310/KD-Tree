# KD-Tree K-Nearest Neighbors in C++

This project implements a C++ program to construct a k-dimensional (KD) tree and perform both 1-nearest neighbor (1-NN) and k-nearest neighbor (k-NN) searches efficiently.

---

## Features

* **KD-Tree Construction**: Recursively builds a balanced KD-Tree on arbitrary-dimensional points.
* **1-NN Search**: Finds the single closest point to a query using backtracking and pruning.
* **k-NN Search**: Retrieves the k closest points via a simple bounded priority queue (vector) and subtree pruning.
* **Euclidean Distance**: Uses squared distance comparisons to prune branches without computing costly square roots during search.
* **Input/Output**:

  * Reads dimension `d`, number of neighbors `k`, and number of data points from `input.txt`.
  * Query point and dataset read from file; results printed to console.
* **Simple Priority Queue**: Maintains top-k candidates in a vector, replacing the farthest when a closer point is found.

---

## Getting Started

### Prerequisites

* C++ compiler with C++11 support (e.g., `g++`, `clang++`).

### Building

```bash
# Compile the KDTree program
g++ -std=c++11 KDTree.cpp -o kdtree
```

### Input File Format (`input.txt`)

```
<d>                  # Dimension of the data points
<k>                  # Number of nearest neighbors to find
<n>                  # Number of data points in the dataset
x1 x2 ... xd         # Coordinates of the query point (d values)
# Next n lines: dataset points
p1_1 p1_2 ... p1_d
p2_1 p2_2 ... p2_d
...
pn_1 pn_2 ... pn_d
```

*Example:* For d=2, k=3, n=5:

```
2
3
5
4 5
1 2
7 8
3 5
5 4
9 0
```

---

## Running

```bash
# Ensure input.txt is present in the working directory
./kdtree
```

* **1-NN Output**:

  ```
  Result of Application of 1-Nearest Neighbor:
  1 Nearest Point is {x-y-…} at Distance: <distance>
  ```

* **k-NN Output**:

  ```
  Results of Application of K-Nearest Neighbors (K = k):
  1 Nearest Point is {…} at Distance: …
  2 Nearest Point is {…} at Distance: …
  …
  ```

---

## Code Structure

* **`struct KDNode`**: Node containing a d-dimensional point and left/right child pointers.
* **`insertKD`**: Inserts points into the KD-Tree by splitting on successive dimensions.
* **`nearestNeighbor`**: Recursively searches for the closest point, pruning subtrees outside current best radius.
* **`kNearestNeighbor`**: Tracks top-k closest with a fixed-size vector, updating when a closer point is encountered.
* **`distanceSquared`**: Computes squared Euclidean distance, used for comparisons.
* **`max_index`**, **`sortKNNResults`**: Helpers for maintaining and ordering the candidate list.

---

## Configuration & Tuning

* **Dimension (`d`) and `k`** set in `input.txt`.
* **Priority Queue**: Currently a linear-scan vector; consider replacing with a binary heap for large k.
* **Pruning Threshold**: Uses squared distances—no configuration needed.

---

## Limitations & Future Improvements

* **Balancing**: Insertion order may unbalance the tree; consider median-of-nodes splitting.
* **Memory Management**: No explicit deletion of allocated nodes—add tree cleanup to avoid leaks.
* **Priority Queue**: Swap in a max-heap or multiset for better performance on large k.
* **Parallelism**: Explore multi-threaded search for very large datasets.
* **File I/O**: Add command-line options to specify input/output filenames.

---

## License

MIT License. Feel free to use, modify, and distribute.
