# ans = []
# def dfs(start_index, path): #[]...additional state])
#     if is_leaf(start_index):
#         ans.append(path[:])
#         return
#     for edge in get_edges(start_index, [...additional states]):
#         #prune if needed
#         if not is_valid(edge):
#             continue
#         path.add(edge)
#         if additional_states:
#             update(...additional states)
#         dfs(start_index + len(edge), path, [...additional state])
#         #revert (... additional states) if necessary e.g. permutations
#         path.pop

# def dfs(start_index, [...additional state]):
#     if is_leaf(start_index):
#         return 1
#     ans = inital_value
#     for edge in get_edges(start_index, [...additional states]):
#         if additional states:
#             update([...additional states])
#         ans = aggregate(ans, dfs(start_index + len(edge)), [...additional states])
#         if additional states:
#             revert([additional states])
#     return ans