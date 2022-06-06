// Math.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <algorithm> // std::reverse
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <string>
#include <set>
#include <queue>
#include <deque>
#include <random>

#include "LeetCodeUtil.h"

using namespace std;

//---------------------------------------------------------------------------------------
// 43. Multiply Strings
//
// Note: You must not use any built-in BigInteger library or convert the inputs to
// integer directly.
//---------------------------------------------------------------------------------------

string multiply(string num1, string num2)
{
    // This question is about math.
    //     8 9  <- num2
    //     7 6 < -num1
    // --------
    //     5 4
    //   4 8   <- shift 1
    //   6 3   <- shift 1
    // 5 6     <- shift 2
    // --------
    // 0 1 2 3
    // Note: Assume num1 has m digit and num2 has n digit.
    // The product at most has m + n digit.
    // The difficult is how to determine to shift.
    // num1[0~i] num2[0~j]
    // By observation, we can know the shift of the product of two digit is
    // i + j.
    const int m = num1.size();
    const int n = num2.size();
    // All digits are initialized to 0, so we must eliminate the leading zeros.
    vector<int> productString(m + n, 0);

    for (int i = m - 1; i >= 0; --i)
    {
        for (int j = n - 1; j >= 0; --j)
        {
            const int smallDigit = i + j + 1;
            const int bigDigit = i + j;

            int partialSum = ( ( num1[i] - '0' ) * ( num2[j] - '0' ) ) + productString[smallDigit];

            productString[i + j + 1] = partialSum % 10;
            productString[i + j] += partialSum / 10;
        }
    }

    string result;
    for (int i : productString)
    {
        if( i != 0 || !result.empty())
        result.push_back( i + '0' );
    }

    return result.empty() ? "0" : result;
}

//---------------------------------------------------------------------------------------
// 159. Longest Substring with At Most Two Distinct Characters
// Topic: Sliding window. hash table.
//
//---------------------------------------------------------------------------------------
int lengthOfLongestSubstringTwoDistinct(string s)
{
    // Idea:: Scan from the left to right. Alway keep the maximum length we ever have.
    // Use hash table to record met char's index. When meet the 3rd char, we keep moving
    // left boundary until all the 1st char are out of window.
    // Input: eceba

    int maxLen = 0;
    int left = 0;
    unordered_map<char, size_t> charPosMap;

    for (int i = 0; i < s.size(); ++i)
    {
        charPosMap[s[i]] = i;
        while (charPosMap.size() > 2)
        {
            // Now we need to erase s[i], but when to stop?
            // charPosMap recoded its latest index, so we found an element's last index
            // is on left, it means this position is its last appearance.
            if (charPosMap[s[left]] == left)
            {
                charPosMap.erase(s[left]);
            }
            left++;
        }

        maxLen = max(maxLen, i - left + 1);
    }

    return maxLen;
}

//---------------------------------------------------------------------------------------
// 681. Next Closest Time
//---------------------------------------------------------------------------------------
string nextClosestTime(string time)
{
    // Input: "19:34"
    // Output: "19:39"
    // Input: "23:59"
    // Output : "22:22"
    // Idea:
    // 1. We should collect usable numbers.
    // 2. Scan from right to left and try to replace the smaller one with a greater one.
    //    If we successfully swap one digit, it is done. We can return.
    // 3. Many digits have constraint of changing. For example, time[3] cannot be greater
    //    than 5.
    // 4. If we cannot find a eligible greater one, we should set it to lowest number and
    //    continue moving left.
    // 5. In the worst case, all digits will be the lowest digit.

    // Collect usable numbers. Use set to sort out duplicates.
    set<char> usableNumber{ time[0], time[1], time[3], time[4] };
    string usableChar(usableNumber.begin(), usableNumber.end());

    for (int i = time.size() - 1; i >= 0; --i)
    {
        const char number = time[i];
        if (number == ':')
        {
            continue;
        }

        int sortIdx = usableChar.find(number);
        // If this number is the maximum number
        if (sortIdx == usableChar.size() - 1)
        {
            // Set to lowest number.
            time[i] = usableChar[0];
        }
        else
        {
            char nextGreater = usableChar[sortIdx + 1];
            // Now handle every cases of different digits.
            if (i == 4) // low minute digit
            {
                time[i] = nextGreater;
                return time;
            }
            else if (i == 3 && nextGreater <= '5') // high minute digit
            {
                time[i] = nextGreater;
                return time;
            }
            else if (i == 1 && ( ( time[0] == '2' && nextGreater <= '3' ) || time[0] != '2' )) // low hour digit
            {
                time[i] = nextGreater;
                return time;
            }
            else if (i == 0 && nextGreater <= '2')
            {
                time[i] = nextGreater;
                return time;
            }
            else
            {
                // All the cases above are made to handle the case when nextGreater is eligible to replace the
                // current digit. If none of them is met, we should set it to lowest number.
                time[i] = usableChar[0];
            }
        }
    }

    return time;
}

