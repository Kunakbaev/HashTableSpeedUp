#ifndef HASH_TABLE_ERRORS_HANDLER_HPP
#define HASH_TABLE_ERRORS_HANDLER_HPP

#define ERROR_UNPACK(errName, errCode, errMessage) \
    HASH_TABLE_##errName##_ERROR = errCode,

enum HashTableErrors {
    HASH_TABLE_STATUS_OK = 0,
    #include "errorsPlainText.txt"
};

#undef ERROR_UNPACK

const char* getHashTableErrorMessage(HashTableErrors error);

#endif