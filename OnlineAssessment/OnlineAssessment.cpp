// OnlineAssessment.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <map>

using namespace std;

//---------------------------------------------------------------------------------------
// 904. Fruit Into Baskets
//
// This one is the same as "159. Longest Substring with At Most Two Distinct Characters".
//---------------------------------------------------------------------------------------
int totalFruit(vector<int>& fruits)
{
    // It seems that we can sliding window concept.
    // We scan from left to right, if we are luck, don't meet the 3rd 'fruit', then the
    // length of the entire list is the answer.
    // If we meet the 3rd, we need to exclude the 1st fruit. In other words, adjust the
    // the left boundary of the sliding window.
    // a b a b c cccc
    //         ^
    // 0 1 2 3 4

    // 1,0,1,4,1,4,1,2,3
    //       ^

    int result = 0;
    size_t left = 0;

    using FRUIT_T = int;
    using IDX_T = size_t;
    // Record the latest index of the fruit type.
    unordered_map<FRUIT_T, IDX_T> window;

    for (int i = 0; i < fruits.size(); ++i)
    {
        window[fruits[i]] = i;

        // If meet 3rd fruit(char)..
        while (window.size() > 2)
        {
            // Continue to move left boundary until we found one type of fruit
            // won't appear anymore.
            if (window[fruits[left]] == left)
            {
                window.erase(fruits[left]);
            }
            left++;
        }

        result = max(result, static_cast<int>(i - left + 1));
    }

    return result;
}

//---------------------------------------------------------------------------------------
// 975. Odd Even Jump (Hard)
//
//---------------------------------------------------------------------------------------
int oddEvenJumps(vector<int>& arr)
{
    const int len = arr.size();
    // Idea:
    // *. Use dynamic programming.
    // *. Make two vectors.
    // byOddJump[i] means that whether we can reach the destination by starting with odd
    // from the index i.
    // byOddJump[len - 1] is the destination, so mark it as true.
    vector<bool> byOddJump(len, false);
    // byEvenJump[i] means that whether we can reach the destination by starting with even
    // from the index i.
    vector<bool> byEvenJump(len, false);
    // [len - 1] is the destination, so mark it as true.
    byOddJump[len - 1] = true;
    byEvenJump[len - 1] = true;
    // *. The destination is on the last number, so we iterate from right to left.
    // We will build byOddJump/byEvenJump from len-2, len-3, until index 0.

    // *. Odd/Even jump must be alternately executed. It means that if I want to do a
    // a odd jump now, the previous jump must be odd.
    // So, byOddJump must be inferred from byEvenJump, but from which index, that is the
    // key of the state transition equation.

    // * Odd jump can only jump to a element that is >= itself.
    // Assume, len = 5
    // So, for byOddJump[2], we need to search arr[3~4] to find the first number which is
    // >= arr[2] (not less).
    // To speed up the search, we can use hash table to record the value/index pair.
    // And, we can use map.lower_bound().
    // For byEvenJump, we need the first number <= arr[i] (means not greater).
    // Cannot use map.upper_bound() directly because it returns the 1st number > the
    // target. But if we -1, then we can get the 1st number that is not greater.

    map<int, size_t> numsIndex;
    numsIndex[arr[len - 1]] = len - 1;
    int result = 1; // The destination always counts.

    for (int i = len - 2; i >= 0; --i)
    {
        auto itNotLess = numsIndex.lower_bound(arr[i]);
        auto itGreater = numsIndex.upper_bound(arr[i]);

        if (itNotLess != numsIndex.end())
        {
            byOddJump[i] = byEvenJump[itNotLess->second];
        }

        // If itGreater is the 1st element, decrementing it is undefined behavior.
        // VS will change it to end(), which is nice, but the compiler of LeetCode won't change it.
        // itGreater might be end() here, it means nothing is greater than arr[i].
        // Decrementing it would still give us the 1st element that is not greater than arr[i].
        if (itGreater != numsIndex.begin())
        {
            itGreater--;
            byEvenJump[i] = byOddJump[itGreater->second];
        }

        // Since the 1st jump is odd jump, so if byOddJump[i] is true, then it
        // is a good starting index.
        if (byOddJump[i])
        {
            result++;
        }
        numsIndex[arr[i]] = i;
    }

    return result;
}

//---------------------------------------------------------------------------------------
// 410. Split Array Largest Sum (Hard)
// Topic: DP, binary search, greedy(?)
// Phone interview
//
// There are two major solutions: DP and binary search. None of them are easy.
// Binary search seems faster.
//---------------------------------------------------------------------------------------

