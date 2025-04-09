#include "linkedList.hpp"
#include "../common/errorHandlerDefines.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, LINKED_LIST_INVALID_ARGUMENT_ERROR, getLinkedListErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getLinkedListErrorMessage, LINKED_LIST_STATUS_OK);

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getLinkedListErrorMessage)\

const uint64_t NUM_OF_LETTERS = 31;

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

LinkedListErrors constructLinkedListShortKeyNode(
    uint64_t                    keyHash,
    int                         value,
    LinkedListShortKeyNode**    node
) {
    IF_ARG_NULL_RETURN(node);

    *node = (LinkedListShortKeyNode*)calloc(1, sizeof(LinkedListShortKeyNode));
    IF_NOT_COND_RETURN(node != NULL,
                       LINKED_LIST_MEMORY_ALLOCATION_ERROR);

    (*node)->key   = keyHash;
    (*node)->value = value;
    (*node)->prev  = NULL;

    return LINKED_LIST_STATUS_OK;
}

// returns key not found error if key is not present in list
LinkedListErrors findValueBySmallLenKey(
    const LinkedListShortKeyNode*   tail,
    uint64_t                        keyHash,
    int*                            value
) {
    IF_ARG_NULL_RETURN(value);

    const LinkedListShortKeyNode* curNode = tail;
    while (curNode != NULL) {
        //LOG_DEBUG_VARS(curNode->key, key);
        if (curNode->key == keyHash) { [[unlikely]]
            *value = curNode->value;
            return LINKED_LIST_STATUS_OK;
        }

        curNode = curNode->prev;
    }

    return LINKED_LIST_KEY_NOT_FOUND_ERROR;
}

LinkedListErrors addNewElement2ShortKeysList(
    LinkedListShortKeyNode**    tail,
    uint64_t                    keyHash,
    int                         value
) {
    IF_ARG_NULL_RETURN(tail);

    LinkedListShortKeyNode* newNode = NULL;
    IF_ERR_RETURN(constructLinkedListShortKeyNode(keyHash, value, &newNode));

    newNode->prev = *tail; // if tail is NULL everything is still valid
    *tail = newNode;

    return LINKED_LIST_STATUS_OK;
}

LinkedListErrors destructShortKeysLinkedList(LinkedListShortKeyNode* node) {
    while (node != NULL) {
        LinkedListShortKeyNode* prev = node->prev;
        //free(node->key);
        free(node);
        node = prev;
    }

    return LINKED_LIST_STATUS_OK;
}
