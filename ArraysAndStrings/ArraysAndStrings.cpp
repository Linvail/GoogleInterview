// ArraysAndStrings.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <algorithm> // std::reverse
#include <vector>
#include <unordered_map>
#include <map>
#include <set>

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
// 777. Swap Adjacent in LR String
// Topic: Two pointers, brainteaser
//
// Google L4 US |  Phone | Swap LR Problem
//---------------------------------------------------------------------------------------

class Solution777
{
public:
    // This is a brainteaser which requires keen perception.
    // Key properties:
    // 1.
    // We can only do "XL" -> "LX" and "RX" -> "XR". We cannot do them in reverse order.
    // So, L can only move forward and R can only move backward.
    // 2. R and L cannot be switched, so the sequence of them won't change.
    // We need to examine the two strings to determine if they meet these properties.
    bool canTransform(string start, string end)
    {
        // This can be solved by two pointers or one pointer.
        // In the two pointers approach. There is a catch - you need to deal with
        // the remaining letters after the main loops end. There must be no R/L in the
        // tail.

        // The following code shows the one pointer approach.
        // Bad: L cannot move backward. start's L must be in back of end's.
        // RXLX
        // RXXL
        //   ^
        // Bad: R cannot move forward. start's R must be in front of end's.
        // XRXL
        // RXXL
        // ^
        // Bad: Bad sequence. Meet L but we expect R.
        // RXRXLXLX
        // XLXLRXRX
        //  ^
        // Good:
        // "RXXLRXRXL"
        // "XRLXXRRLX"

        int countR = 0;
        int countL = 0;
        for (int i = 0; i < start.size(); ++i)
        {
            // start's R must be in front of end's.
            if (start[i] == 'R')
            {
                countR++;
            }
            // countR and countL cannot be > 0 at any moment.
            if (countR > 0 && countL > 0)
            {
                // Bad sequence.
                // "XXRXL"
                // "LXRXX"
                return false;
            }

            if (end[i] == 'R')
            {
                countR--;
            }
            // R can be move backward, so if countR is 2+, it is fine.
            // But it can be < 0.
            if (countR < 0)
            {
                return false;
            }

            // start's L must be in back of end's.
            if (end[i] == 'L')
            {
                countL++;
            }

            // countR and countL cannot be > 0 at any moment.
            if (countR > 0 && countL > 0)
            {
                // Bad sequence.
                // RLX
                // XLR
                return false;
            }

            if (start[i] == 'L')
            {
                countL--;
            }
            if (countL < 0)
            {
                return false;
            }
        }

        return !countR && !countL;
    }
};

//---------------------------------------------------------------------------------------
// 2158. Amount of New Area Painted Each Day (Hard)
//
//---------------------------------------------------------------------------------------
class Solution2158
{
public:
    vector<int> amountPainted(vector<vector<int>>& paint)
    {
        // Find the min day and max day in input tasks.
        int minDay = INT_MAX;
        int maxDay = INT_MIN;
        for (const auto& task : paint)
        {
            minDay = min(minDay, task[0]);
            maxDay = max(maxDay, task[1]);
        }

        // Simulate a painting canvas. Fill with 1, meaning it's available to draw.
        set<int> canvas;
        for (int i = minDay; i < maxDay; ++i)
        {
            canvas.insert(i);
        }

        vector<int> result;
        for (const auto& task : paint)
        {
            int paintedCount = 0;
            // lower_bound() return the first element >= task[0]. We start to paint from
            // the position in this day.
            auto iter = canvas.lower_bound(task[0]);
            while (iter != canvas.end() && *iter < task[1])
            {
                paintedCount++;
                // erase() will return the iterator which follows the deleted item, or end().
                iter = canvas.erase(iter);
            }
            result.push_back(paintedCount);
        }

        return result;
    }
};

//---------------------------------------------------------------------------------------
// 833. Find And Replace in String
//
// The index will change if we replace the string from the beginning.
// So, we should replace from the end.
//---------------------------------------------------------------------------------------
string findReplaceString(string s, vector<int>& indices, vector<string>& sources, vector<string>& targets)
{
    // index ->  <source string's length, target string>
    // Use greater<int> because we want to replace from the end of the string.
    map<int, pair<size_t, string>, greater<int>> replacements;

    for (int i = 0; i < indices.size(); ++i)
    {
        if (s.compare(indices[i], sources[i].length(), sources[i]) == 0)
        {
            replacements.emplace(indices[i], pair<size_t, string>{ sources[i].length(), targets[i] });
        }
    }

    for (const auto& replacement : replacements)
    {
        s.replace(replacement.first, replacement.second.first, replacement.second.second);
    }

    return s;
}

