class Solution:
    def twoSum(self, nums: List[int], target: int) -> List[int]:
        val_idx = {}
        for i, num in enumerate(nums):
            if target - num in val_idx:
                return [i, val_idx[target - num]]
            val_idx[num] = i