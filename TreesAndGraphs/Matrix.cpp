#include "Matrix.h"
#include "LeetCodeUtil.h"

#include <vector>
#include <queue>
#include <unordered_set>
#include <set>

namespace Matrix
{
    using namespace std;

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
    // 947. Most Stones Removed with Same Row or Column
    //---------------------------------------------------------------------------------------
    class Solution947
    {
    public:
        // This is actually a undirected graph question. If stones are in the same row/column
        // , they are in the same group. We can remove stones until there is only one left
        // in the group. So, n - count of group is the answer of this question.
        int removeStones(vector<vector<int>>& stones)
        {
            const int n = stones.size();

            vector<int> group(n);
            // Initial the group. Let every stone be in its own group.
            for (int i = 0; i < n; ++i)
            {
                group[i] = i;
            }

            // Double loops to compare any two coordinates.
            for (int i = 0; i < n; ++i)
            {
                for (int j = i + 1; j < n; ++j)
                {
                    // x-coordinate is equal or y-coordinate is equal.
                    if (stones[i][0] == stones[j][0] || stones[i][1] == stones[j][1])
                    {
                        // Use the index in stones as the stone's Id.
                        int root1 = getRoot(group, i);
                        int root2 = getRoot(group, j);
                        if (root1 != root2)
                        {
                            group[root2] = root1;
                        }
                    }
                }
            }

            int groupCount = 0;
            for (int i = 0; i < n; ++i)
            {
                // Locate the root. The root count is equal to the group count.
                if (group[i] == i)
                {
                    groupCount++;
                }
            }

            return n - groupCount;
        }

        int getRoot(vector<int>& group, int stone)
        {
            while (group[stone] != stone)
            {
                stone = group[stone];
            }

            return stone;
        }
    };

    //---------------------------------------------------------------------------------------
    // 489. Robot Room Cleaner (Hard)
    //---------------------------------------------------------------------------------------
    class Solution489
    {
    public:
        class Robot
        {
        public:
            bool move() {}
            void turnLeft() {}
            void turnRight() {}
            void clean() {}
        };

        using CoordType = pair<int, int>;
        // Record the coordinates we visited. Not that unordered_set doesn't support hash
        // function for pair<int, int>!
        struct coordinate_hash
        {
            inline size_t operator()(const std::pair<int, int>& v) const
            {
                return v.first * 31 + v.second;
            }
        };
        unordered_set<CoordType, coordinate_hash> m_visited;
        const vector<CoordType> m_dirs{ {-1, 0}, {0, 1}, {1, 0}, {0, -1} };

        // We don't need a fancy algorithm to deal with the path. We just need to do DFS
        // traverse at 4 directions and go back the previous state(cell and direction)
        // when there is no cell can go in this cell.
        void cleanRoom(Robot& robot)
        {
            m_visited.clear();

            dfsTraverse(robot, 0, 0, 0);
        }

        void dfsTraverse
            (
            Robot& robot,
            int x,
            int y,
            int dir //!< 0: up, 1,: right, 2: down, 3: left.
            )
        {
        // Clean the current cell.
        robot.clean();
        m_visited.insert({ x, y });
        // Do DFS on all adjacent cells.
        for (int i = 0; i < m_dirs.size(); ++i)
        {
            const int newDir = ( dir + i ) % 4;
            const int newX = x + m_dirs[newDir].first;
            const int newY = y + m_dirs[newDir].second;
            const CoordType newCoord{ newX,newY };
            // i starts with 0, so robot in initially at 'dir' direction.
            if (m_visited.count(newCoord) == 0 && robot.move())
            {
                dfsTraverse(robot, newX, newY, newDir);
                // When DFS return, it visited all possible cells. We need to
                // turn around.
                robot.turnRight();
                robot.turnRight();
                robot.move();
                robot.turnLeft();
                robot.turnLeft();
            }

            // After finished one direction, we turn and go to the next direction.
            robot.turnRight();
        }
        }
    };

    //---------------------------------------------------------------------------------------
    // 1293. Shortest Path in a Grid with Obstacles Elimination (Hard)
    //---------------------------------------------------------------------------------------
    class Solution1293
    {
    public:
        // Idea:
        // 1. The question asks about whether we can reach, so we should use BFS, not DFS.
        //    BFS guarantees the steps is minimum when we reach the destination.
        // 2. When we meet obstacles, we just destroy it with the limited power(k). However,
        //    we cannot consider it as visited. That cell might be reached again through
        //    different paths with different remaining power points.
        //    When we reach a cell, we should record how many power points we still have.
        //    There is important optimization - when we reach a cell with less power points,
        //    we should withdraw because we know the current path is worse than the previous
        //    one, which uses less power points.
        // A cell is not just visited or not, the remaining power points defines its state.
        struct CellState
        {
            int x;
            int y;
            int power;

