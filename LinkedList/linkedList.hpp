#ifndef LINKED_LIST_LINKED_LIST_HPP
#define LINKED_LIST_LINKED_LIST_HPP

#include <inttypes.h>
#include <cstddef>

#include "errorsHandlerLinkedList.hpp"

const size_t MAX_NUM_OF_NODES           = 3710;
const size_t MAX_NUM_OF_SMALL_KEY_NODES = 12e5 + 10;

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

// WARNING: don't forget to call this function
LinkedListErrors allocateFreeNodesBuffers();

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
LinkedListErrors destructLinkedList();

LinkedListErrors destructShortKeysLinkedList();

#endif