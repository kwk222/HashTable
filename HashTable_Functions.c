/*
 *kwk222
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "HashTable_Header.h"
#include "global_defs.h"

int totalBuckets = INITIALBUCKETS;//we will only modify this at some point
int maxChain = CHAININGTHRESHOLD;//If any of these length are more than 5 long, then resize
int maxBucket = PERCENTAGETHRESHOLD;//This is in percentage, if 75% is exceeded, aka maxBucket is exceeded, trigger rehash
int resize = RESIZEPERCENTAGE;//Resize by this much, aka if it is 50, then resize by 50%

int currentBuckets = 0;//divide by totalbuckets, if exceeds maxbucket, resize
int currentChain = 0;//if exceeds maxChain, resize
int numberRehash = 0;

void printHashTable(Node **);

void printHashTable(Node** hashNode)
{
  int i;
  printf("\n");
  for(i = 0; i < totalBuckets; i++) {//that's because it doesn't exist at all anyways
    if (hashNode[i] == NULL) {
      continue;
    }
    Node* currNode = hashNode[i];
    if (currNode->next != NULL) {
      printf("%s%d\n", "Chain contents at index: ", i);
      do {
        printf("key:%lu, value:%d\n", currNode->key, currNode->value);
        currNode = currNode->next;
      } while (currNode != NULL);
    } else {
      printf("%s%d\n", "Chain contents at index: ", i);
      printf("key:%lu, value:%d\n", currNode->key, currNode->value);
    }
    printf("\n");
  }
}

void parseArgs(int argc, char* argv[])
{
  if (argc == 3) {
    if ((strcmp("-input", argv[1]) == 0) || (strcmp("-i", argv[1]) == 0)) {
      return;
    }
  } else if (argc == 2) {
    if ((strcmp("-help", argv[1]) == 0) || (strcmp("-h", argv[1]) == 0)) {
      help();
    }
    if ((strcmp("input", argv[1]) == 0) || (strcmp("-i", argv[1]) == 0)) {
      bail(3, "No argument after -input");
    }
  }
  if (argc >= 4) {
    bail(2, "Input file already specified");
  }

  bail(1, "Incorrect/Unexpected argument entered");

}

Node** createHashTable(Node** hashNode, size_t size)
{
  hashNode = Malloc(size * sizeof(Node *));
  int i;
  for (i = 0; i < size; i++) {
    hashNode[i] = NULL;
  }
  return hashNode;
}

void copyHashTable(Node** newHash, Node** currHash, size_t size)
{//this is basically resize, lmao call create every single time, size = size of current hashTable
  int i;
  for (i = 0; i < size; i++) {
    if (currHash[i] == NULL) {
      continue;
    }
    Node* currNode = currHash[i];
    do {
      create(newHash, currNode->key, currNode->value);
      if (currNode->next == NULL) {
        break;
      }
      currNode = currNode->next;
    } while (currNode != NULL);
  }
}

uint32_t hash(uint64_t key)
{
  uint32_t hash = 5381;
  char* ptr = &key;
  int i;
  for (i = 0; i < 8; i++) {
    hash = (hash * 33) ^ (uint8_t)*ptr;
    ptr++;
  }
  return hash;
}

int create(Node** hashNode, uint64_t key, int value)//This function is done now, remember to change key to hash(key)
{//If create is called with duplicate key, call update instead
  int bucket = hash(key) % totalBuckets;
  Node* newNode = Malloc(sizeof(Node));
  newNode->key = key;
  newNode->value = value;
  newNode->next = NULL;
  if (hashNode[bucket] == NULL) {
    hashNode[bucket] = newNode;
  } else {
    Node* currNode = hashNode[bucket];
    do {
      if (currNode->key == key) {
        fprintf(stderr, "Warning: Attempt to create resulted in an update instead (%"PRIu64", %d)", key, value);
        update(hashNode, key, value);
        return 1;
      }
      if (currNode->next == NULL) {
        currNode->next = newNode;
        return 0;
      }
      currNode = currNode->next;
    } while (currNode != NULL);
  }
}

int update(Node** hashNode, uint64_t key, int value)//This function is done, remember to change key to hash(key)
{
  int bucket = hash(key) % totalBuckets;
  if (hashNode[bucket] == NULL) {
    create(hashNode, key, value);
    return 1;
  } else {
    Node* currNode = hashNode[bucket];
    do {
      if (currNode->key == key) {
        currNode->value = value;
        return 0;
      }
      if (currNode->next == NULL) {
        create(hashNode, key, value);
        return 1;
      }
      currNode = currNode->next;
    } while (currNode != NULL);
  }
}

int read(Node** hashNode, uint64_t key, int* value)//This function is done, remember to change key to hash(key)
{
  int bucket = hash(key) % totalBuckets;
  if (hashNode[bucket] == NULL) {
    return 2;
  } else {
    Node* currNode = hashNode[bucket];
    do {
      if (currNode->key == key) {
        *value = currNode->value;
        return 0;
      }
      if (currNode->next == NULL) {
        return 2;
      }
      currNode = currNode->next;
    } while (currNode != NULL);
  }
}

int delete(Node** hashNode, uint64_t key)//This function is done, remember to change key to hash(key)
{//This should only delete ONLY if it exists, if not, then RIP
  int bucket = hash(key) % totalBuckets;
  if (hashNode[bucket] == NULL) {
    return 2;
  } else {
    Node* currNode = hashNode[bucket];
    Node* prevNode;
    if (currNode->key == key) {//if first bucket is the delete
      if (currNode->next != NULL) {//checks whether the bucket only has one item
        hashNode[bucket] = currNode->next;
        free(currNode);//sdfsdf
        return 0;//sdfsf
      }
      free(currNode);
      hashNode[bucket] = NULL;//sdfs
      return 0;
    }
    do {
      prevNode = currNode;
      currNode = currNode->next;
      if (currNode->key == key) {
        prevNode->next = currNode->next;
        free(currNode);
        return 0;
      }
      if (currNode->next == NULL) {
        return 2;
      }
    } while (currNode != NULL);
  }
}

Node** runHashCommands(Node** hashNode, FILE* fp)
{//run commands
  size_t length;
  char* lineptr = NULL;
  char cmd;
  uint64_t key;
  int value;
  int readable;
  int readValue;
  while (getline(&lineptr, &length, fp) > 0) {
    sscanf(lineptr, "%c %"SCNu64" %d", &cmd, &key, &value);
    if (cmd == 'c') {
      create(hashNode, key, value);
    } else if (cmd == 'r') {
        readable = read(hashNode, key, &readValue);
        if (readable == 0) {
          fprintf(stdout, "Key:%lu Value:%d\n", key, readValue);
        } else {
          fprintf(stdout, "Failed to read Key\n");
        }
    } else if (cmd == 'u') {
      update(hashNode, key, value);
    } else if (cmd == 'd') {
      delete(hashNode, key);
    }
    getHashTableStats(hashNode);//everytime this is called currentChain + currentBuckets is reset to 0, and then it counts again
    if ((currentChain > maxChain) || (((100 * currentBuckets)/totalBuckets) > maxBucket)) {//fix the second condition, integer division is not fun
      int oldBuckets = totalBuckets;
      double resizeMultiplier = (double)resize/(double)100;

      //original
      //totalBuckets = totalBuckets * (1 + resizeMultiplier)
      //new
      //totalBuckets = totalBuckets + ((totalBuckets * resize)/100)

      Node** newHashNode = createHashTable(newHashNode, totalBuckets + ((totalBuckets * resize)/100));

      copyHashTable(newHashNode, hashNode, oldBuckets);

      freeHashTable(hashNode);
      hashNode = newHashNode;

      totalBuckets = totalBuckets + ((totalBuckets * resize)/100);
      numberRehash++;

    }
  }
  printHashTable(hashNode);//This is my own method
  printHashTableStats();
  free(lineptr);
  return hashNode;
}

void getHashTableStats(Node** hashNode)
{
  int i;
  int chain = 0;
  currentChain = 0;
  currentBuckets = 0;
  for (i = 0; i < totalBuckets; i++) {
    if (hashNode[i] == NULL) {
      continue;
    }
    Node* currNode = hashNode[i];
    chain++;
    while(currNode->next != NULL) {//You need to compare it to the max dude
      chain++;
      currNode = currNode->next;
    }
    if (chain > currentChain) {
      currentChain = chain;
    }
    chain = 0;
    currentBuckets++;
  }
}

void printHashTableStats()
{
  printf("Current Buckets Used: %d\n", currentBuckets);
  printf("Current Longest Chain: %d\n", currentChain);
  printf("Total Buckets: %d\n", totalBuckets);
  printf("Number of times resized: %d\n", numberRehash);
}

void freeHashTable(Node** hashNode)
{
  int i;//remember to free hashNode[i]
  for (i = 0; i < totalBuckets; i++) {
    if (hashNode[i] == NULL) {
      //free(hashNode[i]);
      continue;
    } else {
      Node* currNode = hashNode[i];
      Node* prevNode;
      do {
        if (currNode->next == NULL) {
          free(currNode);
          break;
        } else {
          prevNode = currNode;
          currNode = currNode->next;
          free(prevNode);
        }
      } while (currNode != NULL);
      //free(hashNode[i]);
    }
  }
  free(hashNode);
}

void* Malloc(size_t size)
{
  void *ptr;
  ptr = malloc(size);
  if (ptr == NULL) {
    bail(99, "Could not allocate space");
  }
  return ptr;
}

FILE* Fopen(const char* fileName, const char* rwa)
{
  FILE *fp;
  fp = fopen(fileName, rwa);
  if (fp == NULL) {
    bail(10, "Unable to open file");
  }
  return fp;
}

void Fclose(FILE *file)
{
  if(fclose(file) == EOF) {
    bail(13, "Unable to close file");
  }
}


void bail(int value, const char* error)
{
  fprintf(stderr, "Error number (%d), %s\n", value, error);
  exit(0);
}

void help()
{
  fprintf(stdout, "Help function is called \n");
  exit(0);
}

