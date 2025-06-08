#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <limits>
#include <algorithm>

using namespace std;

// D IS DIMENSION OF K-D TREE
// K IS NUMBER OF NEAREST NEIGHBORS WE WANT TO FIND

struct NodeQueue {
    vector<long long> arr;  // Stores the point coordinates
    long long priority;     // Stores the squared distance to the target
};

// Global container to act as our "priority queue" for KNN
vector<NodeQueue> knnQueue;

// Returns the index (0-based) of the element in the first k entries of knnQueue
// that has the maximum priority (i.e., largest squared distance).
long long max_index(long long k) {
    long long largest = 0;
    for (long long i = 1; i < k; ++i) {
        if (knnQueue[i].priority >= knnQueue[largest].priority) {
            largest = i;
        }
    }
    return largest;
}

// A node in the K-D Tree
struct KDNode {
    vector<long long> point;
    KDNode* left;
    KDNode* right;

    KDNode(const vector<long long>& arr)
        : point(arr), left(nullptr), right(nullptr) {}
};

// Inserts a new point into the K-D Tree, comparing on dimension = depth % d
KDNode* insertKD(KDNode* root, const vector<long long>& arr, long long depth, long long d) {
    if (root == nullptr) {
        return new KDNode(arr);
    }
    long long cd = depth % d;
    if (arr[cd] < root->point[cd]) {
        root->left = insertKD(root->left, arr, depth + 1, d);
    } else {
        root->right = insertKD(root->right, arr, depth + 1, d);
    }
    return root;
}

// Computes squared Euclidean distance between point1 and point2 in d dimensions
long long distanceSquared(const vector<long long>& point1, const vector<long long>& point2, long long d) {
    long long dist = 0;
    for (long long i = 0; i < d; ++i) {
        long long diff = point1[i] - point2[i];
        dist += diff * diff;
    }
    return dist;
}

// Returns the closer node (among nodeA and nodeB) to the target point, or nullptr if both null
KDNode* closerNode(KDNode* nodeA, KDNode* nodeB, const vector<long long>& target, long long d) {
    if (nodeA == nullptr) return nodeB;
    if (nodeB == nullptr) return nodeA;

    long long distA = distanceSquared(nodeA->point, target, d);
    long long distB = distanceSquared(nodeB->point, target, d);
    return (distA < distB ? nodeA : nodeB);
}

// Recursively find the nearest neighbor to 'target' in the tree
KDNode* nearestNeighbor(KDNode* root, const vector<long long>& target, long long depth, long long d) {
    if (root == nullptr) return nullptr;

    long long cd = depth % d;
    KDNode* nextBranch = nullptr;
    KDNode* otherBranch = nullptr;

    if (target[cd] < root->point[cd]) {
        nextBranch = root->left;
        otherBranch = root->right;
    } else {
        nextBranch = root->right;
        otherBranch = root->left;
    }

    KDNode* best = nearestNeighbor(nextBranch, target, depth + 1, d);
    best = closerNode(best, root, target, d);

    long long radiusSq = distanceSquared(best->point, target, d);
    long long diff = target[cd] - root->point[cd];
    if (radiusSq >= diff * diff) {
        KDNode* possible = nearestNeighbor(otherBranch, target, depth + 1, d);
        best = closerNode(best, possible, target, d);
    }
    return best;
}

