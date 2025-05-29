def removeElements(head, val):
    if not head:
        return None
    head.next = removeElements(head.next, val)
    return head if head.val != val else head.next
