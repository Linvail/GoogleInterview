// Design.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <set>
#include <random>
#include <numeric> // accumulate

using namespace std;

//---------------------------------------------------------------------------------------
// 2034. Stock Price Fluctuation
//
// The crucial part how to update min/max when correction occurs.
//---------------------------------------------------------------------------------------
class StockPrice
{
public:
    StockPrice()
    {
    }

    void update(int timestamp, int price)
    {
        if (m_priceMap.find(timestamp) != m_priceMap.end())
        {
            // Correction occurs.
            auto iter = m_prices.find(m_priceMap[timestamp]);
            if (iter != m_prices.end())
            {
                m_prices.erase(iter);
            }
        }

        m_priceMap[timestamp] = price;
        m_prices.insert(price);
    }

    int current()
    {
        return m_priceMap.rbegin()->second;
    }

    int maximum()
    {
        return *m_prices.rbegin();
    }

    int minimum()
    {
        return *m_prices.begin();
    }

private:
    map<int, int> m_priceMap;
    multiset<int> m_prices;
};

//---------------------------------------------------------------------------------------
// 1146. Snapshot Array
//
// I can devise two approaches:
// a. Maintain a vector of elements and a map for recording changes between snapshots.
// b. Maintain a map of map. Let every element has its own changing records between
//    snapshots.
//---------------------------------------------------------------------------------------
class SnapshotArray
{
public:
    SnapshotArray(int length)
    {
        for (int i = 0; i < length; ++i)
        {
            // Initial the inner map with 1 pair; {0, 0}.
            m_records[i] = { { 0, 0 } };
        }
    }

    void set(int index, int val)
    {
        m_records[index][m_snapId] = val;
    }

    int snap()
    {
        return m_snapId++;
    }

    int get(int index, int snap_id)
    {
        auto validSnapIdIter = m_records[index].upper_bound(snap_id);
        validSnapIdIter--;

        return validSnapIdIter->second;
    }

    int m_snapId = 0;
    // <index of record> -> <snap Id, value>
    map<int, map<int, int>> m_records;
};

//---------------------------------------------------------------------------------------
// 528. Random Pick with Weight
// Topic: Math
//
// Need to use random number.
//---------------------------------------------------------------------------------------
class Solution528
{
public:
    Solution528(vector<int>& w)
        : m_generator(random_device()())
    {
        m_SumOfWeight = std::accumulate(w.begin(), w.end(), 0);

        int prev = w[0];
        m_PrefixSum[prev] = 0;
        for (int i = 1; i < w.size(); ++i)
        {
            int newSum = prev + w[i];
            m_PrefixSum[newSum] = i;
            prev = newSum;
        }
    }

    int pickIndex()
    {
        uniform_int_distribution<int> dist(1, m_SumOfWeight);
        int dice = dist(m_generator);

        // Find the first element >= dice.
        auto iter = m_PrefixSum.lower_bound(dice);
        return iter->second;
    }

private:
    // <prefix sum, index>
    map<int, int> m_PrefixSum;
    int m_SumOfWeight = 0;
    // Caution! The construction cost a lot of time. Don't put this in pickIndex().
    mt19937 m_generator;
};

//---------------------------------------------------------------------------------------
// 715. Range Module
//
// This questions requires good understanding of ranges/intervals.
// left <= x < right.
//---------------------------------------------------------------------------------------
// Basic data structure: map<start of interval, end of interval>.
// So, the internals are sorted by its start time, and we can quickly find its end time.
class RangeModule
{
public:
    RangeModule()
    {
    }

    void addRange(int left, int right)
    {
        auto newRange = insertRange(left, right);
        m_ranges[newRange.first] = newRange.second;
    }

    bool queryRange(int left, int right)
    {
        // [............)
        //   ^         $
        //  left      right
        auto iter = m_ranges.upper_bound(left);
        if (iter == m_ranges.begin())
        {
            return false;
        }
        iter--;

        return iter->second >= right;
    }

    void removeRange(int left, int right)
    {
        // Removing a range might generates two smaller ranges.
        auto x = insertRange(left, right);
        //   x.left                        x.right
        //   [.....)              [........)
        //      ^                     $
        //     left                  right

        if (x.first < left)
        {
            // Create a small range on the left.
            m_ranges[x.first] = left;
        }

        if (x.second > right)
        {
            // Create a small range on the right.
            m_ranges[right] = x.second;
        }

    }

private:

    // Insert a range, merging overlapped areas.
    // Return the new range that covers <left, right>.
    pair<int, int> insertRange(int left, int right)
    {
        // Try to find the range whose start is not greater than (<=) 'left'.
        // There is no function can directly give us that, so we use upper_bound() and
        // move left if necessary.
        //       [.....)       [.....)
        //       ^             iter
        //      left
        //      iter--
        auto iterLeft = m_ranges.upper_bound(left);
        if (iterLeft != m_ranges.begin()) // Cannot move left if it's already beginning.
        {
            iterLeft--;
            if (iterLeft->second < left)
            {
                // This range is too small, falling in the left side of the target range and do
                // not intersect. So, we move right back.
                iterLeft++;
            }
        }
        // Now the iterLeft is the range whose left is not greater than 'left'.
        // iterLeft might be end().
        auto iterRight = m_ranges.upper_bound(right);
        if (iterLeft == iterRight)  // Note that they might be both end().
        {
            //                          [....)
            //           ^          $
            //          left       right
            // [end)
            return { left, right };
        }

        //       iterLeft           iterRight
        //       [.........)        [.........)
        //                      [..)
        //   ^   ^        $     $
        //      left     right
        int i = min(left, iterLeft->first);
        iterRight--;
        int j = max(right, iterRight->second);
        iterRight++;
        // Erase range from iterLeft to iterRight - 1.
        m_ranges.erase(iterLeft, iterRight);

        return { i, j };
    }