//---------------------------------------------------------------------------------------
// 809. Expressive Words
//---------------------------------------------------------------------------------------
int expressiveWords(string s, vector<string>& words)
{
    // This is difficult practice of two pointers, which are not used in the same array.
    // Idea:
    // 1. s must be longer than any word.
    // 2. For each word, iterate char in s. Two loops, outer is for words, inner is for
    //    s.
    // 3. The crucial part is how to determine whether a word is stretchy. When we scan
    //    the characters, if we found mismatch and there are less than 3 identical
    //    characters in 's', then we are done. This word is not stretchy.

    const int sLen = s.size();
    int result = 0;
    for (const auto& word : words)
    {
        const int wordLen = word.size();
        int j = 0; // index for word.
        int i = 0;
        for (; i < sLen; ++i)
        {
            if (j < wordLen && s[i] == word[j])
            {
                j++;
            }
            else if (i > 0 && j > 0 && word[j - 1] == s[i] &&
                s[i] == s[i - 1] && i + 1 < sLen && s[i] == s[i + 1])
            {
                // e e e e l l...
                // e l l o....
                //   ^   #
                // or
                // e e e
                // e
                //   ^   #
                // Increment one more time. Including one time in loop. We make it +2,
                // so that it skip the 3rd 'e'.
                i++;
            }
            else if( !(i > 1 && s[i] == s[i - 1] && s[i] == s[i - 2]))
            {
                // e e e o
                //       ^
                // e l l
                //   ^
                // or
                // h e l l o
                //       ^
                // h e l o
                //       ^
                break;
            }
            else
            {
                // e e e e l l
                //       ^
                // e l l
                //   ^
                // Intentionally left empty.
                // We just let i++.
            }
        }

        if (i == sLen && j == wordLen)
        {
            result++;
        }
    }
    return result;
}

//---------------------------------------------------------------------------------------
// 539. Minimum Time Difference
//---------------------------------------------------------------------------------------
int findMinDifference(vector<string>& timePoints)
{
    if (timePoints.size() < 2)
    {
        return 0;
    }

    set<int> inMinutes;

    for (const auto& time : timePoints)
    {
        const int hour = stoi(time.substr(0, 2));
        const int minute = stoi(time.substr(3, 2));
        const int total = hour * 60 + minute;
        if (inMinutes.count(total))
        {
            return 0;
        }

        inMinutes.insert(total);
    }

    auto iter = inMinutes.begin();
    int prev = *iter;
    iter++;
    int result = INT_MAX;

    for (; iter != inMinutes.end(); ++iter)
    {
        int diff = min( *iter - prev, prev + (1440 - *iter) );
        result = min(result, diff);
        prev = *iter;
    }

    // Deal with the head and tail.
    prev = *inMinutes.begin();
    int diff = min(*inMinutes.rbegin() - prev, prev + ( 1440 - *inMinutes.rbegin() ));
    result = min(result, diff);

    return result;
}

//---------------------------------------------------------------------------------------
// 1610. Maximum Number of Visible Points
// Topic: Math
//
// Use atan2(y,x) to transform the 2d point into polar angle.
// If you don't know atan2, you almost have no chance to solve this.
//---------------------------------------------------------------------------------------

const double PI = 3.1415926;

