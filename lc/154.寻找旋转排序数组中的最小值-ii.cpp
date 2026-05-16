/*
 * @lc app=leetcode.cn id=154 lang=cpp
 *
 * [154] 寻找旋转排序数组中的最小值 II
 *
 * https://leetcode.cn/problems/find-minimum-in-rotated-sorted-array-ii/description/
 *
 * algorithms
 * Hard (51.00%)
 * Likes:    757
 * Dislikes: 0
 * Total Accepted:    226.1K
 * Total Submissions: 442.7K
 * Testcase Example:  '[1,3,5]'
 *
 * 已知一个长度为 n 的数组，预先按照升序排列，经由 1 到 n 次 旋转 后，得到输入数组。例如，原数组 nums = [0,1,4,4,5,6,7]
 * 在变化后可能得到：
 * 
 * 若旋转 4 次，则可以得到 [4,5,6,7,0,1,4]
 * 若旋转 7 次，则可以得到 [0,1,4,4,5,6,7]
 * 
 * 
 * 注意，数组 [a[0], a[1], a[2], ..., a[n-1]] 旋转一次 的结果为数组 [a[n-1], a[0], a[1], a[2],
 * ..., a[n-2]] 。
 * 
 * 给你一个可能存在 重复 元素值的数组 nums ，它原来是一个升序排列的数组，并按上述情形进行了多次旋转。请你找出并返回数组中的 最小元素 。
 * 
 * 你必须尽可能减少整个过程的操作步骤。
 * 
 * 
 * 
 * 示例 1：
 * 
 * 
 * 输入：nums = [1,3,5]
 * 输出：1
 * 
 * 
 * 示例 2：
 * 
 * 
 * 输入：nums = [2,2,2,0,1]
 * 输出：0
 * 
 * 
 * 
 * 
 * 提示：
 * 
 * 
 * n == nums.length
 * 1 <= n <= 5000
 * -5000 <= nums[i] <= 5000
 * nums 原来是一个升序排序的数组，并进行了 1 至 n 次旋转
 * 
 * 
 * 
 * 
 * 进阶：这道题与 寻找旋转排序数组中的最小值 类似，但 nums 可能包含重复元素。允许重复会影响算法的时间复杂度吗？会如何影响，为什么？
 * 
 */

// @lc code=start
class Solution {
public:
    int findMin(vector<int>& nums) {
        // mid > right，还能保证min在[mid, right]吗？
        // left mid min right. -> min right left mid. 可能情况是mid >= right
        // left min mid right. -> min mid right left. mid <= right
        // 问题就来了，mid == right时，左右都有可能出现
        // 对于相等，直接去除最后一位，继续二分(相当于去除掉一个不可能为最小的数)
        // 假设min在右边，去掉right，如果right==min,那么mid==min，全体都是一个数
        // 假设在左边，去掉right, 也一定有mid兜底
        int n = nums.size();
        int left = 0, right = n - 1;
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] == nums[right]) {
                right--;
            } else if (nums[mid] > nums[right]) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        return nums[right];
    }
};
// @lc code=end

