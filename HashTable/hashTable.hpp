#ifndef HASH_TABLE_HASH_TABLE_HPP
#define HASH_TABLE_HASH_TABLE_HPP

#include <inttypes.h>
#include <cstddef>

#include "../LinkedList/linkedList.hpp"
#include "errorsHandlerHashTable.hpp"

struct HashTable {
    size_t                    numOfLongWords;
    size_t                    capacityLongWords;
    LinkedListNode**          longWordsArray;

    size_t                    numOfShortWords;
    size_t                    capacityShortWords;
    LinkedListShortKeyNode**  shortWordsArray;
};

HashTableErrors readListOfWordsFromFile(
    size_t*         numOfWordsPtr,
    char***         words,
    const char*     pathToWordsFile,
    size_t*         numOfLongWordsPtr,
    size_t*         numOfShortWordsPtr
);

HashTableErrors constructHashTableFromWordsFile(
    HashTable*      hashTable,
    size_t          numOfLongWords,
    size_t          numOfShortWords,
    char**          words
);

HashTableErrors getNumberOfWordsOccurences(
    HashTable*      hashTable,
    const char*     word,
    int*            value
);

HashTableErrors destructHashTable(
    HashTable*      hashTable
);

#endif