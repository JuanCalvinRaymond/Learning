def subarray_sum_fixed(nums: list[int], k: int) -> int:
    window_sum = 0
    for i in range(k):
        window_sum += nums[i]
    largest_sum = window_sum
    for right in range(k, len(nums)):
        left = right - k
        window_sum -= nums[left]
        window_sum += nums[right]
        largest_sum = max(largest_sum, window_sum)
            
    return largest_sum


def sliding_window_flexible_longest(input):
    initialize window, ans
    left = 0
    for right in range(len(input)):
        append input[right] to window
        while invalid(window):        # update left until window is valid again
            remove input[left] from window
            left += 1
        ans = max(ans, window)        # window is guaranteed to be valid here
    return ans