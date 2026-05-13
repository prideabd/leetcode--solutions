/*
 * @lc app=leetcode.cn id=633 lang=cpp
 *
 * [633] 平方数之和
 *
 * https://leetcode.cn/problems/sum-of-square-numbers/description/
 *
 * algorithms
 * Medium (38.58%)
 * Likes:    518
 * Dislikes: 0
 * Total Accepted:    182.1K
 * Total Submissions: 472K
 * Testcase Example:  '5'
 *
 * 给定一个非负整数 c ，你要判断是否存在两个整数 a 和 b，使得 a^2 + b^2 = c 。
 * 
 * 
 * 
 * 示例 1：
 * 
 * 
 * 输入：c = 5
 * 输出：true
 * 解释：1 * 1 + 2 * 2 = 5
 * 
 * 
 * 示例 2：
 * 
 * 
 * 输入：c = 3
 * 输出：false
 * 
 * 
 * 
 * 
 * 提示：
 * 
 * 
 * 0 <= c <= 2^31 - 1
 * 
 * 
 */

// @lc code=start
class Solution {
public:
    bool judgeSquareSum(int c) {
        int left = 0, right = sqrt(c);
        while (left <= right) {
            if (left * left == c - right * right) {
                return true;
            }
            if (left * left < c - right * right) {
                left++;
            } else {
                right--;
            }
        }
        return false;
    }
};
// @lc code=end