int visiblePoints(vector<vector<int>>& points, int angle, vector<int>& location)
{
    // Idea
    // 1. Use atan2(y,x) to transform the 2d point into polar angle.
    // 2. Put polar angles into an array, sort it. This will become a sliding window
    //    question.
    int seenPoints = 0;
    vector<double> polarAngles;

    for (int i = 0; i < points.size(); ++i)
    {
        if (points[i][0] == location[0] && points[i][1] == location[1])
        {
            // The point is just on the view's location.
            seenPoints++;
            continue;
        }

        const int y = points[i][1] - location[1];
        const int x = points[i][0] - location[0];
        double polarAngle = atan2(y, x);

        // Perform insertion sort.
        //auto iter = std::lower_bound(polarAngles.begin(), polarAngles.end(), polarAngle);
        //polarAngles.insert(iter, polarAngle);
        polarAngles.push_back(polarAngle);
    }
    sort(polarAngles.begin(), polarAngles.end());

    // The direction of view many rotate nearly 360 degree. In 2nd plane, you
    // are supposed to see the points at the beginning of the array, but you won't see
    // them in the tail of the array.
    // To overcome this, we can add (angle +  2 PI) for every angles into the array.
    const size_t pointCount = polarAngles.size();
    for (int i = 0; i < pointCount; ++i)
    {
        const double dupAngle = polarAngles[i] + 2 * PI;
        polarAngles.emplace_back(dupAngle);
    }

    const double viewAngleRad = angle * PI / 180.0;
    // Assume your view's boundary just cover the i point, calculate how many points you
    // can see in this situation. Iterate over every i points.
    int maxCovered = 0;
    int covered = 0;
    for (int i = 0; i < pointCount; ++i)
    {
        while (polarAngles[i] + viewAngleRad >= polarAngles[covered])
        {
            covered++;
        }

        maxCovered = max(maxCovered, covered - i);
    }

    return seenPoints + maxCovered;
}

//---------------------------------------------------------------------------------------
// 2178. Maximum Split of Positive Even Integers
// Greedy
//---------------------------------------------------------------------------------------
class Solution2178
{
public:
    vector<long long> maximumEvenSplit(long long finalSum)
    {
        // 28:   2  4  6  8  10
        // psum: 2  6 12 20  30(X)
        // 28 - 20 = 8
        // Add 8 to 8 (the last element) = 16.
        // So answer is {2, 4, 6, 16}

        vector<long long> result;
        if (finalSum % 2 != 0)
        {
            return result;
        }

        long long item = 0;
        long long sum = 0;
        long long prevSum = 0;
        while (true)
        {
            item += 2;
            prevSum = sum;
            sum += item;
            if (sum > finalSum)
            {
                *result.rbegin() += finalSum - prevSum;
                break;
            }
            else
            {
                result.push_back(item);
            }
        }

        return result;
    }
};

//---------------------------------------------------------------------------------------
// 843. Guess the Word (Hard)
// Topic: Math, game theory
//---------------------------------------------------------------------------------------
class Solution843
{
public:

    class Master
    {
    public:
        int guess(string word)
        {
            guessCount++;
            int ret = 0;
            for (int i = 0; i < m_secret.size(); ++i)
            {
                if (word[i] == m_secret[i])
                {
                    ret++;
                }
            }

            if (ret == 6)
            {
                cout << "Matched!";
            }

            return ret;
        }

        Master()
            : m_secret("hbaczn")
        {
        }

        int guessCount = 0;
        string m_secret;
    };

    void findSecretWord(vector<string>& wordlist, Master& master)
    {
        int guessCount = 10;

        random_device rd;
        mt19937 generator(rd());
        int returned = 0;

        while (returned != 6 && guessCount > 0)
        {
            guessCount--;

            uniform_int_distribution<int> dist(0, wordlist.size() - 1);
            string candidate = wordlist[dist(generator)];

            returned = master.guess(candidate);

            if (returned == 6)
            {
                break;
            }

            vector<string> subSet;
            for(const auto& word : wordlist)
            {
                int match = calcMatchCount(candidate, word);
                if (match == returned)
                {
                    subSet.emplace_back(word);
                }
            }

            wordlist.swap(subSet);
        }
    }

    int calcMatchCount(const string& str1, const string& str2)
    {
        int ret = 0;
        for (int i = 0; i < str1.size(); ++i)
        {
            if (str1[i] == str2[i])
            {
                ret++;
            }
        }
        return ret;
    }
};