//---------------------------------------------------------------------------------------
// 68. Text Justification (Hard)
//
//---------------------------------------------------------------------------------------
class Solution68
{
public:
    vector<string> fullJustify(vector<string>& words, int maxWidth)
    {
        vector<string> result;
        m_maxWidth = maxWidth;
        const int count = words.size();
        for (int i = 0; i < count; )
        {
            vector<string> lineStringArray;
            lineStringArray.push_back(words[i]);
            int len = words[i].size();
            int j = i;
            while ( len <= maxWidth && i < words.size() - 1 && j < words.size() - 1)
            {
                j++;
                const int newLen = len + 1 + words[j].size();
                if (newLen > maxWidth)
                {
                    break;
                }
                else
                {
                    len = newLen;
                    lineStringArray.push_back(words[j]);
                }
            }

            i += lineStringArray.size();
            string newStr = makeLineString(lineStringArray, i == count);
            result.push_back(newStr);
        }

        return result;
    }

    string makeLineString(const vector<string>& lineStringArray, bool lastWord)
    {
        const int slotCount = lineStringArray.size() - 1;
        int totalChar = 0;
        for (const auto& str : lineStringArray)
        {
            totalChar += str.size();
        }

        if (slotCount == 0)
        {
            // This line has only one word. We need to append space to the end.
            lastWord = true;
        }

        const int spaceInSlot = lastWord ? 1 : ( m_maxWidth - totalChar ) / slotCount;
        int remainingSpace = lastWord ? 0 : ( m_maxWidth - totalChar ) % slotCount;

        string result;
        for (int i = 0; i < lineStringArray.size(); ++i)
        {
            result += lineStringArray[i];

            if (i != lineStringArray.size() - 1)
            {
                int spaceCount = spaceInSlot;
                if (remainingSpace > 0)
                {
                    spaceCount += 1;
                    remainingSpace--;
                }
                while (spaceCount > 0)
                {
                    result.push_back(' ');
                    spaceCount--;
                }
            }
            else if (lastWord)
            {
                int spaceCount = m_maxWidth - totalChar - slotCount;
                while (spaceCount > 0)
                {
                    result.push_back(' ');
                    spaceCount--;
                }
            }
        }

        return result;
    }

private:
    int m_maxWidth = 0;
};


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

    // 777. Swap Adjacent in LR String
    // Input: start = "RXXLRXRXL", end = "XRLXXRRLX"
    // Output: true
    // "RLX" and "XLR"
    // Expect: false.
    // "XXRXL" and "LXRXX"
    // Expect: false
    Solution777 sol777;
    cout << "\n777. Swap Adjacent in LR String: " << sol777.canTransform("RLX", "XLR") << endl;

    // 2158. Amount of New Area Painted Each Day (Hard)
    Solution2158 sol2158;
    // Input: paint = [[1,4],[5,8],[4,7]]
    // Output: [3, 3, 1]
    // Input: paint = [[6,17],[3,6],[7,17],[16,20],[2,20]]
    // Output: [11,3,0,3,1]
    vector<vector<int>> inputVVI;
    LeetCodeUtil::BuildIntMatrixFromString("[[6,17],[3,6],[7,17],[16,20],[2,20]]", &inputVVI);
    auto resultVI = sol2158.amountPainted(inputVVI);
    cout << "\n2158. Amount of New Area Painted Each Day (Hard): " << endl;
    LeetCodeUtil::PrintVector(resultVI);

    // 833. Find And Replace in String
    // Input: s = "abcd", indices = [0, 2], sources = ["ab", "ec"], targets = ["eee", "ffff"]
    // Output : "eeecd"
    inputStr = "abcd";
    intV = { 0, 2 };
    vector<string> sources{ "ab", "ec" };
    vector<string> targets{ "eee", "ffff" };
    string resultS = findReplaceString(inputStr, intV, sources, targets);

    // 68. Text Justification
    // Input: words = ["This", "is", "an", "example", "of", "text", "justification."], maxWidth = 16
    sources = { "What","must","be","acknowledgment","shall","be" };
    Solution68 sol68;
    auto resultVS = sol68.fullJustify(sources, 16);
    cout << "\n68. Text Justification (Hard): " << endl;
    LeetCodeUtil::PrintVector(resultVS);

}
