#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Solution
{
public:
    void Dfs(int n, int index, string &num, vector<string> &allStr)
    {
        if (num.size() == n)
        {
            allStr.emplace_back(num);
            return;
        }
        for (int i = index; i <= n; i++)
        {
            if (index == 1)
            {
                string num;
            }

            if (num.find(std::to_string(i)) != num.end())
            {
                continue;
            }
            Dfs(n, index++, num, allStr);
        }
    }

    string getPermutation(int n, int k)
    {
        vector<string> allStr;
        // Dfs();
    }
};