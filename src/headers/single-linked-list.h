#ifndef single_linked
#define single_linked

typedef struct kv_pair {
    char *key;
    char *value;
    struct kv_pair *next;
} kv_pair;

typedef struct sll {
    kv_pair* first;
    kv_pair* last;
    int size;
} sll;

sll* new_sll();
void sll_append(sll*, char*, char*);
void free_sll(sll *);

#endif
