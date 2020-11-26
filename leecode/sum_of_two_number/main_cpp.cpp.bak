// main.cpp - main

// Author : liyunteng <liyunteng@streamocean.com>
// Date   : 2019/09/16

// Copyright (C) 2019 StreamOcean, Inc.
// All rights reserved.
#include <vector>
#include <map>
#include <iostream>

using namespace std;

class Solution {
public:
#if 0
    vector<int> twoSum(vector<int>& nums, int target) {
        vector<int>::size_type a, b;
        for (a = 0; a < nums.size(); a++) {
            for (b = a+1; b < nums.size(); b++) {
                if (nums[a] + nums[b] == target) {
                    return {static_cast<int>(a), static_cast<int>(b)};
                }
            }
        }
        return {-1, -1};
    }
#endif

    vector<int> twoSum(vector<int>& nums, int target) {
        map<int, int>m;
        for (int i = 0; i < nums.size(); ++i) {
            if (m.count(target - nums[i]) > 0) {
                return {m[target - nums[i]], i};
            }
            m[nums[i]] = i;
        }
        return {-1, -1};
    }
};

int main(void)
{
    vector<int>test;
    for (int i=0; i < 100; i++) {
        test.push_back(i);
    }
    Solution s;
    for (auto i : s.twoSum(test, 98+99)) {
        cout << i << " ";
    }
    cout << endl;
    return 0;
}
