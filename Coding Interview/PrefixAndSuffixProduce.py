def build_prefix_sum(arr):
    n = len(arr)
    prefix_sum = [0] * n
    prefix_sum[0] = arr [0]
    for i in range(1, n):
        prefix_sum[i] = prefix_sum[i-1] + arr[i]

def query_range(prefix_sum, left, right):
    if left == 0:
        return prefix_sum[right]
    return prefix_sum[right] - prefix_sum[left - 1]

def prefix_sum_array(arr):
    prefix_sum = [0]
    for num in arr:
        prefix_sum.append(prefix_sum[-1] + num)
    return prefix_sum

arr = [1, -20, -3, 30, 5, 4]
print(prefix_sum_array(arr))