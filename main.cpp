#include "LinkedList/linkedList.hpp"
#include "HashTable/hashTable.hpp"
#include "external/LoggerLib/include/logLib.hpp"

const size_t MAX_WORD_LEN   = 20;
const size_t NUM_OF_QUERIES = 8000000;
const int    NUM_OF_LETTERS = 26;

void generateRandomWord(char* word) {
    assert(word != NULL);

    int len = rand() % MAX_WORD_LEN + 1;
    for (int i = 0; i < len; ++i)
        word[i] = 'a' + (rand() % NUM_OF_LETTERS);
}

void getWordWordForQuery(char* result, char** words, size_t numOfWords) {
    if (rand() & 1) {
        generateRandomWord(result);
        return;
    }

    size_t wordInd = rand() % numOfWords;
    char* word = words[wordInd];
    size_t len = strlen(word);
    strcpy(result, word);
}

void askQueries(
    struct HashTable*       hashTable,
    char**                  words,
    size_t                  numOfWords
) {
    assert(hashTable != NULL);

    char word[MAX_WORD_LEN] = {};
    size_t sumOfOccur = 0;
    for (size_t queryInd = 0; queryInd < NUM_OF_QUERIES; ++queryInd) {
        getWordWordForQuery(word, words, numOfWords);

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
    }
    LOG_DEBUG_VARS(sumOfOccur);
}

int main() {
    setLoggingLevel(DEBUG);

    LOG_DEBUG("Hello!");

    // WARNING: don't forget to call this function
    allocateFreeNodesBuffers();

    size_t numOfWords      = 0;
    size_t numOfShortWords = 0;
    size_t numOfLongWords  = 0;
    char** words = NULL;
    readListOfWordsFromFile(&numOfWords, &words, "sourceFiles/allWords.txt", &numOfLongWords, &numOfShortWords);

    LOG_DEBUG_VARS(numOfLongWords, numOfShortWords);

    struct HashTable hashTable = {};
    constructHashTableFromWordsFile(&hashTable, numOfLongWords, numOfShortWords, words);    
    LOG_DEBUG_VARS("ok");

    askQueries(&hashTable, words, numOfWords);





    // const char* queries[] = {
    //     "hobbit",
    //     "hello",
    //     "home",
    //     "fast",
    //     "shield",
    //     "armor",
    //     "Bilbo",
    //     "was",
    //     "not"
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




    // int numOfOccur = 0;
    // HashTableErrors err = getNumberOfWordsOccurences(&hashTable, "Hermione", &numOfOccur);
    // LOG_DEBUG_VARS(numOfOccur);

    destructHashTable(&hashTable);
    for (size_t wordInd = 0; wordInd < numOfWords; ++wordInd) {
        if (words[wordInd] != NULL) {
            //LOG_DEBUG_VARS(wordInd, words[wordInd]);
            free(words[wordInd]);
        }
    }
    free(words);


    // struct HashTable hashTable = {};
    // constructHashTableFromWordsFile(&hashTable, "sourceFiles/allWords.txt");

    // const char* words[] = {
    //     "hobbit",
    //     "hello",
    //     "home",
    //     "fast",
    //     "shield",
    //     "armor",
    //     "Bilbo",
    //     "was",
    //     "not"
    // };

    // const size_t NUM_OF_WORDS = sizeof(words) / sizeof(*words);

    // // setLoggingLevel(DEBUG);
    // for (size_t wordInd = 0; wordInd < NUM_OF_WORDS; ++wordInd) {
    //     int value = -1;
    //     HashTableErrors err = getNumberOfWordsOccurences(&hashTable, words[wordInd], &value);
    //     if (err != HASH_TABLE_STATUS_OK) {
    //         LOG_DEBUG_VARS(words[wordInd], "no such word in text");
    //     } else {
    //         LOG_DEBUG_VARS(words[wordInd], value);
    //     }
    // }

    // destructHashTable(&hashTable);

    // LinkedListNode* tail = NULL;
    // constructLinkedListNode("i am first node", 12, &tail);

    // addNewElement(&tail, "i am 2nd node", 19);

    // int value = -1;
    // LinkedListErrors err = findValueByKey(tail, "i am 2nd node", &value);
    // LOG_DEBUG_VARS(getLinkedListErrorMessage(err), value);

    // destructLinkedList(tail);

    return 0;
}
