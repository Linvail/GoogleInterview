// TreesAndGraphs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "LeetCodeUtil.h"
#include "Graph.h"
#include "Matrix.h"

#include <algorithm> // std::reverse
#include <iostream>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>
#include <deque> // To build string in reverse order.

using namespace LeetCodeUtil;

//---------------------------------------------------------------------------------------
// 222. Count Complete Tree Nodes (Medium)
// Design an algorithm that runs in less than O(n) time complexity.
//
// Topic: Binary search. DFS.
//
// Phone interview
//---------------------------------------------------------------------------------------

// Height: count of edge from root to the deepest node.
// So, if root has no child, the height is 0, not 1.
int getHeight(TreeNode* root)
{
    if (!root)
    {
        return -1;
    }
    else
    {
        return 1 + getHeight(root->left);
    }
}

int countNodes(TreeNode* root)
{
    // The topic includes "binary search", it is tricky.
    // We must think about the property of "Complete Tree" carefully.
    // In a complete tree, only the lowest level is not complete. The position of the
    // break point is the key of binary search.
    int height = getHeight(root);
    if (height < 0)
    {
        return 0;
    }

    int result = 0;
    if (height - 1 == getHeight(root->right))
    {
        // The break point is not in the left tree, so the left tree is full.
        // For a full binary tree with k height, it has 2^(k + 1) - 1 nodes.
        // The height of left tree is height - 1.
        result = ( ( 1 << height ) - 1 ) + // node count of left tree
            1 +  // root
            countNodes(root->right); // node count of right tree
    }
    else
    {
        // The break point is in the left tree.
        // The height of left tree is height - 2.
        result = ( ( 1 << ( height - 1 ) ) - 1 ) + // node count of right tree
            1 +  // root
            countNodes(root->left); // node count of left tree
    }

    return result;
}

//---------------------------------------------------------------------------------------
// 2096. Step-By-Step Directions From a Binary Tree Node to Another
// Topic: Lowest Common Ancestor
//---------------------------------------------------------------------------------------
class Solution2096
{
public:

    string getDirections(TreeNode* root, int startValue, int destValue)
    {
        // There are approaches:
        // A:
        // 1. find lowest common ancestor of start and dest.
        // 2. find path's length from LCA to start
        // 3. find path from LCA to dest.
        // 4. Combine startPath length's "U" with destPath's reverse.
        // B:
        // 1. find path from root to start
        // 2. find path from root to end.
        // 3. delete the common path of the path 1 and path 2.
        // 4. Combine startPath length's "U" with destPath.

        // The implementation shows solution B.
        deque<char> pathToStart;
        findPath(root, startValue, pathToStart);
        deque<char> pathToDest;
        findPath(root, destValue, pathToDest);

        // Delete the common path.
        size_t minStrLen = min(pathToStart.size(), pathToDest.size());
        size_t i = 0;
        for (; i < minStrLen; ++i)
        {
            if (pathToStart[i] != pathToDest[i])
            {
                break;
            }
        }
        pathToStart.erase(pathToStart.begin(), pathToStart.begin() + i);
        pathToDest.erase(pathToDest.begin(), pathToDest.begin() + i);

        string result;
        for (int i = 0; i < pathToStart.size(); ++i)
        {
            result.push_back('U');
        }

        result.append(pathToDest.begin(), pathToDest.end());

        return result;
    }

    bool findPath(TreeNode* root, int target, deque<char>& resultStr)
    {
        if (!root)
        {
            return false;
        }
        if (root->val == target)
        {
            return true;
        }

        bool onLeft = findPath(root->left, target, resultStr);
        if (onLeft)
        {
            resultStr.push_front('L');
            return true;
        }
        bool onRight = findPath(root->right, target, resultStr);
        if (onRight)
        {
            resultStr.push_front('R');
            return true;
        }

        return false;
    }
};