// Recursively find the K nearest neighbors to 'target' in the tree
void kNearestNeighbor(KDNode* root,
                      const vector<long long>& target,
                      long long depth,
                      long long d,
                      long long k)
{
    if (root == nullptr) return;

    long long cd = depth % d;
    KDNode* nextBranch = nullptr;
    KDNode* otherBranch = nullptr;

    if (target[cd] < root->point[cd]) {
        nextBranch = root->left;
        otherBranch = root->right;
    } else {
        nextBranch = root->right;
        otherBranch = root->left;
    }

    // First, recurse down the "best" branch
    kNearestNeighbor(nextBranch, target, depth + 1, d, k);

    // Compute distance of root->point to target
    long long distSq = distanceSquared(root->point, target, d);

    if ((long long)knnQueue.size() < k) {
        // If we have fewer than k stored, just push this one
        NodeQueue entry;
        entry.arr = root->point;
        entry.priority = distSq;
        knnQueue.push_back(std::move(entry));
    } else {
        // Otherwise, see if this is closer than the current farthest
        long long idxMax = max_index(k);
        if (distSq < knnQueue[idxMax].priority) {
            knnQueue[idxMax].arr = root->point;
            knnQueue[idxMax].priority = distSq;
        }
    }

    // Now decide whether to explore the other branch
    if ((long long)knnQueue.size() < k) {
        kNearestNeighbor(otherBranch, target, depth + 1, d, k);
    } else {
        long long idxMax = max_index(k);
        long long radiusSq = knnQueue[idxMax].priority;
        long long diff = target[cd] - root->point[cd];
        if (radiusSq >= diff * diff) {
            kNearestNeighbor(otherBranch, target, depth + 1, d, k);
        }
    }
}

// After collecting k neighbors (unordered), sort them by increasing distance
void sortKNNResults(long long k) {
    if (k == 0) return;
    // Simple selection-sort–style bring the farthest to the end repeatedly
    for (long long i = k; i > 1; --i) {
        long long idxMax = max_index(i);
        if (idxMax != i - 1) {
            swap(knnQueue[idxMax], knnQueue[i - 1]);
        }
    }
}

int main() {
    // Read inputs from "input.txt"
    ifstream infile("input.txt");
    if (!infile.is_open()) {
        cerr << "Error: Could not open input.txt\n";
        return 1;
    }

    long long d, k, number_points;
    infile >> d;
    infile >> k;
    infile >> number_points;

    // Read the query point (point1) of dimension d
    vector<long long> point1(d);
    for (long long i = 0; i < d; ++i) {
        infile >> point1[i];
    }

    // Read all data points into a vector of vectors
    vector<vector<long long>> allPoints(number_points, vector<long long>(d));
    for (long long i = 0; i < number_points; ++i) {
        for (long long j = 0; j < d; ++j) {
            infile >> allPoints[i][j];
        }
    }

    infile.close();

    // Build the K-D Tree
    KDNode* root = nullptr;
    for (long long i = 0; i < number_points; ++i) {
        root = insertKD(root, allPoints[i], 0, d);
    }

    // === 1-NN Search ===
    cout << "Result of Application of 1-Nearest Neighbor:\n";
    KDNode* nearest = nearestNeighbor(root, point1, 0, d);
    if (nearest) {
        cout << "1 Nearest Point is {";
        for (long long i = 0; i < d; ++i) {
            cout << nearest->point[i] << (i + 1 < d ? "-" : "");
        }
        cout << "}";
        long long distSq1 = distanceSquared(nearest->point, point1, d);
        double dist1 = sqrt((long double)distSq1);
        cout << " at Distance: " << dist1 << "\n\n";
    } else {
        cout << "No nearest neighbor found.\n\n";
    }

    // === K-NN Search ===
    cout << "Results of Application of K-Nearest Neighbors (K = " << k << "):\n";
    knnQueue.clear();
    knnQueue.reserve(k);
    kNearestNeighbor(root, point1, 0, d, k);

    // Sort the K results by increasing distance
    sortKNNResults(k);

    for (long long i = 0; i < (long long)knnQueue.size(); ++i) {
        cout << (i + 1) << " Nearest Point is {";
        for (long long j = 0; j < d; ++j) {
            cout << knnQueue[i].arr[j] << (j + 1 < d ? "-" : "");
        }
        cout << "}";
        double distK = sqrt((long double)knnQueue[i].priority);
        cout << " at Distance: " << distK << "\n";
    }

    return 0;
}
