/*
 * @lc app=leetcode.cn id=611 lang=cpp
 *
 * [611] 有效三角形的个数
 *
 * https://leetcode.cn/problems/valid-triangle-number/description/
 *
 * algorithms
 * Medium (55.50%)
 * Likes:    715
 * Dislikes: 0
 * Total Accepted:    165.7K
 * Total Submissions: 298.5K
 * Testcase Example:  '[2,2,3,4]'
 *
 * 给定一个包含非负整数的数组 nums ，返回其中可以组成三角形三条边的三元组个数。
 * 
 * 
 * 
 * 示例 1:
 * 
 * 
 * 输入: nums = [2,2,3,4]
 * 输出: 3
 * 解释:有效的组合是: 
 * 2,3,4 (使用第一个 2)
 * 2,3,4 (使用第二个 2)
 * 2,2,3
 * 
 * 
 * 示例 2:
 * 
 * 
 * 输入: nums = [4,2,3,4]
 * 输出: 4
 * 
 * 
 * 
 * 提示:
 * 
 * 
 * 1 <= nums.length <= 1000
 * 0 <= nums[i] <= 1000
 * 
 * 
 */

// @lc code=start
class Solution {
public:
    int triangleNumber(vector<int>& nums) {
        ranges::sort(nums);
        int res = 0, n = nums.size();
        for (int i = 0; i < n - 2; ++i) {
            if (nums[i] == 0) {
                continue;
            }
            int j = i + 1;
            for (int k = j + 1; k < n; ++k) {
                while (nums[i] + nums[j] <= nums[k]) {
                    // 两边之和 小于 第三边
                    // 增大j边
                    j++;
                }
                res += k - j;
            }
        }
        return res;
    }
};
// @lc code=end

