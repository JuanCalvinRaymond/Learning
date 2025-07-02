class Solution:
    def longestCommonPrefix(self, strs: List[str]) -> str:
        result = strs[0]
        for i in range(1, len(strs)):
            while(not strs[i].startswith(result)):
                result = result[0 : len(result) - 1]
                if not result:
                    return ""
        return result