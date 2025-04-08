#include "errorsHandlerLinkedList.hpp"

const char* getLinkedListErrorMessage(LinkedListErrors error) {
    #define ERROR_UNPACK(errName, errCode, errMessage)  \
        case LINKED_LIST_##errName##_ERROR:             \
            return "Linked list error: " errMessage ".\n";

    switch (error) {
        case LINKED_LIST_STATUS_OK:
            return "Linked list: no errors occured.\n";
        
        #include "errorsPlainText.txt"

        default:
            return "Linked list error: unknown error\n";
    }

    #undef ERROR_UNPACK
}