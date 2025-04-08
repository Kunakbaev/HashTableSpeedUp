#ifndef HASH_TABLE_HASH_TABLE_HPP
#define HASH_TABLE_HASH_TABLE_HPP

#include <inttypes.h>
#include <cstddef>

#include "../LinkedList/linkedList.hpp"
#include "errorsHandlerHashTable.hpp"

struct HashTable {
    size_t                  numOfElements;
    size_t                  capacity;
    LinkedListNode**        array;
};

HashTableErrors readListOfWordsFromFile(
    size_t*         numOfWordsPtr,
    char***         words,
    const char*     pathToWordsFile
);

HashTableErrors constructHashTableFromWordsFile(
    HashTable*      hashTable,
    size_t          numOfWords,
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