//---------------------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------------------
int main()
{
    std::cout << "Math!\n";

    // 43. Multiply Strings
    // Input: num1 = "123", num2 = "456"
    // Output: "56088"
    string num1 = "9";
    string num2 = "99";
    cout << "Result of Multiply Strings: " << multiply(num1, num2) << endl;

    // 159. Longest Substring with At Most Two Distinct Characters
    // Input: s = "eceba"
    // Output: 3
    string inputString = "eceba";
    cout << "Result of Longest Substring with At Most Two Distinct Characters: " << lengthOfLongestSubstringTwoDistinct(inputString) << endl;

    // 681. Next Closest Time
    // Input: time = "23:59"
    // Output: "22:22"
    inputString = "20:48";
    cout << "Result of Next Closest Time: " << nextClosestTime(inputString) << endl;

    // 809. Expressive Words
    // Input: s = "heeellooo", words = ["hello", "hi", "helo"]
    // Output: 1
    // Input: s = "zzzzzyyyyy", words = ["zzyy","zy","zyy"]
    // Output: 3
    inputString = "heeellooo";
    vector<string> words{ "hello", "hi", "helo" };

    cout << "Result of Expressive Words: " << expressiveWords(inputString, words) << endl;
    inputString = "zzzzzyyyyy";
    words = { "zzyy","zy","zyy" };
    cout << "Result of Expressive Words: " << expressiveWords(inputString, words) << endl;

    // 539. Minimum Time Difference
    // Input: timePoints = ["00:00","23:59","00:00"]
    // Output: 0
    vector<string> inputVS = {"03:00","00:00","23:00"};
    cout << "\n539. Minimum Time Difference: " << findMinDifference(inputVS) << endl;

    // 1610. Maximum Number of Visible Points
    // Input: points = [[2,1],[2,2],[3,3]], angle = 90, location = [1,1]
    // Output: 3
    // Input: points = [[2,1],[2,2],[3,4],[1,1]], angle = 90, location = [1,1]
    // Output: 4
    // Input: points = [[1,0],[2,1]], angle = 13, location = [1,1]
    // Output: 1
    // Input: points = [[1,1],[1,1],[1,1]], angle = 1, location = [1, 1]
    // Output: 3
    vector<vector<int>> points;
    LeetCodeUtil::BuildIntMatrixFromString("[[2,1],[2,2],[3,4],[1,1]]", &points);
    int angle = 90;
    vector<int> location = { 1,1 };
    cout << "\n1610. Maximum Number of Visible Points: " << visiblePoints(points, angle, location) << endl;

    // 2178. Maximum Split of Positive Even Integers
    long long finalSum = 28;
    Solution2178 sol2178;
    vector<long long> resultVL = sol2178.maximumEvenSplit(finalSum);
    cout << "\n2178. Maximum Split of Positive Even Integers: " << endl;
    LeetCodeUtil::PrintVector(resultVL);

    words = { "gaxckt", "trlccr", "jxwhkz", "ycbfps", "peayuf", "yiejjw", "ldzccp", "nqsjoa", "qrjasy", "pcldos", "acrtag", "buyeia", "ubmtpj", "drtclz", \
        "zqderp", "snywek", "caoztp", "ibpghw", "evtkhl", "bhpfla", "ymqhxk", "qkvipb", "tvmued", "rvbass", "axeasm", "qolsjg", "roswcb", "vdjgxx", "bugbyv", "zipjpc", \
        "tamszl", "osdifo", "dvxlxm", "iwmyfb", "wmnwhe", "hslnop", "nkrfwn", "puvgve", "rqsqpq", "jwoswl", "tittgf", "evqsqe", "aishiv", "pmwovj", "sorbte", "hbaczn", \
        "coifed", "hrctvp", "vkytbw", "dizcxz", "arabol", "uywurk", "ppywdo", "resfls", "tmoliy", "etriev", "oanvlx", "wcsnzy", "loufkw", "onnwcy", "novblw", "mtxgwe", \
        "rgrdbt", "ckolob", "kxnflb", "phonmg", "egcdab", "cykndr", "lkzobv", "ifwmwp", "jqmbib", "mypnvf", "lnrgnj", "clijwa", "kiioqr", "syzebr", "rqsmhg", "sczjmz", \
        "hsdjfp", "mjcgvm", "ajotcx", "olgnfv", "mjyjxj", "wzgbmg", "lpcnbj", "yjjlwn", "blrogv", "bdplzs", "oxblph", "twejel", "rupapy", "euwrrz", "apiqzu", "ydcroj", \
        "ldvzgq", "zailgu", "xgqpsr", "wxdyho", "alrplq", "brklfk" };

    Solution843 sol843;
    Solution843::Master master;
    sol843.findSecretWord(words, master);

}
