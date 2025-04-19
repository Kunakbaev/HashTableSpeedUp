#ifndef LINKED_LIST_LINKED_LIST_HPP
#define LINKED_LIST_LINKED_LIST_HPP

#pragma GCC target("avx2")

#include <inttypes.h>
#include <cstddef>
#include <immintrin.h>

#include "errorsHandlerLinkedList.hpp"

const size_t UNROLL_BATCH_SIZE          = 16;
const size_t MAX_NUM_OF_NODES           = 120000;
const size_t MAX_NUM_OF_SMALL_KEY_NODES = 12e5 + 10;

struct LinkedListNode {
    LinkedListNode*                 prev;
    char*                           key;
    int                             value;
};

struct LinkedListShortKeyNode {
    LinkedListShortKeyNode*         prev;
    __restrict uint64_t                        key[UNROLL_BATCH_SIZE];
    int                             value[UNROLL_BATCH_SIZE];
    char                            numOfKeys;
};

// WARNING: don't forget to call this function
LinkedListErrors allocateFreeNodesBuffers();

uint64_t getHashForSmallLenKey(const char* key);

LinkedListErrors constructLinkedListShortKeyNode(
    int                             value,
    LinkedListShortKeyNode**        node
);

LinkedListErrors constructLinkedListNode(
    const char*                     key, 
    int                             value,
    LinkedListNode**                node
);

// returns key not found error if key is not present in list
LinkedListErrors findValueByKey(
    LinkedListNode*                 tail,
    const char*                     key,
    int**                           value
);

LinkedListErrors getPointerToValueBySmallLenKey(
    LinkedListShortKeyNode*         tail,
    uint64_t                        keyHash,
    int**                           value
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