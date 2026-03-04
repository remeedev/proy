#include <stdlib.h>
#include <string.h>

#include "headers/extras.h"

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

sll* new_sll(){
    sll *out = (sll*)malloc(sizeof(sll));
    if (out == NULL) {
        mem_alloc_error();
        return NULL;
    }
    out->first = NULL;
    out->last = NULL;
    out->size = 0;
    return out;
}

void sll_append(sll* list, char *key, char *value){ // Set value to NULL for default sll
    kv_pair *new_elem = (kv_pair *)malloc(sizeof(kv_pair));
    if (!new_elem){
        mem_alloc_error();
        return;
    }
    new_elem->key = strdup(key);
    new_elem->value = strdup(value);
    new_elem->next = NULL;
    if (list ->first == NULL){
        list->first = new_elem;
        list->last = new_elem;
    }else {
        list->last->next = new_elem;
        list->last = new_elem;
    }
    list->size++;
}

void free_sll(sll* list){
    kv_pair *_elem = list->first;
    while (_elem != NULL){
        free(_elem->key);
        free(_elem->value);
        kv_pair* to_free = _elem;
        _elem = _elem->next;
        free(to_free);
    }
    free(list);
}
