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
}
