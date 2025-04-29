
# O(n) solution
string1 = "nameless"
string2 = "Someting"

def are_anagrams(s1, s2):
    if len(s1) != len(s2):
        return False
    freq1 = {}
    freq2 = {}

    for ch in s1:
        if ch in freq1:
            freq1[ch] += 1
        else:
            freq1[ch] = 1
    for ch in s2:
        if ch in freq2:
            freq2[ch] += 1
        else:
            freq2[ch] = 1
    for key in freq1:
        if key not in freq2 or freq1[key] != freq2[key]:
            return False
    return True
    
# O(nlogn) solution
def are_anagrams(s1, s2):
    if len(s1) != len(s2):
        return False
    return sorted(s1) == sorted(s2)