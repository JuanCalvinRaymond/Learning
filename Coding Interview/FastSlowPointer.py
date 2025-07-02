def fast_slowPointer(head: Node):
    slow, fast = head, head
    while fast and fast.next:
        fast = fast.next.next
        slow = slow.next
        slow = slow.next
    return slow
