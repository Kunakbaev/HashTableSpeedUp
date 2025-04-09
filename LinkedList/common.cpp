#include "linkedList.hpp"
#include "../common/errorHandlerDefines.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, LINKED_LIST_INVALID_ARGUMENT_ERROR, getLinkedListErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getLinkedListErrorMessage, LINKED_LIST_STATUS_OK);

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getLinkedListErrorMessage)\

LinkedListNode*         listNodesBuffer         = NULL;
LinkedListShortKeyNode* listShortKeyNodesBuffer = NULL;
size_t freeNodeInd          = MAX_NUM_OF_NODES; // protection from not calling allocation function
size_t smallWordFreeNodeInd = MAX_NUM_OF_SMALL_KEY_NODES;

// WARNING: don't forget to call this function
LinkedListErrors allocateFreeNodesBuffers() {
    freeNodeInd = smallWordFreeNodeInd = 0;

    listNodesBuffer         =         (LinkedListNode*)calloc(MAX_NUM_OF_NODES,
                                                              sizeof(LinkedListNode));
    listShortKeyNodesBuffer = (LinkedListShortKeyNode*)calloc(MAX_NUM_OF_SMALL_KEY_NODES,
                                                              sizeof(LinkedListShortKeyNode));

    IF_NOT_COND_RETURN(listNodesBuffer         != NULL,
                       LINKED_LIST_MEMORY_ALLOCATION_ERROR);
    IF_NOT_COND_RETURN(listShortKeyNodesBuffer != NULL,
                       LINKED_LIST_MEMORY_ALLOCATION_ERROR);

    return LINKED_LIST_STATUS_OK;
}

LinkedListErrors constructLinkedListNode(
    const char*         key,
    int                 value,
    LinkedListNode**    node
) {
    IF_ARG_NULL_RETURN(node);

    IF_NOT_COND_RETURN(freeNodeInd != MAX_NUM_OF_NODES,
                       LINKED_LIST_MEMORY_ALLOCATION_ERROR);
    *node = &listNodesBuffer[freeNodeInd++];

    (*node)->key   = (char*)key;
    (*node)->value = value;
    (*node)->prev  = NULL;

    return LINKED_LIST_STATUS_OK;
}

LinkedListErrors constructLinkedListShortKeyNode(
    uint64_t                    keyHash,
    int                         value,
    LinkedListShortKeyNode**    node
) {
    IF_ARG_NULL_RETURN(node);

    //LOG_DEBUG_VARS(smallWordFreeNodeInd);
    IF_NOT_COND_RETURN(smallWordFreeNodeInd != MAX_NUM_OF_SMALL_KEY_NODES,
                       LINKED_LIST_MEMORY_ALLOCATION_ERROR);
    *node = &listShortKeyNodesBuffer[smallWordFreeNodeInd++];

    (*node)->key   = keyHash;
    (*node)->value = value;
    (*node)->prev  = NULL;

    return LINKED_LIST_STATUS_OK;
}

// destructs whole link of nodes, not just this node
LinkedListErrors destructLinkedList() {
    free(listNodesBuffer);
    freeNodeInd = MAX_NUM_OF_NODES;

    return LINKED_LIST_STATUS_OK;
}

LinkedListErrors destructShortKeysLinkedList() {
    free(listShortKeyNodesBuffer);
    smallWordFreeNodeInd = MAX_NUM_OF_SMALL_KEY_NODES;

    return LINKED_LIST_STATUS_OK;
}
