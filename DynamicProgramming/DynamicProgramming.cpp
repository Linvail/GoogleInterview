// DynamicProgramming.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "LeetCodeUtil.h"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

//---------------------------------------------------------------------------------------
// 818. Race Car (Hard)
// Topic: DP
//---------------------------------------------------------------------------------------
class Solution818
{
public:
    int racecar(int target)
    {
        // AAA: 0 -> 1 -> 3 -> 7 -> 15
        //           1    2    4     4
        //       2^1 - 1           2^4 -1
        // Idea:
        // We have 3 types of maneuver to reach the target.
        // 1. Keep accelerate and reach.
        // 2. Before reach, reverse, accelerate 0+ times, and reverse again.
        //    Then, start to accelerate then reach. (we have a sub-problem).
        // 3. Accelerate until pass the target. Reverse. Then start to accelerate again
        //    (we have a sub-problem backward).

        // dp[i] : the length of the shortest sequence of instructions to get position i.
        // dp[target] is the answer.
        //
        // dp[i] = count of acceleration (countA) if we get type 1.
        // dp[i] = countA + 1 + countB + 1 + dp[i - (j - k)] where countB is the count accelerate backward.
        // dp[i] = countA + 1 + dp[j - i] where j is the position we reached by countA, j > i. This is type 3.

        // Firstly, deal with the lucky case - type 1.
        int j = 0;
        int countA = 0;
        while (j <= target)
        {
            if (j == target)
            {
                return countA;
            }
            countA++;
            j = ( 1 << countA ) - 1;
        }

        vector<int> dp(target + 1, INT_MAX);
        dp[0] = 0;
        dp[1] = 1;

        // Iterate for all possible length...
        for (int i = 2; i <= target; ++i)
        {
            // Build dp table from beginning.
            j = 1; // The position we reach by keep accelerating.
            countA = 1;
            int upperBound = 0;
            for (; j < i; j = upperBound)
            {
                // Deal with case type 2. j < i < upperBound
                // Now we need to reverse twice and handle the subproblem.
                // k is the distance we move backward.
                int countB = 0;
                int lowerBound = 0;
                for (int k = 0; k < j; k = lowerBound)
                {
                    // 0......(j-k)....j.....i....
                    dp[i] = min(dp[i], countA + 1 + countB + 1 + dp[i - (j - k)]);
                    countB++;
                    lowerBound = ( 1 << countB ) - 1;
                }
                countA++; // Must be in the end.
                upperBound = ( 1 << countA ) - 1;
            }

            // Case type 2. i < j. Reverse one and handle the subproblem.
            int candidate = j == i ? countA : countA + 1 + dp[j - i];
            dp[i] = min(dp[i], candidate);
        }

        return dp[target];
    }
};

//---------------------------------------------------------------------------------------
// 1048. Longest String Chain
//
// Hint:
// 1. Instead of adding a character, try deleting a character to form a chain in
// reverse.
// 2. For each word in order of length, for each word2 which is word with one character
// removed, length[word2] = max(length[word2], length[word] + 1).
//---------------------------------------------------------------------------------------
bool checkPredecessor(const string& longStr, const string& shortStr)
{
    for (int i = 0; i < longStr.size(); ++i)
    {
        if (longStr.compare(i + 1, string::npos, shortStr, i, string::npos) == 0 &&
            longStr.compare(0, i, shortStr, 0, i) == 0)
        {
            return true;
        }
    }

    return false;
}

int longestStrChain(vector<string>& words)
{
    // Define dp[s] : the length of the longest possible word chain with string s.
    // dp[s] = max(dp[s], dp[t] + 1). s is predecessor of t.
    const int len = words.size();
    if (len < 2)
    {
        return len;
    }

    sort(words.begin(), words.end(),
        [](const string& str1, const string& str2) { return str1.size() < str2.size(); });

    vector<int> dp(len, 1);
    int result = 1;

    for (int j = len - 1; j >= 0; --j)
    {
        const size_t curLen = words[j].size();
        for (int k = j - 1; k >= 0; --k)
        {
            const size_t prevLen = words[k].size();
            if (prevLen == curLen)
            {
                continue;
            }
            else if (prevLen < curLen - 1)
            {
                break;
            }

            // Check if words[k] is predecessor of words[j].
            // We already know words[k]'s length is words[j]'s length - 1 here.
            if (checkPredecessor(words[j], words[k]))
            {
                // k is the predecessor of j.
                dp[k] = max(dp[k], dp[j] + 1);

                result = max(result, dp[k]);
            }
        }
    }

    return result;
}

