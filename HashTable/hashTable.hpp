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
    char*                   fileBuffer;
};

HashTableErrors constructHashTableFromWordsFile(
    const char*     pathToWordsFile,
    HashTable*      hashTable
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