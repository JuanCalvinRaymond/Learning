def houseRobber(nums: list[int]):
    rob1 = rob2 = 0

    for n in nums:
        rob1, rob2 =  rob2, max(rob1 + n, rob2)
    return rob2