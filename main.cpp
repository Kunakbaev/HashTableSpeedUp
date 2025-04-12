#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "LinkedList/linkedList.hpp"
#include "HashTable/hashTable.hpp"
#include "external/LoggerLib/include/logLib.hpp"

const size_t MAX_WORD_LEN      = 40;
const size_t MAX_BYTES_IN_FILE = 33e6 + 10;

size_t getFileSize(FILE* file) {
    struct stat buf;
    fstat(fileno(file), &buf);
    off_t fileSize = buf.st_size;
    return fileSize;
}

void askQueries(
    struct HashTable*       hashTable
) {
    assert(hashTable != NULL);

    FILE* file = fopen("sourceFiles/queries.txt", "rb");
    assert(file != NULL);
    if (file == NULL) {
        printf(getHashTableErrorMessage(HASH_TABLE_COULDNT_OPEN_FILE_ERROR));
        return;
    }

    size_t fileSize = getFileSize(file);
    LOG_DEBUG("successfuly opened queries file");
    LOG_DEBUG_VARS(fileSize);

    char* fileBuffer = (char*)calloc(MAX_BYTES_IN_FILE, sizeof(char));
    assert(fileBuffer != NULL);

    fread(fileBuffer, sizeof(char), fileSize, file);

    size_t sumOfOccur = 0;
    size_t cntOfQueries = 0;
    char* word = fileBuffer, *nxt = word;
    while ((nxt = strchr(word, '\n')) != NULL) {
        *nxt = '\0'; // change \n to \0

        int numOfOccur = 0;
        HashTableErrors err = getNumberOfWordsOccurences(hashTable, word, &numOfOccur);
        // if got error, than either no such word is present in hash table, 
        // or that's a real error and not just key absence
        if (err == HASH_TABLE_STATUS_OK) {
            sumOfOccur += numOfOccur;
            //LOG_DEBUG_VARS(word, numOfOccur);
        } else {
            //LOG_DEBUG_VARS(word, "no such word in text");
        }

        word = nxt + 1;
        ++cntOfQueries;
    }
    LOG_DEBUG_VARS(cntOfQueries, sumOfOccur);

    fclose(file);
    free(fileBuffer);
}

int main() {
    setLoggingLevel(DEBUG);
    
    // WARNING: don't forget to call this function
    allocateFreeNodesBuffers();

    struct HashTable hashTable = {};
    constructHashTableFromWordsFile("sourceFiles/allWords.txt", &hashTable);

    askQueries(&hashTable);

    // const char* queries[] = {
    //     "hobbit",
    //     "hello",
    //     "home",
    //     "fast",
    //     "shield",
    //     "armor",
    //     "Bilbo",
    //     "was",
    //     "not",
    //     "absolutely",
    //     "definitely"
    // };

    // const size_t NUM_OF_Q = sizeof(queries) / sizeof(*queries);

    // // setLoggingLevel(DEBUG);
    // for (size_t wordInd = 0; wordInd < NUM_OF_Q; ++wordInd) {
    //     int value = -1;
    //     HashTableErrors err = getNumberOfWordsOccurences(&hashTable, queries[wordInd], &value);
    //     if (err != HASH_TABLE_STATUS_OK) {
    //         LOG_DEBUG_VARS(queries[wordInd], "no such word in text");
    //     } else {
    //         LOG_DEBUG_VARS(queries[wordInd], value);
    //     }
    // }


    destructHashTable(&hashTable);

    return 0;
}