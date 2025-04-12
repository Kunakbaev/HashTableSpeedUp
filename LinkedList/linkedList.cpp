#include "linkedList.hpp"
#include "../common/errorHandlerDefines.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, LINKED_LIST_INVALID_ARGUMENT_ERROR, getLinkedListErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getLinkedListErrorMessage, LINKED_LIST_STATUS_OK);

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getLinkedListErrorMessage)\

LinkedListErrors constructLinkedListNode(
    const char*         key,
    int                 value,
    LinkedListNode**    node
) {
    IF_ARG_NULL_RETURN(node);

    *node = (LinkedListNode*)calloc(1, sizeof(LinkedListNode));
    IF_NOT_COND_RETURN(node != NULL,
                       LINKED_LIST_MEMORY_ALLOCATION_ERROR);

    (*node)->key = (char*)key;
    // size_t keyLen = strlen(key);
    // (*node)->key = (char*)calloc(keyLen + 1, sizeof(char));
    // IF_NOT_COND_RETURN((*node)->key != NULL,
    //                    LINKED_LIST_MEMORY_ALLOCATION_ERROR);
    // strcpy((*node)->key, key);

    (*node)->value = value;
    (*node)->prev  = NULL;
    // **node = {
    //     .value      = value,
    //     .prev       = NULL
    // };

    return LINKED_LIST_STATUS_OK;
}

// returns key not found error if key is not present in list
LinkedListErrors getPointerToValueByKey(
    LinkedListNode*         tail,
    const char*             key,
    int**                   value
) {
    IF_ARG_NULL_RETURN(key);
    IF_ARG_NULL_RETURN(value);

    LinkedListNode* curNode = tail;
    while (curNode != NULL) {
        //LOG_DEBUG_VARS(curNode->key, key);
        if (strcmp(key, curNode->key) == 0) {
            *value = &(curNode->value);
            return LINKED_LIST_STATUS_OK;
        }

        curNode = curNode->prev;
    }

    return LINKED_LIST_KEY_NOT_FOUND_ERROR;
}

LinkedListErrors addNewElement(
    LinkedListNode**        tail,
    const char*             key,
    int                     value
) {
    IF_ARG_NULL_RETURN(tail);

    LinkedListNode* newNode = NULL;
    IF_ERR_RETURN(constructLinkedListNode(key, value, &newNode));

    newNode->prev = *tail; // if tail is NULL everything is still valid
    *tail = newNode;

    return LINKED_LIST_STATUS_OK;
}

// destructs whole link of nodes, not just this node
LinkedListErrors destructLinkedList(LinkedListNode* node) {
    while (node != NULL) {
        LinkedListNode* prev = node->prev;
        //free(node->key);
        free(node);
        node = prev;
    }

    return LINKED_LIST_STATUS_OK;
}
