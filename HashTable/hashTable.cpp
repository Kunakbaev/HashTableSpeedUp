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
const size_t SMALL_WORD_MAX_LEN     = 12;
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

    return heh & (hashTable->capacityLongWords - 1);
    //return heh % hashTable->capacityLongWords;
}

static size_t getHashOfSmallKey(
    struct HashTable* hashTable,
    uint64_t          key
) {
    assert(hashTable != NULL);
    return key & (hashTable->capacityShortWords - 1);
    //return key % hashTable->capacityShortWords;
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

HashTableErrors readListOfWordsFromFile(
    size_t*         numOfWordsPtr,
    char***         words,
    const char*     pathToWordsFile,
    size_t*         numOfLongWordsPtr,
    size_t*         numOfShortWordsPtr
) {
    FILE* file = fopen(pathToWordsFile, "rb");
    IF_NOT_COND_RETURN(file != NULL,
                       HASH_TABLE_COULDNT_OPEN_FILE_ERROR);

    size_t fileSize = getFileSize(file);
    LOG_DEBUG("successfuly opened file");
    LOG_DEBUG_VARS(fileSize);

    char* fileBuffer = (char*)calloc(MAX_BYTES_IN_FILE, sizeof(char));
    IF_NOT_COND_RETURN(fileBuffer != NULL,
                       HASH_TABLE_MEMORY_ALLOCATION_ERROR);

    fread(fileBuffer, sizeof(char), fileSize, file);

    size_t numOfWords      = 0; // equal to number of lines in file
    size_t numOfLongWords  = 0;
    size_t numOfShortWords = 0;
    getNumberOfLines(file, '\n', &numOfWords);
    LOG_DEBUG_VARS(numOfWords);

    *words = (char**)calloc(numOfWords, sizeof(char*));
    IF_NOT_COND_RETURN(*words != NULL,
                       HASH_TABLE_MEMORY_ALLOCATION_ERROR);

    char wordBuffer[MAX_WORD_LEN] = {};
    for (size_t wordInd = 0; wordInd < numOfWords; ++wordInd) {
        fgets(wordBuffer, MAX_WORD_LEN, file);
        //LOG_DEBUG_VARS(wordBuffer);

        // remove \n from end of word
        size_t wordBuffLen = strlen(wordBuffer);
        if (wordBuffLen <= 1) continue;
        wordBuffer[wordBuffLen - 1] = '\0';

        if (wordBuffLen > SMALL_WORD_MAX_LEN)
            ++numOfLongWords;
        else
            ++numOfShortWords;

        (*words)[wordInd] = (char*)calloc(wordBuffLen + 1, sizeof(char));
        IF_NOT_COND_RETURN((*words)[wordInd] != NULL,
                           HASH_TABLE_MEMORY_ALLOCATION_ERROR);
        strcpy((*words)[wordInd], wordBuffer);
        (*words)[wordInd][wordBuffLen - 1] = '\0';
    }

    *numOfWordsPtr      = numOfWords;
    *numOfLongWordsPtr  = numOfLongWords;
    *numOfShortWordsPtr = numOfShortWords;
    fclose(file);

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
    size_t wordLen = strlen(word);

    int* nodeValuePtr = NULL;
    LinkedListErrors err = LINKED_LIST_STATUS_OK;
    if (wordLen > SMALL_WORD_MAX_LEN) {
        int keyHash = getHashOfKey(hashTable, word);
        err = findValueByKey(hashTable->longWordsArray[keyHash], word, &nodeValuePtr);
    } else {
        uint64_t keyHashForSmall = getHashForSmallLenKey(word);
        int keyHash = getHashOfSmallKey(hashTable, keyHashForSmall);
        //LOG_DEBUG_VARS(keyHash, keyHashForSmall, word);
        err = getPointerToValueBySmallLenKey(hashTable->shortWordsArray[keyHash], keyHashForSmall, &nodeValuePtr);
    }
    *value = nodeValuePtr == NULL ? 0 : *nodeValuePtr;

    if (err != LINKED_LIST_KEY_NOT_FOUND_ERROR &&
        err != LINKED_LIST_STATUS_OK)
        IF_LIST_ERR_RETURN(err);
    return err == LINKED_LIST_KEY_NOT_FOUND_ERROR
                ? HASH_TABLE_KEY_NOT_FOUND_ERROR
                : HASH_TABLE_STATUS_OK;
}

static size_t getPowerOf2Capacity(size_t numOfWords) {
    numOfWords /= HASH_TABLE_LOAD_FACTOR;
    int powOf2 = 1;
    while (powOf2 < numOfWords)
        powOf2 *= 2;

    return powOf2;
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

    size_t numOfLongWords  = 0;
    size_t numOfShortWords = 0;
    char* ptr = hashTable->fileBuffer, *nxt = ptr;
    while ((nxt = strchr(ptr, '\n')) != NULL) {
        size_t len = nxt - ptr - 1;
        //LOG_DEBUG_VARS(len);
        if (len > SMALL_WORD_MAX_LEN) {
            ++numOfLongWords;
        } else {
            ++numOfShortWords;
        }

        *nxt = '\0'; // change \n to \0
        ptr = nxt + 1;
    }
    LOG_DEBUG_VARS("counted number of words");








    hashTable->numOfLongWords     = numOfLongWords;
    hashTable->numOfShortWords    = numOfShortWords;
    hashTable->capacityLongWords  = getPowerOf2Capacity(numOfLongWords);
    hashTable->capacityShortWords = getPowerOf2Capacity(numOfShortWords);
    hashTable->longWordsArray  =         (LinkedListNode**)calloc(hashTable->capacityLongWords,
                                                          sizeof(LinkedListNode*));
    hashTable->shortWordsArray = (LinkedListShortKeyNode**)calloc(hashTable->capacityShortWords,
                                                          sizeof(LinkedListShortKeyNode*));
    IF_NOT_COND_RETURN(hashTable->longWordsArray  != NULL,
                       HASH_TABLE_MEMORY_ALLOCATION_ERROR);
    IF_NOT_COND_RETURN(hashTable->shortWordsArray != NULL,
                       HASH_TABLE_MEMORY_ALLOCATION_ERROR);

    LOG_DEBUG_VARS(hashTable->capacityShortWords, hashTable->capacityLongWords);
    size_t numOfWords = numOfLongWords + numOfShortWords;
    char* word = hashTable->fileBuffer;
    for (size_t wordInd = 0; wordInd < numOfWords; ++wordInd) {
        size_t wordLen = strlen(word);

        // if (wordInd % 10000 == 0)
        //     LOG_DEBUG_VARS(wordInd);

        if (wordLen > SMALL_WORD_MAX_LEN) {
            int keyHash = getHashOfKey(hashTable, word);
            int* nodeValuePtr = NULL;
            LinkedListErrors err = findValueByKey(hashTable->longWordsArray[keyHash], word, &nodeValuePtr);
            //LOG_DEBUG_VARS(wordBuffer, oldNumOfOccur);
            if (err == LINKED_LIST_KEY_NOT_FOUND_ERROR) {
                IF_LIST_ERR_RETURN(addNewElement(&hashTable->longWordsArray[keyHash], word, 1));
            } else {
                *nodeValuePtr = *nodeValuePtr + 1;
            }
        } else {
            uint64_t keyHashForSmall = getHashForSmallLenKey(word);
            int keyHash = getHashOfSmallKey(hashTable, keyHashForSmall);
            int* nodeValuePtr = NULL;
            LinkedListErrors err = getPointerToValueBySmallLenKey(hashTable->shortWordsArray[keyHash], keyHashForSmall, &nodeValuePtr);
            //LOG_DEBUG_VARS(wordBuffer, oldNumOfOccur);
            if (err == LINKED_LIST_KEY_NOT_FOUND_ERROR) {
                IF_LIST_ERR_RETURN(addNewElement2ShortKeysList(&hashTable->shortWordsArray[keyHash], keyHashForSmall, 1));
            } else {
                //LOG_DEBUG_VARS(nodeValuePtr);
                *nodeValuePtr = *nodeValuePtr + 1;
            }
        }

        // if (wordInd % 30000 == 0) LOG_DEBUG_VARS(wordInd);
        //LOG_DEBUG_VARS(wordLen, word);
        word += wordLen + 1;
    }
    LOG_DEBUG_VARS("finished adding words to table");

    return HASH_TABLE_STATUS_OK;
}

HashTableErrors destructHashTable(
    HashTable*      hashTable
) {
    IF_LIST_ERR_RETURN(destructLinkedList());
    IF_LIST_ERR_RETURN(destructShortKeysLinkedList());
    free(hashTable->shortWordsArray);
    free(hashTable->longWordsArray);
    free(hashTable->fileBuffer);

    return HASH_TABLE_STATUS_OK;
}