class Solution410_dp
{
public:
    int splitArray(vector<int>& nums, int m)
    {
        const int len = nums.size();
        // len = mums.size()
        // dp[i][j] : answer of sub-problem, splitting nums[0~j] into i groups
        // The final answer is dp[m][len-1]
        // dp[1][j] = sum (nums[0] ~ sum[j]). Prefix sum at j.
        // dp[i][j] = min( max( dp[i - 1][k], sum(k+1, j) ) ). i >= 2. 0 <= k < j
        // Need to iterate i, j, k. triple for-loops.
        //
        // Example, nums = [7,2,5,10,8], m = 2
        // dp[2][4] = min of
        // k = 0 : max( dp[1][0], sum[2,5,10,8] ) = max(7, 25) = 25
        // k = 1 : max( dp[1][1], sum[5,10,8] ) = max(9, 23) = 23
        // k = 2 : max( dp[1][2], sum[10,8] ) = max(14, 18) = 18
        // k = 3 : max( dp[1][3], sum[8] ) = max(24, 8) = 24

        // Build prefix sum.
        vector<int> prefixSum(len, 0);
        prefixSum[0] = nums[0];
        for (int i = 1; i < len; ++i)
        {
            prefixSum[i] = prefixSum[i - 1] + nums[i];
        }

        vector<vector<int>> dp(m, vector<int>(len, INT_MAX));

        // Set known value for the 1 group case.
        for (int j = 0; j < len; ++j)
        {
            // 1 group
            dp[0][j] = prefixSum[j];
        }


        for (int i = 1; i < m; ++i) // Start from 2 groups
        {
            // A group cannot be empty, so for 2 groups, we must have 2 numbers.
            // So, j cannot be less than i.
            for (int j = i; j < len; ++j)
            {
                // A group cannot be empty. If we target to 2 groups(i=2), there is
                // at one group in the head, so at least 1 number should be skipped.
                for (int k = i - 1; k < j; ++k)
                {
                    int tempMax = max(dp[i - 1][k], prefixSum[j] - prefixSum[k]);
                    dp[i][j] = min(dp[i][j], tempMax);
                }
            }
        }

        return dp[m - 1][len - 1];
    }
};


class Solution410_bis
{
public:
    int splitArray(vector<int>& nums, int m)
    {
        // Idea:
        // There is only one group (m=1). The answer is sum of all numbers.
        // If m = nums.size(). Each group only have one number. The answer is max among
        // all numbers. So, we have lower bound and upper bound. The answer must be
        // within the range.
        // We must find the minimum value.
        int left = INT_MIN;
        int right = 0;
        for (const auto& num : nums)
        {
            left = max(left, num);
            right += num;
        }

        // To do binary search, we must need to how to adjust left/right.
        // For arbitrary value - mid, if we can make m group, we should try a
        // smaller value, meaning we should adjust right to midVale.
        // If we cannot make m group, we should try a greater mid value.
        while (left < right)
        {
            int mid = left + ( right - left ) / 2;
            if (canSplit(nums, m, mid))
            {
                right = mid;
            }
            else
            {
                // In the end, this
                left = mid + 1;
            }
        }

        // right stay on the last valid value.
        return right;
    }

    bool canSplit(vector<int>& nums, int m, int value)
    {
        int partialSum = 0;
        int group = 1;
        for (int i = 0; i < nums.size(); ++i)
        {
            partialSum += nums[i];
            if (partialSum > value)
            {
                group++;
                if (group > m)
                {
                    return false;
                }
                partialSum = nums[i];
            }
        }

        return true;
    }
};

//---------------------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------------------
int main()
{
    std::cout << "Online Assessment!\n";

    // 904. Fruit Into Baskets
    vector<int> intV = { 1,0,1,4,1,4,1,2,3 };
    cout << "\n904 Fruit Into Baskets: " << totalFruit(intV) << endl;

    // 975. Odd Even Jump (Hard)
    // Input: arr = [10,13,12,14,15]
    // Output: 2
    // Input: arr =  [5,1,3,4,2]
    // Output: 3
    intV = { 5,1,3,4,2 };
    cout << "\n975 Odd Even Jump: " << oddEvenJumps(intV) << endl;

    // 410. Split Array Largest Sum (Hard)
    // Input: nums = [7, 2, 5, 10, 8], m = 2
    // Output : 18
    //intV = { 7, 2, 5, 10, 8 };
    intV = { 7, 2, 5, 10, 8 };
    int m = 2;
    Solution410_dp sol410_dp;
    Solution410_bis sol410_bis;
    cout << "\n410. Split Array Largest Sum: " << sol410_bis.splitArray(intV, m) << endl;

}
