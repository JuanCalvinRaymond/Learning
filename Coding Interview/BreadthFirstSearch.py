num_row, num_col = len(grid), len(grid[0])
def get_neighbour(startingPoint: list[int, int]) -> list[int, int]:
    row, col  = startingPoint
    res = []
    row_movement = [-1, 0, 1, 0]
    col_movement = [0, 1, 0, -1]
    for i in range(len(row_movement)):
        neighbor_row = row + row_movement[i]
        neighbor_col = col + col_movement[i]
        if 0 <= neighbor_row < num_row and 0 <= neighbor_col < num_col:
            res.append((neighbor_row, neighbor_col))
    return res

from collections import deque

def bfs(startingCoord):
    queue = deque(startingCoord)
    visited = list(startingCoord)
    while len(queue):
        node = queue.popleft()
        for neighbor in get_neighbour(node):
            if neighbor in visited:
                continue
            queue.append(neighbor)
            visited.append(neighbor)

