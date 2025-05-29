def isPalindrome(x: int) -> bool:
        if x < 0: 
            return False
        
        rev = 0 
        y = x

        while y: 
            rev = rev * 10 + y % 10 
            y = y // 10 
            print(rev, y)
        
        return rev == x 

isPalindrome(1243421)