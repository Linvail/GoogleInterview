#include "Matrix.h"
#include "LeetCodeUtil.h"

#include <vector>
#include <queue>
#include <unordered_set>
#include <set>
#include <map>

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
        const size_t m = grid.size();
        const size_t n = grid[0].size();

        int result = 0;
        int remainingGoodOrange = 0;
        // Store the orange that is rotten but not yet infect others.
        queue<pair<size_t, size_t>> orangeQueue;

        // Gather fresh and rotten oranges.
        for (size_t i = 0; i < m; i++)
        {
            for (size_t j = 0; j < n; ++j)
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
            const size_t orangeCount = orangeQueue.size();
            // Use a for-loop to get all rotten oranges start to infect others.
            for (size_t k = 0; k < orangeCount; ++k)
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

    private:

        void dfs
            (
            vector<vector<bool>>& terrains,
            vector<vector<int>>& heights,
            size_t x,
            size_t y,
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

        size_t m = 0;
        size_t n = 0;
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
            const size_t n = stones.size();

            vector<size_t> group(n);
            // Initial the group. Let every stone be in its own group.
            for (size_t i = 0; i < n; ++i)
            {
                group[i] = i;
            }

            // Double loops to compare any two coordinates.
            for (size_t i = 0; i < n; ++i)
            {
                for (size_t j = i + 1; j < n; ++j)
                {
                    // x-coordinate is equal or y-coordinate is equal.
                    if (stones[i][0] == stones[j][0] || stones[i][1] == stones[j][1])
                    {
                        // Use the index in stones as the stone's Id.
                        auto root1 = getRoot(group, i);
                        auto root2 = getRoot(group, j);
                        if (root1 != root2)
                        {
                            group[root2] = root1;
                        }
                    }
                }
            }

            size_t groupCount = 0;
            for (size_t i = 0; i < n; ++i)
            {
                // Locate the root. The root count is equal to the group count.
                if (group[i] == i)
                {
                    groupCount++;
                }
            }

            return static_cast<int>( n - groupCount );
        }

        size_t getRoot(vector<size_t>& group, size_t stone)
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
                const auto count = remainingStates.size();
                for (size_t i = 0; i < count; ++i)
                {
                    const int x = remainingStates.front().x;
                    const int y = remainingStates.front().y;
                    const int power = remainingStates.front().power;
                    remainingStates.pop();

                    for (size_t j = 0; j < m_dirs.size(); ++j)
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
    // 2013. Detect Squares
    // 0 <= x, y <= 1000
    //---------------------------------------------------------------------------------------
    class DetectSquares
    {
    public:

        int countMatrix[1001][1001] = {};
        vector<pair<int, int>> points;

        DetectSquares()
        {
        }

        void add(vector<int> point)
        {
            countMatrix[point[0]][point[1]]++;
            points.emplace_back(point[0], point[1]);
        }

        int count(vector<int> point)
        {
            int x1 = point[0];
            int y1 = point[1];
            int result = 0;
            // Search the point in diagonal.
            for (auto& [x3, y3] : points)
            {
                if (x3 != x1 && abs(x3 - x1) == abs(y3 - y1))
                {
                    result += countMatrix[x3][y1] * countMatrix[x1][y3];
                }
            }

            return result;
        }
    };

    //---------------------------------------------------------------------------------------
    // 329. Longest Increasing Path in a Matrix (Hard)
    // Topic: DFS/BFS, DP, Topological sort, memorization.
    //---------------------------------------------------------------------------------------
    class Solution329
    {
    public:
        int longestIncreasingPath(vector<vector<int>>& matrix)
        {
            // Idea:
            // Use DFS. We can only move to the cell having greater number.
            // Normally, we need to mark a cell 'visited', so we won't go to it. In this case
            // , we could use dp[i][j] to note the longest path from cell(i, j).
            // If we encounter a cell having dp[i][j] > 0, we should skip it.
            // * Need to accumulate the length we traveled.
            // * Need to track the maximum of dp[i][j]. It is our answer.
            m = matrix.size();
            n = matrix[0].size();
            vector<vector<int>> dp(m, vector<int>(n, -1));
            int result = -1;

            for (size_t i = 0; i < m; ++i)
            {
                for (size_t j = 0; j < n; ++j)
                {
                    int len = findPathDfs(matrix, dp, i, j);
                    result = max(result, len);
                }
            }

            return result;
        }

    private:

        int findPathDfs(vector<vector<int>>& matrix, vector<vector<int>>& dp, size_t x, size_t y)
        {
            if (dp[x][y] != -1)
            {
                // This cell has been visited, return its value directly.
                return dp[x][y];
            }

            int len = 1; // Initially, a cell can visit itself.
            const int curr = matrix[x][y];
            // Do DFS for nearby accessible cells.
            // We don't which direction would give us the longest path, so use max().
            if (x > 0 && curr < matrix[x - 1][y])
            {
                len = max(len, 1 + findPathDfs(matrix, dp, x - 1, y));
            }
            if (y > 0 && curr < matrix[x][y - 1])
            {
                len = max(len, 1 + findPathDfs(matrix, dp, x, y - 1));
            }
            if (x < m - 1 && curr < matrix[x + 1][y])
            {
                len = max(len, 1 + findPathDfs(matrix, dp, x + 1, y));
            }
            if (y < n - 1 && curr < matrix[x][y + 1])
            {
                len = max(len, 1 + findPathDfs(matrix, dp, x, y + 1));
            }
            // Update dp.
            dp[x][y] = len;

            return len;
        }

        size_t m = 0;
        size_t n = 0;
    };

    //---------------------------------------------------------------------------------------
    // 1284. Minimum Number of Flips to Convert Binary Matrix to Zero Matrix (Hard)
    //---------------------------------------------------------------------------------------
    class Solution1284
    {
    public:
        int minFlips(vector<vector<int>>& mat)
        {
            // Idea:
            // This is hard... No smart tricky to determine the answer, so we must just try
            // every steps.
            // Unlike most maze question, the goal is not to get a certain position.
            // The goal is a zero matrix. So, the entire matrix must be considered as a
            // 'state'. We need to figure out if we can reach the state.
            // Pay attention to the following facts:
            // * The matrix only have 0 or 1, and it is not big (3x3), so it can be encoded
            //   into a int. We only need at most 9 bits to store the 'state'.
            // * BFS is more suitable in this case.
            int initialState = 0;
            const size_t m = mat.size();
            const size_t n = mat[0].size();

            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    initialState |= mat[i][j] ? 1 << ( i * n + j ) : 0;
                }
            }

            queue<int> states({ initialState });
            unordered_set<int> seenStates({ initialState });
            int result = 0;
            while (!states.empty()) // while there is remaining state to be processed.
            {
                const size_t len = states.size();
                for (size_t s = 0; s < len; ++s)
                {
                    const int currState = states.front();
                    states.pop();
                    if (currState == 0) // Solved!
                    {
                        return result;
                    }
                    // Understand this state, try every cells.
                    for (size_t i = 0; i < m; i++)
                    {
                        for (size_t j = 0; j < n; j++)
                        {
                            int newState = currState;
                            // Flip this cell, update the nearby cells, so we have a new state.
                            for (const auto& dir : directions)
                            {
                                size_t newX = i + dir.first;
                                size_t newY = j + dir.second;
                                if (newX >= 0 && newX < m && newY >= 0 && newY < n)
                                {
                                    // To flip, we can use XOR.
                                    newState ^= 1 << ( newX * n + newY );
                                }
                            }

                            if (seenStates.count(newState) == 0)
                            {
                                seenStates.insert(newState);
                                states.push(newState);
                            }
                        }
                    }
                }
                result++;
            }

            return -1;
        }

    private:
        // Unlike the usual matrix traversal question, there is {0, 0}, meaning flipping
        // the current cell.
        vector<pair<int, int>> directions = { {0, 0}, {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
    };

    //---------------------------------------------------------------------------------------
    // 939. Minimum Area Rectangle
    // Topic: hash table.
    //
    //---------------------------------------------------------------------------------------
    class Solution939
    {
    public:
        // The following implementation iterates the points many times. It is O(n^2).
        int minAreaRect(vector<vector<int>>& points)
        {
            // To find a rectangle, we should find a diagonal point first. And then, check
            // whether they can form a rectangle.
            sort(points.begin(), points.end(), [](const vector<int>& left, const vector<int>& right)
                {
                    return left[0] < right[0];
                });

            // <coordinate x axis, <coordinate y at this x coordinate> >
            map<int, unordered_set<int>> xAxisMap;
            for (const auto& point : points)
            {
                xAxisMap[point[0]].insert(point[1]);
            }

            int result = INT_MAX;
            const size_t length = points.size();
            for (size_t i = 0; i < length; i++)
            {
                int lastX = INT_MAX;
                for (size_t j = i + 1; j < length; j++)
                {
                    // The points has been sorted, so if we meet a bigger X, the next area
                    // (if there is one) must be greater, so we can stop now.
                    const int x2 = points[j][0];
                    if (x2 > lastX)
                    {
                        break;
                    }

                    const int x1 = points[i][0];
                    const int y1 = points[i][1];
                    const int y2 = points[j][1];
                    // Check if two are in diagonal.
                    if (x1 != x2 && y1 != y2)
                    {
                        // Check there are two other points that make a rectangle.
                        if (xAxisMap[x1].count(y2) && xAxisMap[x2].count(y1))
                        {
                            int area = abs(( x2 - x1 ) * ( y2 - y1 ));
                            result = min(result, area);
                            lastX = x2;
                        }
                    }
                }
            }

            return result == INT_MAX ? 0 : result;
        }
    };

    //---------------------------------------------------------------------------------------
    // 778. Swim in Rising Water (Hard)
    // Topic: DFS, BFS, Priority queue, BIS, Union Find
    //---------------------------------------------------------------------------------------
    // I can devise two possible solutions:
    // 1. Use BFS + priority queue.
    // 2. Use BFS/DFS + binary search. Because the question set the upper bound of the cell
    //    to n^2. We can call binary search between 0 ~ n^2. Try if we can reach by not
    //    exceeding that elevation. (TODO)
    class Solution778
    {
    public:

        // Use BFS + priority queue.
        int swimInWater(vector<vector<int>>& grid)
        {
            m = grid.size();
            n = grid[0].size();
            // Idea: Do special BFS - always pick the cell having smallest elevation.
            using Cell = pair<int, int>; // <elevation, compiled coordinate integer: x * n + y>.
            // The priority_queue by default is max heap. If we want to turn it into a
            // min heap, we must use > for the comparison.
            auto comp = [](const Cell& left, const Cell& right)
            {
                return left.first > right.first;
            };
            priority_queue<Cell, deque<Cell>, decltype( comp )> cellQueue(comp);

            // Store the compiled coordinate integer: x * n + y.
            unordered_set<int> visited;
            visited.insert(0);

            // Start from the left-top corner.
            cellQueue.emplace(Cell( grid[0][0], 0 ));

            int result = INT_MIN;
            while (!cellQueue.empty())
            {
                const int x = static_cast<int>( cellQueue.top().second / n );
                const int y = static_cast<int>( cellQueue.top().second % n );
                cellQueue.pop();

                result = max(result, grid[x][y]);
                if (x == m - 1 && y == n - 1)
                {
                    return result;
                }

                // Push nearby cells into queue.
                vector<pair<int, int>> dirs = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
                for (size_t i = 0; i < dirs.size(); ++i)
                {
                    const int newX = x + dirs[i].first;
                    const int newY = y + dirs[i].second;
                    int compiled = static_cast<int>( newX * n + newY );
                    if (newX >= 0 && newX < m && newY >= 0 && newY < n && visited.count(compiled) == 0)
                    {
                        cellQueue.emplace(Cell(grid[newX][newY], compiled));
                        visited.insert(compiled);
                    }
                }
            }

            return result;
        }

    private:
        size_t m = 0;
        size_t n = 0;
    };

    //---------------------------------------------------------------------------------------
    // 419. Battleships in a Board
    //
    // Follow up: Could you do it in one-pass, using only O(1) extra memory and without
    // modifying the values board?
    //---------------------------------------------------------------------------------------
    class Solution419
    {
    public:
        int countBattleships(vector<vector<char>>& board)
        {
            // Idea: The head of battleship's left and top cell can not be 'X'.
            // It could be the matrix boundary or '.'.

            int count = 0;
            for (int i = 0; i < board.size(); i++)
            {
                for (int j = 0; j < board[0].size(); ++j)
                {
                    if (board[i][j] == 'X' && ( i == 0 || board[i - 1][j] == '.' ) && ( j == 0 ||
                        board[i][j - 1] == '.' ))
                    {
                        count++;
                    }
                }
            }

            return count;
        }
    };

    //---------------------------------------------------------------------------------------
    // Test function
    //---------------------------------------------------------------------------------------
    void TestMatrix()
    {
        // 994. Rotting Oranges
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

        // 2013. Detect Squares

        DetectSquares* detectSquares = new DetectSquares();
        detectSquares->add({ 3, 10 });
        detectSquares->add({ 11, 2 });
        detectSquares->add({ 3, 2 });
        int resultI = detectSquares->count({ 11, 10 }); // return 1. You can choose:
                                       //   - The first, second, and third points
        resultI = detectSquares->count({ 14, 8 });  // return 0. The query point cannot form a square with any points in the data structure.
        detectSquares->add({ 11, 2 });    // Adding duplicate points is allowed.
        resultI = detectSquares->count({ 11, 10 }); // return 2. You can choose:
                                       //   - The first, second, and third points
                                       //   - The first, third, and fourth points

        delete detectSquares;

        // 329. Longest Increasing Path in a Matrix (Hard)
        Solution329 sol329;
        LeetCodeUtil::BuildIntMatrixFromString("[[3,4,5],[3,2,6],[2,2,1]]", &inputMatrix);
        cout << "\n329. Longest Increasing Path in a Matrix: " << sol329.longestIncreasingPath(inputMatrix) << endl;

        // 1284. Minimum Number of Flips to Convert Binary Matrix to Zero Matrix (Hard)
        Solution1284 sol1284;
        LeetCodeUtil::BuildIntMatrixFromString("[[0,0],[0,1]]", &inputMatrix);
        cout << "\n1284. Minimum Number of Flips to Convert Binary Matrix to Zero Matrix (Hard): " << sol1284.minFlips(inputMatrix) << endl;

        // 939. Minimum Area Rectangle
        // Input: points = [[1,1],[1,3],[3,1],[3,3],[2,2]]
        // Output: 4
        // Input: points = [[1,1],[1,3],[3,1],[3,3],[4,1],[4,3]]
        // Output: 2
        // Input: points = [[3,2],[3,1],[4,4],[1,1],[4,3],[0,3],[0,2],[4,0]]
        // Output: 0
        Solution939 sol939;
        LeetCodeUtil::BuildIntMatrixFromString("[[1,1],[1,3],[3,1],[3,3],[4,1],[4,3]]", &inputMatrix);
        cout << "\n939. Minimum Area Rectangle: " << sol939.minAreaRect(inputMatrix) << endl;

        // 778. Swim in Rising Water (Hard)
        // Input: grid = [[0,1,2,3,4],[24,23,22,21,5],[12,13,14,15,16],[11,17,18,19,20],[10,9,8,7,6]]
        // Output: 16
        Solution778 sol778;
        LeetCodeUtil::BuildIntMatrixFromString("[[0,1,2,3,4],[24,23,22,21,5],[12,13,14,15,16],[11,17,18,19,20],[10,9,8,7,6]]", &inputMatrix);
        cout << "\n778. Swim in Rising Water: " << sol778.swimInWater(inputMatrix) << endl;

        // 419. Battleships in a Board
        // Input: board = [["X",".",".","X"],[".",".",".","X"],[".",".",".","X"]]
        // Output: 2
        Solution419 sol419;
        vector<vector<char>> inputVVC = { {'X','.','.','X' }, { '.', '.', '.', 'X' }, { '.', '.', '.', 'X' } };
        cout << "\n419. Battleships in a Board: " << sol419.countBattleships(inputVVC) << endl;
    }
}