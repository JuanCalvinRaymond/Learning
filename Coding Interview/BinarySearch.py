class BinarySearch:
    def Search(self, arr: list[int], target: int) -> int:
        l, r = 0, len(arr) - 1
        while l <= r:
            mid = r - l // 2
            if arr[mid] == target:
                return mid
            if arr[mid] < target:
                l = mid + 1
            if arr[mid] > target:
                r = mid - 1
        return - 1