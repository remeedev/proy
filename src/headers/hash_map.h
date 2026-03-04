#include "single-linked-list.h"

#ifndef hash_table
#define hash_table

typedef struct hash_elem {
    char *key;
    char *value;
} hash_elem;

typedef struct hash {
    int capacity;
    int size;
    float load_capacity;
    float load_factor;
    hash_elem** elems;
} hash;

hash* create_hash_map(int capacity);
void hash_free(hash *table);
hash* hash_append(hash* table, char *key, char *value);
void hash_remove(hash *, char *);
char *hash_get(hash *, char *);
int hash_key(hash* table, char *key);

sll* get_key_value_pairs(hash* table);

int hash_int_get(hash *, char *);
hash *hash_int_append(hash *table, char *key, int value);
void hash_save(hash *, char *);
hash *hash_read(hash *, char *);

int equal_hash(hash *a, hash *b);
hash* hash_merge_missing_keys(hash* src, hash* target);

#endif