            CellState(int aX, int aY, int aPower) : x(aX), y(aY), power(aPower) {}
        };

        const vector<pair<int, int>> m_dirs{ {-1,0 }, {0, 1}, {1, 0}, {0, -1} };

        int shortestPath(vector<vector<int>>& grid, int k)
        {
            const size_t m = grid.size();
            const size_t n = grid[0].size();

            if (m == 1 && n == 1)
            {
                return 0;
            }

            // -1: We have not reach this cell.
            // 0~k: The last time we reach this cell we have 0~k power points.
            vector<vector<int>> table(m, vector<int>(n, -1));
            table[0][0] = k;

            // To do BFS, we need a queue.
            queue<CellState> remainingStates;
            remainingStates.push(CellState(0, 0, k));

            int result = 0;
            while (!remainingStates.empty())
            {
                // Think! It's easy to determine this is the place we should increment it.
                result++;
                // Similar to level-order traversal, we need a for-loop here to fetch all.
                const int count = remainingStates.size();
                for (int i = 0; i < count; ++i)
                {
                    const int x = remainingStates.front().x;
                    const int y = remainingStates.front().y;
                    const int power = remainingStates.front().power;
                    remainingStates.pop();

                    for (int j = 0; j < m_dirs.size(); ++j)
                    {
                        int newX = x + m_dirs[j].first;
                        int newY = y + m_dirs[j].second;

                        if (newX < 0 || newX >= m || newY < 0 || newY >= n)
                        {
                            // Exceeds the boundaries.
                            continue;
                        }
                        else if (newX == m - 1 && newY == n - 1)
                        {
                            // Reach the destination.
                            return result;
                        }
                        int newPower = power - grid[newX][newY]; // 1: obstacle, 0: empty.
                        if (newPower < 0 || // blocked.
                            newPower <= table[newX][newY]) // This is a worse or equal path. No need to continue.
                        {
                            continue;
                        }

                        table[newX][newY] = newPower;
                        remainingStates.push(CellState(newX, newY, newPower));
                    }
                }
            }

            return -1;
        }
    };

    //---------------------------------------------------------------------------------------
    // 2128. Remove All Ones With Row and Column Flips
    // Topic: Math, Bit manipulation
    //---------------------------------------------------------------------------------------
    class Solution2128
    {
    public:
        // This question requires very keen perception.
        // The answer is yes if and only if the row 2 and row 3 are either equal to row 1 or
        // to row 1's reversal.
        bool removeOnes(vector<vector<int>>& grid)
        {
            vector<int> reversedRow1;
            getReversed(grid[0], reversedRow1);

            return std::all_of(grid.begin(), grid.end(), [&](const vector<int>& row)
                {
                    return row == grid[0] || row == reversedRow1;
                }
            );
        }

        void getReversed(vector<int>& from, vector<int>& result)
        {
            for (const auto& i : from)
            {
                result.push_back(i == 0 ? 1 : 0);
            }
        }
    };

    //---------------------------------------------------------------------------------------
    // Test function
    //---------------------------------------------------------------------------------------
    void TestMatrix()
    {
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

        // 947. Most Stones Removed with Same Row or Column
        // Input: stones = [[0, 0], [0, 1], [1, 0], [1, 2], [2, 1], [2, 2]]
        // Output : 5
        LeetCodeUtil::BuildIntMatrixFromString("[[0, 0], [0, 1], [1, 0], [1, 2], [2, 1], [2, 2]]", &inputMatrix);
        Solution947 sol947;
        cout << "\n947. Most Stones Removed with Same Row or Column: " << sol947.removeStones(inputMatrix) << endl;

        // 1293. Shortest Path in a Grid with Obstacles Elimination (Hard)
        // Input: grid = [[0,0,0],[1,1,0],[0,0,0],[0,1,1],[0,0,0]], k = 1
        // Output: 6
        // Input: grid = [[0]], k = 1
        // Output: 1
        LeetCodeUtil::BuildIntMatrixFromString("[[0]]", &inputMatrix);
        int k = 1;
        Solution1293 sol1293;
        cout << "\n1293. Shortest Path in a Grid with Obstacles Elimination: " << sol1293.shortestPath(inputMatrix, k) << endl;

        // 2128. Remove All Ones With Row and Column Flips
        // Input: grid = [[0, 1, 0], [1, 0, 1], [0, 1, 0]]
        // Output : true
        LeetCodeUtil::BuildIntMatrixFromString("[[1, 1, 0], [1, 1, 0], [0, 0, 1]]", &inputMatrix);
        Solution2128 sol2128;
        cout << "\n2128. Remove All Ones With Row and Column Flips: " << sol2128.removeOnes(inputMatrix) << endl;
    }
}