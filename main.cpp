#include "LinkedList/linkedList.hpp"
#include "HashTable/hashTable.hpp"
#include "external/LoggerLib/include/logLib.hpp"

const size_t MAX_WORD_LEN   = 10;
const size_t NUM_OF_QUERIES = 2000000;

void generateRandomWord(size_t maxWordLen, char** word) {
    assert(word != NULL);

    int len = rand() % maxWordLen + 1;
    *word = (char*)calloc(len + 1, sizeof(char));
    assert(*word != NULL);

    for (int i = 0; i < len; ++i)
        (*word)[i] = 'a' + (rand() % 26);
}

char** prepareListOfWords(
    size_t numOfWords,
    char** words,
    size_t numToGen
) {
    char** result = (char**)calloc(numToGen, sizeof(char*));
    assert(result != NULL);

    for (int i = 0; i < numToGen; ++i) {
        // random seed is static, so each run we get same set of words
        if (rand() & 1) {
            generateRandomWord(MAX_WORD_LEN, &result[i]);
        } else {
            size_t wordInd = rand() % numOfWords;
            char* word = words[wordInd];
            size_t len = strlen(word);
            result[i] = (char*)calloc(len + 1, sizeof(char));
            assert(result[i] != NULL);

            strcpy(result[i], word);
        }
    }

    return result;
}

int main() {
    setLoggingLevel(DEBUG);

    LOG_DEBUG("Hello!");

    size_t numOfWords = 0;
    char** words = NULL;
    readListOfWordsFromFile(&numOfWords, &words, "sourceFiles/allWords.txt");

    size_t numOfQueires = NUM_OF_QUERIES;
    char** wordsForQueries = prepareListOfWords(numOfWords, words, numOfQueires);

    struct HashTable hashTable = {};
    constructHashTableFromWordsFile(&hashTable, numOfWords, words);

    

    size_t sumOfOccur = 0;
    for (size_t queryInd = 0; queryInd < numOfQueires; ++queryInd) {
        char* word = wordsForQueries[queryInd];
        int numOfOccur = 0;
        HashTableErrors err = getNumberOfWordsOccurences(&hashTable, word, &numOfOccur);
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

    for (size_t wordInd = 0; wordInd < numOfQueires; ++wordInd) {
        free(wordsForQueries[wordInd]);
    }
    free(wordsForQueries);



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