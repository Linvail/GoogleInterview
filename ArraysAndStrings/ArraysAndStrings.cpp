// ArraysAndStrings.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <algorithm> // std::reverse
#include <vector>
#include <unordered_map>
#include <map>

#include "LeetCodeUtil.h"

using namespace std;

//---------------------------------------------------------------------------------------
// 11. Container With Most Water
//
// Topic: two pointer
//---------------------------------------------------------------------------------------
int maxArea(vector<int>& height)
{
    size_t left = 0;
    size_t right = height.size() - 1;

    size_t area = 0;
    while (left < right)
    {
        size_t minHeight = min(height[left], height[right]);
        area = max(area, minHeight * ( right - left ));

        // Now, move left and right toward center.
        // If we move left or height, and found the minHeight doesn't change,
        // we should keep move because it meaningless to calculate the area again (it
        // must be smaller).

        while (left < right && minHeight == height[left])
        {
            left++;
        }
        while (left < right && minHeight == height[right])
        {
            right--;
        }
    }

    return area;
}

//---------------------------------------------------------------------------------------
// 31. Next Permutation
// The replacement must be in place and use only constant extra memory.
// Topic: two pointers
//---------------------------------------------------------------------------------------

void nextPermutation(vector<int>& nums)
{
    // This question requires good observation. We should make a longer example and try
    // to find the next permutation manually. Then, observe it carefully.
    // For example:
    // 1  2  7  4  3  1
    //    <------------------- scan from right to left.
    // To find "lexicographically next greater", we must make one digit bigger by swapping
    // a bigger behind it with it.
    // It gives a fact: We should keep scan until we found nums[i] < nums[i + 1]
    //
    // We want 'next' permutation manually, 'next' means smallest. So, we must find the
    // first number greater than '2' and swap them. In this case, it is 3.
    // 1  3  7  4  2  1
    //       ^^^^^^^^^^
    // Not done yet, to make it 'smallest', we must make rest of sequence sorted in
    // ascending order.
    // 1  3  1  2  4  7 <- done

    const int len = nums.size();
    for (int i = len - 2; i >= 0; --i)
    {
        if (nums[i] < nums[i + 1])
        {
            // search the first number greater than nums[i].
            // The question said only constant extra memory is allowed, so we cannot
            // use hash table to speed up the search.
            int j = len - 1;
            for (; j > i; --j)
            {
                if (nums[j] > nums[i])
                {
                    break;
                }
            }

            swap(nums[i], nums[j]);
            std::reverse(nums.begin() + i + 1, nums.end());
            return;
        }
    }

    // For the completely descending sorted case, we're unable to find the i to make
    // nums[i] < nums[i + 1]. In this case, we should just reverse the vector.
    // 5 4 3 2 1
    std::reverse(nums.begin(), nums.end());
}

//---------------------------------------------------------------------------------------
// 424. Longest Repeating Character Replacement
// Topic: Sliding window
// Input: s = "AABABBA", k = 1
//---------------------------------------------------------------------------------------
int characterReplacement(string s, int k)
{
    // Idea: If there is no limit of k, how many steps should we take?
    // That will be s.size() - max appearance count of certain char (meaning we
    // replace all the others).
    // So, we are looking for the substrings whose length - max appearance count >= k.
    int result = 0;
    int left = 0;  // left boundary the sliding window. The start of the substring.
    vector<int> charAppearance(26, 0);
    int maxCountAppear = 0;

    for (int i = 0; i < s.size(); ++i)
    {
        charAppearance[s[i] - 'A']++;
        maxCountAppear = max(maxCountAppear, charAppearance[s[i] - 'A']);

        // AABABBA
        //     ^
        while (i - left + 1 - maxCountAppear > k)
        {
            // This substring is invalid, adjust the window.
            charAppearance[s[left] - 'A']--;
            left++;
        }

        result = max(result, i - left + 1);
    }

    return result;
}

//---------------------------------------------------------------------------------------
// 844. Backspace String Compare (Easy)
//
// Follow up: Can you solve it in O(n) time and O(1) space?
//
// Phone interview
//---------------------------------------------------------------------------------------
bool backspaceCompare(string s, string t)
{
    const int len1 = s.size();
    const int len2 = t.size();
    int i = len1 - 1;
    int j = len2 - 1;

    while (i >= 0 || j >= 0)
    {
        int countOfDel = 0;
        while (i >= 0 && (s[i] == '#' || countOfDel > 0))
        {
            if (s[i] == '#')
            {
                countOfDel++;
            }
            else
            {
                countOfDel--;
            }
            i--;
        }

        countOfDel = 0;
        while (j >= 0 && (t[j] == '#' || countOfDel > 0))
        {
            if (t[j] == '#')
            {
                countOfDel++;
            }
            else
            {
                countOfDel--;
            }
            j--;
        }

        // Both reach the end.
        if (i == j && i == -1)
        {
            return true;
        }

        if (( i == -1 && j != -1 ) || ( i != -1 && j == -1 ))
        {
            return false;
        }

        if (s[i] == t[j])
        {
            i--;
            j--;
        }
        else
        {
            break;
        }
    }

    return i == j && i == -1;
}

//---------------------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------------------
int main()
{
    std::cout << "Arrays and Strings\n";

    // 11. Container With Most Water
    // Input: height = [1,8,6,2,5,4,8,3,7]
    // Output: 49
    vector<int> intV = { 1,8,6,2,5,4,8,3,7 };
    cout << "\n11. Container With Most Water: " << maxArea(intV) << endl;

    // 31. Next Permutation
    // Input: nums = [1, 1, 5]
    // Output : [1, 5, 1]
    intV = { 8, 7, 4, 3, 1 };
    nextPermutation(intV);
    cout << "\n31. Next Permutation: ";
    LeetCodeUtil::PrintVector(intV);
    cout << "\n";

    // 424. Longest Repeating Character Replacement
    // Input: s = "AABABBA", k = 1
    // Output: 4
    string inputStr = "AABABBA";
    int k = 1;
    cout << "\n424. Longest Repeating Character Replacement for " << inputStr << " : ";
    cout << characterReplacement(inputStr, k) << endl;

    // 844. Backspace String Compare (Easy)
    // Input: s = "ab##", t = "c#d#"
    // Output: true
    // "bxj##tw", "bxo#j##tw"
    // Output: true
    // "a##c", "#a#c"
    // Output: true
    // "nzp#o#g", "b#nzp#o#g"
    // Output: true
    inputStr = "nzp#o#g";
    string inputStr2 = "b#nzp#o#g";
    cout << "\n844. Backspace String Compare (Easy): " << backspaceCompare(inputStr, inputStr2) << endl;
}
