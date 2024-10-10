#include <bits/stdc++.h>

/**
 * @class UnionFind
 * @brief A class to represent the Union-Find data structure (also known as Disjoint Set Union).
 *
 * This class provides efficient methods to perform union and find operations on disjoint sets.
 * It uses path compression and union by rank to optimize the operations.
 */
class UnionFind
{
    int *parent;
    int *rank;

public:
    /**
     * @brief Constructs a UnionFind data structure with a specified number of elements.
     * 
     * @param n The number of elements in the UnionFind data structure.
     */
    UnionFind(int n);

    /**
     * @brief Destructor for the UnionFind class.
     * 
     * This destructor is responsible for cleaning up any resources
     * allocated by the UnionFind instance. It ensures that all memory
     * and other resources are properly released when the UnionFind
     * object is destroyed.
     */
    ~UnionFind();

    /**
     * @brief Finds the representative (or root) of the set containing the element i.
     * 
     * This function implements the "find" operation of the Union-Find data structure,
     * which is used to determine the set to which a particular element belongs. It 
     * typically uses path compression to flatten the structure, ensuring that future 
     * queries are faster.
     * 
     * @param i The element for which to find the representative.
     * @return int The representative (or root) of the set containing the element i.
     */
    int find(int i);
    
    /**
     * @brief Unites two elements into the same set.
     * 
     * This function merges the sets that contain the elements x and y. 
     * If x and y are already in the same set, the function does nothing.
     * 
     * @param x The first element to unite.
     * @param y The second element to unite.
     */
    void unite(int x, int y);
};