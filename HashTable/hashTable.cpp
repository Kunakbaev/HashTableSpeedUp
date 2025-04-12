#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hashTable.hpp"
#include "../common/errorHandlerDefines.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, HASH_TABLE_INVALID_ARGUMENT_ERROR, getHashTableErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getHashTableErrorMessage, HASH_TABLE_STATUS_OK);

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getHashTableErrorMessage)\

#define IF_LIST_ERR_RETURN(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getLinkedListErrorMessage, LINKED_LIST_STATUS_OK, HASH_TABLE_LINKED_LIST_SUBMODULE_ERROR);

// we specifically set load factor to be this high,
// so that hash table is overloaded and we can test perfomance better
const size_t HASH_TABLE_LOAD_FACTOR = 200;
const uint64_t HASH_BASE            = 31;
const size_t MAX_WORD_LEN           = 40;
const size_t MAX_BYTES_IN_FILE      = 13e6 + 10;

static size_t getHashOfKey(
    struct HashTable* hashTable,
    const char*       key
) {
    assert(hashTable != NULL);
    assert(key       != NULL);

    uint64_t heh = 0;
    const char* charPtr = key;
    while (*charPtr != '\0') {
        heh *= HASH_BASE;
        heh += *charPtr - 'a' + 1;
        ++charPtr;
    }

    return heh % hashTable->capacity;
}

static HashTableErrors getNumberOfLines(
    FILE*       file,
    char        charToSearch,
    size_t*     numOfOccurences
) {
    IF_ARG_NULL_RETURN(file);
    IF_ARG_NULL_RETURN(numOfOccurences);

    *numOfOccurences = 0;
    int symbol = -1;
    int tmpCounter = 0;
    while ((symbol = fgetc(file)) != EOF) {
        tmpCounter += symbol == charToSearch;
    }
    *numOfOccurences = tmpCounter;

    rewind(file); // return reading pointer to the beginning of the file

    return HASH_TABLE_STATUS_OK;
}

static size_t getFileSize(FILE* file) {
    struct stat buf;
    fstat(fileno(file), &buf);
    off_t fileSize = buf.st_size;
    return fileSize;
}

HashTableErrors constructHashTableFromWordsFile(
    const char*     pathToWordsFile,
    HashTable*      hashTable
) {
    IF_ARG_NULL_RETURN(hashTable);





    FILE* file = fopen(pathToWordsFile, "r");
    IF_NOT_COND_RETURN(file != NULL,
                       HASH_TABLE_COULDNT_OPEN_FILE_ERROR);

    size_t fileSize = getFileSize(file);
    LOG_DEBUG("successfuly opened file");
    LOG_DEBUG_VARS(fileSize);

    hashTable->fileBuffer = (char*)calloc(MAX_BYTES_IN_FILE, sizeof(char));
    IF_NOT_COND_RETURN(hashTable->fileBuffer != NULL,
                       HASH_TABLE_MEMORY_ALLOCATION_ERROR);

    fread(hashTable->fileBuffer, sizeof(char), fileSize, file);

    size_t numOfWords = 0;
    char* ptr = hashTable->fileBuffer, *nxt = ptr;
    while ((nxt = strchr(ptr, '\n')) != NULL) {
        size_t len = nxt - ptr - 1;
        ++numOfWords;

        *nxt = '\0'; // change \n to \0
        ptr = nxt + 1;
    }
    LOG_DEBUG_VARS("counted number of words");








    hashTable->numOfElements = numOfWords;
    hashTable->capacity = numOfWords / HASH_TABLE_LOAD_FACTOR;
    hashTable->array    =         (LinkedListNode**)calloc(hashTable->capacity,
                                                          sizeof(LinkedListNode*));
    IF_NOT_COND_RETURN(hashTable->array  != NULL,
                       HASH_TABLE_MEMORY_ALLOCATION_ERROR);

    LOG_DEBUG_VARS(hashTable->capacity);
    char* word = hashTable->fileBuffer;
    for (size_t wordInd = 0; wordInd < numOfWords; ++wordInd) {
        size_t wordLen = strlen(word);
        //LOG_DEBUG_VARS(word);

        // if (wordInd % 10000 == 0)
        //     LOG_DEBUG_VARS(wordInd);

        int keyHash = getHashOfKey(hashTable, word);
        int* nodeValuePtr = NULL;
        LinkedListErrors err = getPointerToValueByKey(hashTable->array[keyHash], word, &nodeValuePtr);
        //LOG_DEBUG_VARS(wordBuffer, oldNumOfOccur);
        if (err == LINKED_LIST_KEY_NOT_FOUND_ERROR) {
            IF_LIST_ERR_RETURN(addNewElement(&hashTable->array[keyHash], word, 1));
        } else {
            *nodeValuePtr = *nodeValuePtr + 1;
        }

        // if (wordInd % 30000 == 0) LOG_DEBUG_VARS(wordInd);
        //LOG_DEBUG_VARS(wordLen, word);
        word += wordLen + 1;
    }
    LOG_DEBUG_VARS("finished adding words to table");

    return HASH_TABLE_STATUS_OK;
}

// returns list submodule error in case if key is not present in hash table
HashTableErrors getNumberOfWordsOccurences(
    HashTable*      hashTable,
    const char*     word,
    int*            value
) {
    IF_ARG_NULL_RETURN(hashTable);
    IF_ARG_NULL_RETURN(word);
    IF_ARG_NULL_RETURN(value);

    int* nodeValuePtr = NULL;
    int keyHash = getHashOfKey(hashTable, word);
    //LOG_DEBUG_VARS(word, keyHash);
    LinkedListErrors err = getPointerToValueByKey(hashTable->array[keyHash], word, &nodeValuePtr);
    *value = nodeValuePtr == NULL ? 0 : *nodeValuePtr;

    if (err != LINKED_LIST_KEY_NOT_FOUND_ERROR &&
        err != LINKED_LIST_STATUS_OK)
        IF_LIST_ERR_RETURN(err);
    return err == LINKED_LIST_KEY_NOT_FOUND_ERROR
                ? HASH_TABLE_KEY_NOT_FOUND_ERROR
                : HASH_TABLE_STATUS_OK;
}

HashTableErrors destructHashTable(
    HashTable*      hashTable
) {
    for (int i = 0; i < hashTable->capacity; ++i) {
        IF_LIST_ERR_RETURN(destructLinkedList(hashTable->array[i]));
    }
    free(hashTable->array);
    free(hashTable->fileBuffer);

    return HASH_TABLE_STATUS_OK;
}
