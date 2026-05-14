/*
 * @lc app=leetcode.cn id=2563 lang=cpp
 *
 * [2563] 统计公平数对的数目
 *
 * https://leetcode.cn/problems/count-the-number-of-fair-pairs/description/
 *
 * algorithms
 * Medium (48.76%)
 * Likes:    184
 * Dislikes: 0
 * Total Accepted:    52.4K
 * Total Submissions: 107.5K
 * Testcase Example:  '[0,1,7,4,4,5]\n3\n6'
 *
 * 给你一个下标从 0 开始、长度为 n 的整数数组 nums ，和两个整数 lower 和 upper ，返回 公平数对的数目 。
 * 
 * 如果 (i, j) 数对满足以下情况，则认为它是一个 公平数对 ：
 * 
 * 
 * 0 <= i < j < n，且
 * lower <= nums[i] + nums[j] <= upper
 * 
 * 
 * 
 * 
 * 示例 1：
 * 
 * 
 * 输入：nums = [0,1,7,4,4,5], lower = 3, upper = 6
 * 输出：6
 * 解释：共计 6 个公平数对：(0,3)、(0,4)、(0,5)、(1,3)、(1,4) 和 (1,5) 。
 * 
 * 
 * 示例 2：
 * 
 * 
 * 输入：nums = [1,7,9,2,5], lower = 11, upper = 11
 * 输出：1
 * 解释：只有单个公平数对：(2,3) 。
 * 
 * 
 * 
 * 
 * 提示：
 * 
 * 
 * 1 <= nums.length <= 10^5
 * nums.length == n
 * -10^9 <= nums[i] <= 10^9
 * -10^9 <= lower <= upper <= 10^9
 * 
 * 
 */

// @lc code=start
// 1. 双指针
class Solution {
public:
    long long countFairPairs(vector<int>& nums, int lower, int upper) {
        // lower - nums[j] <= nums[i] <= upper - nums[j]
        // 假设遍历i，，当增大时，j需要同步变小
        int n = nums.size();
        long long res = 0;
        ranges::sort(nums);
        int l = n, r = n;
        for (int i = 0; i < n - 1; ++i) {
            // 保持在i右边的问题？
            // 假设 nums = [10, 20, 30], upper = 5。
            // 当你处理 i = 0 (nums[i] = 10) 时：
            // 没有任何 j 能满足 10 + nums[j] <= 5。
            // 如果你写 while (r > i && ...)，r 会停在 1。
            // 如果你写 while (r > 0 && ...)，r 会停在 0。
            while (r > 0 && nums[r - 1] + nums[i] > upper) {
                r--;
            }
            while (l > 0 && nums[l - 1] + nums[i] >= lower) {
                l--;
            }
            // 这里max(r, i + 1)是因为有个隐藏条件，i < j
            // 所以找出的j在[l, r]里，还有一个隐藏就是在[i + 1, n - 1]
            // 然后求这两个交集
            res += max(r, i + 1) - max(l, i + 1);
        }
        return res;
    }
};

// 2.库函数 + 二分
class Solution {
public:
    long long countFairPairs(vector<int>& nums, int lower, int upper) {
        // 库函数写法
        ranges::sort(nums);
        long long res = 0;
        for (int j = 0; j < nums.size(); ++j) {
            auto r = upper_bound(nums.begin(), nums.begin() + j, upper - nums[j]);
            auto l = lower_bound(nums.begin(), nums.begin() + j, lower - nums[j]);
            res += r - l;
        }
        return res;
    }
};
// @lc code=end

