#pragma GCC target("avx2")

#include "linkedList.hpp"
#include "../common/errorHandlerDefines.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, LINKED_LIST_INVALID_ARGUMENT_ERROR, getLinkedListErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getLinkedListErrorMessage, LINKED_LIST_STATUS_OK);

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getLinkedListErrorMessage)\

const size_t   ONE_KEY_BITS_SIZE = 256 / UNROLL_BATCH_SIZE;
const uint64_t NUM_OF_LETTERS    = 31;
static_assert(256 % UNROLL_BATCH_SIZE == 0);

uint64_t getHashForSmallLenKey(const char* key) {
    uint64_t heh = 0;
    const char* charPtr = key;
    while (*charPtr != '\0') {
        heh *= NUM_OF_LETTERS;
        heh += *charPtr - 'a' + 1;
        ++charPtr;
    }

    return heh;
}

// returns key not found error if key is not present in list
LinkedListErrors getPointerToValueBySmallLenKey(
    LinkedListShortKeyNode*         tail,
    uint64_t                        keyHash,
    int**                           value
) {
    IF_ARG_NULL_RETURN(value);
    //IF_ARG_NULL_RETURN(*value);

    //*value = 0;

    //LOG_DEBUG_VARS("find value");
    const __m256i searchKeyHashReg = _mm256_set1_epi64x(keyHash);
    LinkedListShortKeyNode* curNode = tail;
    size_t len = 0;
    while (curNode != NULL) {
        __m256i curNodeKey = _mm256_loadu_si256((__m256i*)curNode->key);
        __m256i cmpResReg = _mm256_cmpeq_epi64(curNodeKey, searchKeyHashReg);
        //int mask = 0;
        if (!_mm256_testz_si256(cmpResReg, cmpResReg)) {
            break;
        }

        ++len;
        curNode = curNode->prev;
    }
    //LOG_DEBUG_VARS(len);
    //LOG_DEBUG_VARS("loop end");

    if (curNode == NULL) {
        //*value = 0;
        //LOG_DEBUG_VARS("ret not found");
        return LINKED_LIST_KEY_NOT_FOUND_ERROR;
    }

    // *value = 0;
    // yes, that's heavy for loop, but it's only 4 iterations
    for (int i = 0; i < UNROLL_BATCH_SIZE; ++i) {
        if (curNode->key[i] == keyHash) {
            *value = &(curNode->value[i]);
            break;
        }
    }

    //LOG_DEBUG_VARS(*value);
    //LOG_DEBUG_VARS("ret has found");

    return LINKED_LIST_STATUS_OK;
}

static LinkedListErrors setNodesKeyAtIndex(
    LinkedListShortKeyNode** node,
    size_t                   keyIndex,
    uint64_t                 key
) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(*node);

    (*node)->key[keyIndex] = key;
    // // ASK: how to do this properly?
    // // will be done via jump table? only 4 keys, close located
    // __m256i setterReg; // no initialization, because it's one operation and we want to make everything fast
    // switch (keyIndex) {
    //     case 0: setterReg = _mm256_set_epi64x(  0,   0,   0, key); break;
    //     case 1: setterReg = _mm256_set_epi64x(  0,   0, key,   0); break;
    //     case 2: setterReg = _mm256_set_epi64x(  0, key,   0,   0); break;
    //     case 3: setterReg = _mm256_set_epi64x(key,   0,   0,   0); break;
    //     default: assert(false);
    // }

    // // what's better addition or bitwise or
    // _mm256_or_si256((*node)->key, setterReg);

    return LINKED_LIST_STATUS_OK;
}

LinkedListErrors addNewElement2ShortKeysList(
    LinkedListShortKeyNode**    tail,
    uint64_t                    keyHash,
    int                         value
) {
    IF_ARG_NULL_RETURN(tail);

    // LOG_DEBUG_VARS("ok");
    if (*tail == NULL || (*tail)->numOfKeys == UNROLL_BATCH_SIZE) {
        LinkedListShortKeyNode* newNode = NULL;
        // LOG_DEBUG_VARS("new node");
        IF_ERR_RETURN(constructLinkedListShortKeyNode(keyHash, &newNode));
        newNode->value[0] = value;
        // LOG_DEBUG_VARS("good");

        newNode->prev = *tail; // if tail is NULL everything is still valid
        *tail = newNode;
    }

    // LOG_DEBUG_VARS("ok2", *tail, (int)((*tail)->numOfKeys));
    IF_ERR_RETURN(setNodesKeyAtIndex(tail, (*tail)->numOfKeys, keyHash));
    (*tail)->value[(*tail)->numOfKeys] = value;
    ++(*tail)->numOfKeys;

    return LINKED_LIST_STATUS_OK;
}
