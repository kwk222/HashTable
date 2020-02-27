#ifndef HASHTABLE_FUNCS
#define HASHTABLE_FUNCS

#include "global_defs.h"

void     parseArgs(int, char *[]);                /* in: argc, argv */
Node   **createHashTable(Node **, size_t);        /* in: pointer to current hash table (if there is one), number of initial or new buckets; return: pointer to new hash table */
void     copyHashTable(Node **, Node **, size_t); /* in: pointer to new hash table, pointer to current hash table, size of current hash table */
uint32_t hash(uint64_t);                          /* in: 8-byte key; return: 4 byte hash */
int      create(Node **, uint64_t, int);          /* in: pointer to hash table, key, value; return 0 for success and 1 when update occurred  */
int      update(Node **, uint64_t, int);          /* in: pointer to hash table, key, value; return 0 for success and 1 when create occurred  */
int      read(Node **, uint64_t, int *);          /* in: pointer to hash table, key, pointer to value to be updated; return 0 for success and 2 when error */
int      delete(Node **, uint64_t);               /* in: pointer to hash table, key; return 0 for success and 2 when not successful  */
Node   **runHashCommands(Node **, FILE *);        /* in: pointer to hash table, pointer to commands file; return: pointer to new hash table */
void     getHashTableStats(Node **);              /* in: pointer to hash table */
void     printHashTableStats();                   /* no parameters to pass, simply prints out some global variables */
void     freeHashTable(Node **);                  /* in: pointer to hash table */
void    *Malloc(size_t);                          /* in: bytes to allocate, return: void ptr to allocated memory */
FILE    *Fopen(const char *, const char *);       /* in: path/file, mode; return: file ptr */
void     Fclose(FILE *);                          /* in: file ptr; close file */
void     bail(int, const char *);                 /* in: rc, message; exit with rc after printing message to stderr */
void     help();                                  /* print help message to stdout and exit with rc = 0 */

#endif
