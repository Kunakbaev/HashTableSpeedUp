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
const size_t HASH_TABLE_LOAD_FACTOR = 20;
const uint64_t HASH_BASE            = 31;
const size_t MAX_WORD_LEN           = 40;

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

HashTableErrors readListOfWordsFromFile(
    size_t*         numOfWordsPtr,
    char***         words,
    const char*     pathToWordsFile
) {
    FILE* file = fopen(pathToWordsFile, "r");
    IF_NOT_COND_RETURN(file != NULL,
                       HASH_TABLE_COULDNT_OPEN_FILE_ERROR);

    LOG_DEBUG("successfuly opened file");
    size_t numOfWords = 0; // equal to number of lines in file
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

        (*words)[wordInd] = (char*)calloc(wordBuffLen + 1, sizeof(char));
        IF_NOT_COND_RETURN((*words)[wordInd] != NULL,
                           HASH_TABLE_MEMORY_ALLOCATION_ERROR);
        strcpy((*words)[wordInd], wordBuffer);
        (*words)[wordInd][wordBuffLen - 1] = '\0';
    }

    *numOfWordsPtr = numOfWords;
    fclose(file);

    return HASH_TABLE_STATUS_OK;
}

HashTableErrors constructHashTableFromWordsFile(
    HashTable*      hashTable,
    size_t          numOfWords,
    char**          words
) {
    IF_ARG_NULL_RETURN(hashTable);

    hashTable->numOfElements = numOfWords;
    hashTable->capacity      = numOfWords / HASH_TABLE_LOAD_FACTOR;
    hashTable->array = (LinkedListNode**)calloc(hashTable->capacity,
                                                sizeof(LinkedListNode*));
    IF_NOT_COND_RETURN(hashTable->array != NULL,
                       HASH_TABLE_MEMORY_ALLOCATION_ERROR);

    for (size_t wordInd = 0; wordInd < numOfWords; ++wordInd) {
        char* word = words[wordInd];
        int keyHash = getHashOfKey(hashTable, word);
        int oldNumOfOccur = 0;
        LinkedListErrors err = findValueByKey(hashTable->array[keyHash], word, &oldNumOfOccur);
        //LOG_DEBUG_VARS(wordBuffer, oldNumOfOccur);
        IF_LIST_ERR_RETURN(addNewElement(&hashTable->array[keyHash], word, oldNumOfOccur + 1));
    }

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

    int keyHash = getHashOfKey(hashTable, word);
    //LOG_DEBUG_VARS(word, keyHash);
    LinkedListErrors err = findValueByKey(hashTable->array[keyHash], word, value);
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

    return HASH_TABLE_STATUS_OK;
}