//---------------------------------------------------------------------------------------
// 1937. Maximum Number of Points with Cost (Medium but actually Hard)
// Topic: Dynamic programming
//
// Hint: dp[i][j] is the maximum number of points you can have if points[i][j] is the
// most recent cell you picked.
//
// The standard DP algorithm will cause TLE. To pass, we need an advanced algorithm,
// making this be a hard question.
//---------------------------------------------------------------------------------------
class Solution1937
{
public:
    long long maxPoints(vector<vector<int>>& points)
    {
        // dp[i][j] = max(dp[i][j], dp[x][y] + points[i][j] - abs(j - y))
        // i - 1 = x.
        const int m = points.size();
        const int n = points[0].size();

        vector<vector<long long>> dp(m, vector<long long>(n, 0));
        for (int j = 0; j < n; ++j)
        {
            dp[0][j] = points[0][j];
        }

        for (int i = 1; i < m; ++i)
        {
            // The following code would cause TLE.
            /*for (int j = 0; j < n; ++j)
            {
                const int point = points[i][j];
                for (int k = 0; k < n; ++k)
                {
                    dp[i][j] = max(dp[i][j], dp[i - 1][k] + point - abs(j - k));
                }
            }*/

            // The following code is a hard mathematic trick. Hard to explain.
            // Note that we modify and re-use the space of the previous row.
            // So, in fact, we actually don't need a matrix for this DP algorithm.
            // We just need two vector with m size because we only need to use the
            // previous row.

            vector<long long>& prevRow = dp[i - 1];
            // Sweep right
            for (int j = 1; j < n; ++j)
            {
                prevRow[j] = max(prevRow[j], prevRow[j - 1] - 1);
            }
            // Sweep right
            for (int j = n - 2; j >= 0; --j)
            {
                prevRow[j] = max(prevRow[j], prevRow[j + 1] - 1);
            }

            for (int j = 0; j < n; ++j)
            {
                dp[i][j] = prevRow[j] + points[i][j];
            }
        }

        return *std::max_element(dp[m - 1].begin(), dp[m - 1].end());
    }
};

//---------------------------------------------------------------------------------------
// 1986. Minimum Number of Work Sessions to Finish the Tasks
// Topic: DP, binary search.
//
//---------------------------------------------------------------------------------------
class Solution1986
{
public:

    int m_sessionTime = 0;

    int minSessions(vector<int>& tasks, int sessionTime)
    {
        m_sessionTime = sessionTime;
        // Remember, you may complete the tasks in any order.
        // We start from the biggest task because it can make our recursion complete
        // sooner (put a task into session, and find the next session that can
        // contain the next biggest task).
        sort(tasks.begin(), tasks.end(), greater<int>());

        int left = 1; // We at least need 1 session.
        int right = tasks.size(); // We at most need n session.
        // Using binary search to find the minimum count of session.

        while (left < right)
        {
            int mid = left + ( right - left ) / 2;

            // Allocate mid sessions, tasks will be filled in.
            vector<int> sessions(mid, 0);
            if (canSolve(tasks, 0, sessions))
            {
                right = mid; // mid is fine, but try less sessions.
            }
            else
            {
                left = mid + 1; // Try more sessions.
            }
        }

        return right;
    }

    bool canSolve
        (
        const vector<int>& tasks,  //!< Tasks need to be done.
        int start,  //!< The starting index of tasks.
        vector<int>& sessions  //!< The sessions we can use.
        )
    {

        if (start == tasks.size())
        {
            // All tasks have been put in the sessions.
            return true;
        }

        // Iterate over the sessions, find a session that can hold the next task
        // (tasks[start]).
        // Put it in and call this recursively. If it returns false, it means the
        // task shouldn't be put in this session. We have to take it out.
        for (int i = 0; i < sessions.size(); ++i)
        {
            if (sessions[i] + tasks[start] <= m_sessionTime)
            {
                sessions[i] += tasks[start];
                if (canSolve(tasks, start + 1, sessions))
                {
                    return true;
                }

                // We cannot solve by putting tasks in this session. Take it out.
                sessions[i] -= tasks[start];
                // If this session is originally empty, then we can break because the later
                // session are all empty, too.
                if (sessions[i] == 0)
                {
                    break;
                }
            }
        }

        return false;
    }
};

int main()
{
    std::cout << "DynamicProgramming!\n";

    // 818. Race Car (Hard)
    // Input: target = 3
    // Output: 2
    // Input: target = 6
    // Output: 5
    // Input: target = 4
    // Output: 5
    // Input: target = 5
    // Output: 7
    Solution818 sol818;
    int target = 7;
    cout << "\nRace Car: " << sol818.racecar(target) << endl;

    // 1048. Longest String Chain
    // Input: words = ["a","b","ba","bca","bda","bdca"]
    // Output: 4
    // Input: words = ["xbc","pcxbcf","xb","cxbc","pcxbc"]
    // Output: 5
    // Input: words = ["abcd","dbqca"]
    // Output: 1
    vector<string> words = { "xbc","pcxbcf","xb","cxbc","pcxbc" };
    cout << "\n1048. Longest String Chain : " << longestStrChain(words) << endl;

    // 1937. Maximum Number of Points with Cost
    // Input: points = [[1, 2, 3], [1, 5, 1], [3, 1, 1]]
    // Output : 9
    // Input: points = [[1,5],[2,3],[4,2]]
    // Output: 11
    Solution1937 sol1937;
    vector<vector<int>> inputVVI;
    LeetCodeUtil::BuildIntMatrixFromString("[[1,5],[2,3],[4,2]]", &inputVVI);
    cout << "\n1937. Maximum Number of Points with Cost: " << sol1937.maxPoints(inputVVI) << endl;

    // 1986. Minimum Number of Work Sessions to Finish the Tasks
    // Input: tasks = [3, 1, 3, 1, 1], sessionTime = 8
    // Output : 2
    // tasks = {3, 4, 7, 8, 10} sessionTime = 12 <-- Greedy won't work on this.

    Solution1986 sol1986;
    vector<int> inputVI = { 3, 4, 7, 8, 10 };
    int sessionTime = 12;
    cout << "\n1986. Minimum Number of Work Sessions to Finish the Tasks: " << sol1986.minSessions(inputVI, sessionTime) << endl;

}
