class Solution:
    def romanToInt(self, s: str) -> int:
        dict = {"I": 1, "V": 5, "X": 10, "L": 50, "C": 100, "D": 500, "M": 1000}
        result = 0
        
        for a, b in zip(s, s[1:]):
            if dict[a] < dict [b]:
                result -= dict[a]
            else:
                result += dict[a] 
            
        return result + dict[s[-1]]