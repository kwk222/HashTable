/*
 *
 * Main source code file for hash table program
 * 
 *
 */

#include <stdio.h>          /* for FILE */
#include "global_defs.h"    /* for node structure and typedef Node */
#include "HashTable_Header.h"    /* for supporting functions */

int main(int argc, char *argv[]) {
  Node **hashTable = NULL;
  FILE *cmdFile = NULL;

  parseArgs(argc, argv);

  cmdFile = Fopen(argv[2], "r");

  hashTable = createHashTable(hashTable, INITIALBUCKETS);

  hashTable = runHashCommands(hashTable, cmdFile);

  freeHashTable(hashTable);

  Fclose(cmdFile);

  return 0;
}
