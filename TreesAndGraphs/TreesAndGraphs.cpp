// TreesAndGraphs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <algorithm> // std::reverse
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>

#include "LeetCodeUtil.h"

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
// 994. Rotting Oranges (Medium)
// You are given an m x n grid where each cell can have one of three values:
// 0 representing an empty cell,
// 1 representing a fresh orange, or
// 2 representing a rotten orange.
//
// Topic: Breadth first search
//
// Real onsite interview question.
//---------------------------------------------------------------------------------------

int orangesRotting(vector<vector<int>>& grid)
{
    // This is not a simple BFS practice because there might be multiple rotten oranges.
    // All rotten oranges infect adjacent orange at the same time.
    // Therefore, we must do BFS for all rotten oranges at the same time.
    const int m = grid.size();
    const int n = grid[0].size();

    int result = 0;
    int remainingGoodOrange = 0;
    // Store the orange that is rotten but not yet infect others.
    queue<pair<int, int>> orangeQueue;

    // Gather fresh and rotten oranges.
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; ++j)
        {
            if (grid[i][j] == 2)
            {
                orangeQueue.push({ i, j });
            }
            else if (grid[i][j] == 1)
            {
                remainingGoodOrange++;
            }
        }
    }

    while (!orangeQueue.empty() && remainingGoodOrange > 0)
    {
        const int orangeCount = orangeQueue.size();
        // Use a for-loop to get all rotten oranges start to infect others.
        for (int k = 0; k < orangeCount; ++k)
        {
            auto coord = orangeQueue.front();
            orangeQueue.pop();

            // Get adjacent orange rotten.
            if (coord.first > 0 && grid[coord.first - 1][coord.second] == 1)
            {
                grid[coord.first - 1][coord.second] = 2;
                orangeQueue.push({ coord.first - 1, coord.second });
                remainingGoodOrange--;
            }
            if (coord.second > 0 && grid[coord.first][coord.second - 1] == 1)
            {
                grid[coord.first][coord.second - 1] = 2;
                orangeQueue.push({ coord.first, coord.second - 1 });
                remainingGoodOrange--;
            }
            if (coord.first < m - 1 && grid[coord.first + 1][coord.second] == 1)
            {
                grid[coord.first + 1][coord.second] = 2;
                orangeQueue.push({ coord.first + 1, coord.second });
                remainingGoodOrange--;
            }
            if (coord.second < n - 1 && grid[coord.first][coord.second + 1] == 1)
            {
                grid[coord.first][coord.second + 1] = 2;
                orangeQueue.push({ coord.first, coord.second + 1 });
                remainingGoodOrange--;
            }
        }

        result++;
    }

    return remainingGoodOrange > 0 ? -1 : result;
}


//---------------------------------------------------------------------------------------
// 332. Reconstruct Itinerary (Hard)
//
// Real onsite interview question.
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// 417. Pacific Atlantic Water Flow
//
// Blind Curated 75
//---------------------------------------------------------------------------------------
class Solution417
{
public:

    int m = 0;
    int n = 0;

    void dfs
        (
        vector<vector<bool>>& terrains,
        vector<vector<int>>& heights,
        int x,
        int y,
        int prevHeight
        )
    {
        // We traverse backward from low to high.
        if (heights[x][y] < prevHeight || terrains[x][y])
        {
            return;
        }
        // Water can flow to the ocean from this terrain.
        terrains[x][y] = true;

        const int height = heights[x][y];
        if (x > 0)
        {
            dfs(terrains, heights, x - 1, y, height);
        }
        if (x < m - 1)
        {
            dfs(terrains, heights, x + 1, y, height);
        }
        if (y > 0)
        {
            dfs(terrains, heights, x, y - 1, height);
        }
        if (y < n - 1)
        {
            dfs(terrains, heights, x, y + 1, height);
        }
    }

    vector<vector<int>> pacificAtlantic(vector<vector<int>>& heights)
    {
        // Idea: Do DFS from the edge and mark a terrain whether it can direct water
        // to Pacific/Atlantic. If both are true, that terrain is in our answer.
        m = heights.size();
        n = heights[0].size();

        vector<vector<int>> result;
        // To record if a location where water can flow to Pacific/Atlantic from it.
        // pacificTerrains[i][j] means that water can flow to Pacific from location [i, j].
        vector<vector<bool>> pacificTerrains(m, vector<bool>(n, false));
        vector<vector<bool>> atlanticTerrains(m, vector<bool>(n, false));

        for (int i = 0; i < m; ++i)
        {
            // Left shore.
            dfs(pacificTerrains, heights, i, 0, INT_MIN);
            // Right shore.
            dfs(atlanticTerrains, heights, i, n - 1, INT_MIN);
        }

        for (int j = 0; j < n; ++j)
        {
            // Top shore.
            dfs(pacificTerrains, heights, 0, j, INT_MIN);
            // Bottom shore.
            dfs(atlanticTerrains, heights, m - 1, j, INT_MIN);
        }

        for (int i = 0; i < m; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (pacificTerrains[i][j] && atlanticTerrains[i][j])
                {
                    result.push_back({ i, j });
                }
            }
        }

        return result;
    }
};

//---------------------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------------------
int main()
{
    std::cout << "Trees and Graphs!\n";

    // 994. Rotting Oranges (Medium)
    // Input: grid = [[2,1,1],[1,1,0],[0,1,1]]
    // Output: 4
    // Input: grid = [[2,1,1],[0,1,1],[1,0,1]]
    // Output: -1
    // Need to test: [[0]], [[1]], [[2]], [[0,2,2]], [[2,1,0],[0,0,0],[1,1,2]]
    // Input: [[2,1,1],[1,1,1],[0,1,2]]
    // Output: 2
    vector<vector<int>> inputMatrix;
    LeetCodeUtil::BuildIntMatrixFromString("[[2,1,1],[0,1,1],[1,0,1]]", &inputMatrix);
    cout << "\nResult of Rotting Oranges: " << orangesRotting(inputMatrix) << endl;

    // 222. Count Complete Tree Nodes (Medium)
    // Input: root = [1,2,3,4,5,6]
    // Output: 6
    TreeNode* root = LeetCodeUtil::BuildTreeFromLevelOrderString("[1,2,3,4,5,6]");
    cout << "\nResult of Count Complete Tree Nodes (Medium): " << countNodes(root) << endl;
    LeetCodeUtil::DeleteTree(root);

    // 417. Pacific Atlantic Water Flow
    // Input: heights = [[1,2,2,3,5],[3,2,3,4,4],[2,4,5,3,1],[6,7,1,4,5],[5,1,1,2,4]]
    // Output: [[0, 4], [1, 3], [1, 4], [2, 2], [3, 0], [3, 1], [4, 0]]
    // Input: heights = [[1,1],[1,1],[1,1]]
    // Output:
    LeetCodeUtil::BuildIntMatrixFromString("[[1,1],[1,1],[1,1]]", &inputMatrix);
    Solution417 sol417;
    auto resultVV = sol417.pacificAtlantic(inputMatrix);
    cout << "\nPacific Atlantic Water Flow: ";
    LeetCodeUtil::PrintMatrix(resultVV);
}

