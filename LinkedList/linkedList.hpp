#ifndef LINKED_LIST_LINKED_LIST_HPP
#define LINKED_LIST_LINKED_LIST_HPP

#include <inttypes.h>

#include "errorsHandlerLinkedList.hpp"

struct LinkedListNode {
    char*                           key;
    int                             value;
    LinkedListNode*                 prev;
};

struct LinkedListShortKeyNode {
    uint64_t                        key;
    int                             value;
    LinkedListShortKeyNode*         prev;
};

uint64_t getHashForSmallLenKey(const char* key);

LinkedListErrors constructLinkedListShortKeyNode(
    uint64_t                        keyHash, 
    int                             value,
    LinkedListShortKeyNode**        node
);

LinkedListErrors constructLinkedListNode(
    const char*                     key, 
    int                             value,
    LinkedListNode**                node
);

LinkedListErrors constructLinkedListNode(
    const char*                     key, 
    int                             value,
    LinkedListShortKeyNode**        node
);

// returns key not found error if key is not present in list
LinkedListErrors findValueByKey(
    const LinkedListNode*           tail,
    const char*                     key,
    int*                            value
);

LinkedListErrors findValueBySmallLenKey(
    const LinkedListShortKeyNode*   tail,
    uint64_t                        keyHash,
    int*                            value
);

LinkedListErrors addNewElement(
    LinkedListNode**                tail,
    const char*                     key,
    int                             value
);

LinkedListErrors addNewElement2ShortKeysList(
    LinkedListShortKeyNode**        tail,
    uint64_t                        keyHash,
    int                             value
);

// destructs whole link of nodes, not just this node
LinkedListErrors destructLinkedList(LinkedListNode* node);

LinkedListErrors destructShortKeysLinkedList(LinkedListShortKeyNode* node);

#endif