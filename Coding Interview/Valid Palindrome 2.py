 Python program to check whether it is possible to make
# string palindrome by removing one character

# Utility method to check if substring from 
# low to high is palindrome or not.
def isPalindrome(string: str, low: int, high: int) -> bool:
    while low < high:
        if string[low] != string[high]:
            return False
        low += 1
        high -= 1
    return True

# This method returns -1 if it 
# is not possible to make string
# a palindrome. It returns -2 if 
# string is already a palindrome.
# Otherwise it returns index of
# character whose removal can
# make the whole string palindrome.
def possiblepalinByRemovingOneChar(string: str) -> int:

    # Initialize low and right by
    # both the ends of the string
    low = 0
    high = len(string) - 1

    # loop until low and high cross each other
    while low < high:

        # If both characters are equal then
        # move both pointer towards end
        if string[low] == string[high]:
            low += 1
            high -= 1
        else:

            # If removing str[low] makes the whole string palindrome.
            # We basically check if substring str[low+1..high] is
            # palindrome or not.
            if isPalindrome(string, low + 1, high):
                return low

            # If removing str[high] makes the whole string palindrome
            # We basically check if substring str[low+1..high] is
            # palindrome or not
            if isPalindrome(string, low, high - 1):
                return high
            return -1

    # We reach here when complete string will be palindrome
    # if complete string is palindrome then return mid character
    return -2

# Driver Code
if __name__ == "__main__":

    string = "abecbea"
    idx = possiblepalinByRemovingOneChar(string)

    if idx == -1:
        print("Not possible")
    else if idx == -2:
        print("Possible without removing any character")
    else:
        print("Possible by removing character at index", idx)