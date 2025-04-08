#include "errorsHandlerHashTable.hpp"

const char* getHashTableErrorMessage(HashTableErrors error) {
    #define ERROR_UNPACK(errName, errCode, errMessage)  \
        case HASH_TABLE_##errName##_ERROR:              \
            return "Hash table error: " errMessage ".\n";

    switch (error) {
        case HASH_TABLE_STATUS_OK:
            return "Hash table: no errors occured.\n";
        
        #include "errorsPlainText.txt"

        default:
            return "Hash table error: unknown error\n";
    }

    #undef ERROR_UNPACK
}