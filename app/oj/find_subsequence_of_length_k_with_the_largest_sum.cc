#include <iterator>
#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;

class Solution {
public:
    vector<int> maxSubsequence(vector<int>& nums, int k) {
        vector<int> ans;
        vector<int> tmp(nums);
        sort(tmp.begin(), tmp.end(), less<int>());
        for (size_t i = 0; i < nums.size(); i++) {
        }
        return ans;
    }
};