//---------------------------------------------------------------------------------------
// 366. Find Leaves of Binary Tree
//---------------------------------------------------------------------------------------
class Solution366
{
public:
    // There are two majors ways
    // Solution A: Use the height property - height is the number of edges on the path to
    // the root. So, the height of leaf is always 0. The better thing is that the nodes
    // share the same height must exists in the same vector.
    // Therefore, as long as we know the depth of a node, we know which vector we should
    // put it in.
    //
    // Solution B: Use DFS to reach the leaves. Put them in a vector. Remove them from
    // the tree (so its parent must modify the right/left).
    // One iteration will remove one level of the nodes. If the max depth is n, we will
    // have n + 1 vectors.

    vector<vector<int>> findLeaves(TreeNode* root)
    {
        // Solution A
        vector<vector<int>> result;
        findHeight(root, result);
        return result;
    }

    int findHeight(TreeNode* root, vector<vector<int>>& result)
    {
        if (!root)
        {
            // This make leaf have 0 height.
            return -1;
        }
        int depth = 1 + max(findHeight(root->left, result), findHeight(root->right, result));

        if (result.size() < depth + 1)
        {
            result.resize(depth + 1);
        }

        result[depth].push_back(root->val);

        return depth;
    }

    vector<vector<int>> findLeaves_b(TreeNode* root)
    {
        // Solution B
        vector<vector<int>> result;

        while (root)
        {
            vector<int> levelResult;
            root = removeLeaf(root, levelResult);
            result.push_back(levelResult);
        }

        return result;
    }

    TreeNode* removeLeaf(TreeNode* root, vector<int>& levelResult)
    {
        if (!root )
        {
            return nullptr;
        }
        if (!root->left && !root->right) // A leaf
        {
            levelResult.push_back(root->val);
            // We should delete root right here but I don't know if LeetCode allows me.
            return nullptr; // Remove this from the parent.
        }

        root->left = removeLeaf(root->left, levelResult);
        root->right = removeLeaf(root->right, levelResult);

        return root;
    }
};


//---------------------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------------------
int main()
{
    std::cout << "Run problems of tree, matrix, graph!\n";

    // 222. Count Complete Tree Nodes (Medium)
    // Input: root = [1,2,3,4,5,6]
    // Output: 6
    TreeNode* root = LeetCodeUtil::BuildTreeFromLevelOrderString("[1,2,3,4,5,6]");
    cout << "\n222. Count Complete Tree Nodes: " << countNodes(root) << endl;
    LeetCodeUtil::DeleteTree(root);

    // 2096. Step-By-Step Directions From a Binary Tree Node to Another
    // Input: root = [5, 1, 2, 3, null, 6, 4], startValue = 3, destValue = 6
    // Output: "UURL"
    // Input: root = [2,1], startValue = 2, destValue = 1
    // Output: "L"
    // Input: root = [1,null,10,12,13,4,6,null,15,null,null,5,11,null,2,14,7,null,8,null,null,null,9,3], startValue = 6, destValue = 15
    // Output: "UURR"
    Solution2096 sol2096;
    root = LeetCodeUtil::BuildTreeFromLevelOrderString("[1,null,10,12,13,4,6,null,15,null,null,5,11,null,2,14,7,null,8,null,null,null,9,3]");
    int startValue = 6;
    int destValue = 15;
    cout << "\n2096. Step-By-Step Directions From a Binary Tree Node to Another: " << sol2096.getDirections(root, startValue, destValue) << endl;

    // 366. Find Leaves of Binary Tree
    // Input: root = [1,2,3,4,5]
    // Output: [[4, 5, 3], [2], [1]]
    root = LeetCodeUtil::BuildTreeFromLevelOrderString("[1,2,3,4,5]");
    Solution366 sol366;
    auto resultVVI = sol366.findLeaves(root);
    cout << "\n366. Find Leaves of Binary Tree: " << endl;
    LeetCodeUtil::PrintMatrix(resultVVI);
    LeetCodeUtil::DeleteTree(root);

    Graph::TestGraph();

    Matrix::TestMatrix();
}

