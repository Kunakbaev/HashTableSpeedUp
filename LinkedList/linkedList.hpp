#ifndef LINKED_LIST_LINKED_LIST_HPP
#define LINKED_LIST_LINKED_LIST_HPP

#include "errorsHandlerLinkedList.hpp"

struct LinkedListNode {
    char*                       key;
    int                         value;
    LinkedListNode*             prev;
};

LinkedListErrors constructLinkedListNode(
    const char*                 key, 
    int                         value,
    LinkedListNode**            node
);

LinkedListErrors getPointerToValueByKey(
    LinkedListNode*         tail,
    const char*             key,
    int**                   value
);

LinkedListErrors addNewElement(
    LinkedListNode**            tail,
    const char*                 key,
    int                         value
);

// destructs whole link of nodes, not just this node
LinkedListErrors destructLinkedList(LinkedListNode* node);

#endif