    map<int, int> m_ranges;
};

//---------------------------------------------------------------------------------------
// 642. Design Search Autocomplete System
//---------------------------------------------------------------------------------------
class AutocompleteSystem
{
public:
    AutocompleteSystem(vector<string>& sentences, vector<int>& times)
    {
        for (size_t i = 0; i < sentences.size(); ++i)
        {
            m_sentences[sentences[i]] = times[i];
        }
    }

    vector<string> input(char c)
    {
        if (c == '#')
        {
            // Ending sentence.
            m_sentences[m_input]++;
            m_input.clear();
            return {};
        }

        m_input.push_back(c);

        vector<pair<string, size_t>> matched;
        for (const auto& setPair : m_sentences)
        {
            if (0 == setPair.first.compare(0, m_input.size(), m_input))
            {
                matched.push_back(setPair);
            }
        }

        sort(matched.begin(), matched.end(), comp);

        vector<string> result;
        for (int i = 0; i < 3 && i < matched.size(); ++i)
        {
            result.push_back(matched[i].first);
        }

        return result;
    }

private:

    static bool comp(const pair<string, size_t>& left, const pair<string, size_t>& right)
    {
        if (left.second == right.second)
        {
            return left.first < right.first;
        }
        else
        {
            return left.second > right.second;
        }
    }

    map<string, size_t> m_sentences;
    string m_input;
};


int main()
{
    std::cout << "Design!\n";

    // 2034. Stock Price Fluctuation
    StockPrice stockPrice;;
    stockPrice.update(1, 10); // Timestamps are [1] with corresponding prices [10].
    stockPrice.update(2, 5);  // Timestamps are [1,2] with corresponding prices [10,5].
    int result = stockPrice.current();     // return 5, the latest timestamp is 2 with the price being 5.
    result = stockPrice.maximum();     // return 10, the maximum price is 10 at timestamp 1.
    stockPrice.update(1, 3);  // The previous timestamp 1 had the wrong price, so it is updated to 3.
                              // Timestamps are [1,2] with corresponding prices [3,5].
    result = stockPrice.maximum();     // return 5, the maximum price is 5 after the correction.
    stockPrice.update(4, 2);  // Timestamps are [1,2,4] with corresponding prices [3,5,2].
    result =stockPrice.minimum();     // return 2, the minimum price is 2 at timestamp 4.

    // 1146. Snapshot Array
    // Input: ["SnapshotArray", "set", "snap", "set", "get"]
    // [[3], [0, 5], [], [0, 6], [0, 0]]
    // Output : [null, null, 0, null, 5]
    SnapshotArray snapshotArr(3); // set the length to be 3
    snapshotArr.set(0, 5);  // Set array[0] = 5
    int snapId = snapshotArr.snap();  // Take a snapshot, return snap_id = 0
    snapshotArr.set(0, 6);
    int value = snapshotArr.get(0, 0);  // Get the value of array[0] with snap_id = 0, return 5

    // 528. Random Pick with Weight
    vector<int> w{ 1, 2, 3, 4 };
    Solution528 sol528(w);
    map<int, int> stats;
    for (int i = 0; i < 10000; ++i)
    {
        int idx = sol528.pickIndex();
        stats[idx]++;
    }
    for (const auto& row : stats)
    {
        cout << "Count of index: " << row.first << " is " << row.second << endl;
    }

    RangeModule rangeModule;
    rangeModule.addRange(10, 20);
    rangeModule.removeRange(14, 16);
    bool resultB = rangeModule.queryRange(10, 14); // return True,(Every number in [10, 14) is being tracked)
    resultB = rangeModule.queryRange(13, 15); // return False,(Numbers like 14, 14.03, 14.17 in [13, 15) are not being tracked)
    resultB = rangeModule.queryRange(16, 17); // return True, (The number 16 in [16, 17) is still being tracked, despite the remove operation)

    // 642. Design Search Autocomplete System
    vector<string> sentencs{ "i love you", "island", "iroman", "i love leetcode" };
    vector<int> sentFreq{ 5, 3, 2, 2 };
    AutocompleteSystem obj(sentencs, sentFreq);
    obj.input('i'); // return ["i love you", "island", "i love leetcode"]. There are four sentences that have prefix "i". Among them, "ironman" and "i love leetcode" have same hot degree. Since ' ' has ASCII code 32 and 'r' has ASCII code 114, "i love leetcode" should be in front of "ironman". Also we only need to output top 3 hot sentences, so "ironman" will be ignored.
    obj.input(' '); // return ["i love you", "i love leetcode"]. There are only two sentences that have prefix "i ".
    obj.input('a'); // return []. There are no sentences that have prefix "i a".
    obj.input('#'); // return []. The user finished the input, the sentence "i a" should be saved as a historical sentence in system. And the following input will be counted as a new search.

}
