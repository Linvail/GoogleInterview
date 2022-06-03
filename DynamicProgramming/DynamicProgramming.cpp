// DynamicProgramming.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "LeetCodeUtil.h"

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

}
