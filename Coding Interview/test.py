def fun(arr: list[int]) -> list[int]:

    import heapq

    heapq.heapify(arr)

    res = []

    for i in range(3):

        res.append(heapq.heappop(arr))

    return res

def f(arr1, arr2):

    i, j = 0, 0

    new_arr = []

    while i < len(arr1) and j < len(arr2):

        if arr1[i] < arr2[j]:

            new_arr.append(arr1[i])

            i += 1

        else:

            new_arr.append(arr2[j])

            j += 1
    
    print("before: ", new_arr)
    new_arr.extend(arr1[i:])
    print("after 1: ", new_arr)
    new_arr.extend(arr2[j:])
    print("after 2: ", new_arr)
    return new_arr


print(f([30,20,10,100,33,12], [50,40,34,25,1,5]))