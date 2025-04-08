#ifndef LINKED_LIST_ERRORS_HANDLER_HPP
#define LINKED_LIST_ERRORS_HANDLER_HPP

#define ERROR_UNPACK(errName, errCode, errMessage) \
    LINKED_LIST_##errName##_ERROR = errCode,

enum LinkedListErrors {
    LINKED_LIST_STATUS_OK = 0,
    #include "errorsPlainText.txt"
};

#undef ERROR_UNPACK

const char* getLinkedListErrorMessage(LinkedListErrors error);

#endif