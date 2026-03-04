#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define EQUAL_SIGN '='

#include "headers/single-linked-list.h"
#include "headers/extras.h"

#define BIG_PRIME 2147483647
#define random_float 0.69

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

int hash_key(hash* table, char *key){
    int key_transposed = 0;
    for (int i = 0; key[i] != '\0'; i++){
        key_transposed = key_transposed + ((int) key[i]) * (i + 1);
    }
    double r = key_transposed*random_float;
    double int_part, fract_part;
    fract_part = modf(r, &int_part);
    int out = floor( (double)BIG_PRIME*fract_part ); 

    out = out%table->capacity;
    return out;
}

hash* create_hash_map(int capacity){
    hash *out = (hash *)malloc(sizeof(hash));
    if (out == NULL){
        mem_alloc_error();
        return NULL;
    }
    out->capacity = capacity;
    out->size = 0;
    out->load_capacity = 0.5;
    out->load_factor = 0.0;
    out->elems = (hash_elem **)malloc(sizeof(void *)*capacity);
    if (out->elems == NULL){
        mem_alloc_error();
        free(out);
        return NULL;
    }
    for (int i = 0; i < capacity; i++){
        out->elems[i] = NULL;
    }
    return out;
}

void hash_free(hash *table){
    for (int i = 0; i < table->capacity; i++){
        if (table->elems[i] != NULL){
            if (table->elems[i]->key){
                free(table->elems[i]->key);
            }
            if (table->elems[i]->value) {
                free(table->elems[i]->value);
            }
            free(table->elems[i]);
        }
    }
    free(table->elems);
    free(table);
}

sll* get_key_value_pairs(hash* );
hash* hash_append(hash*, char *, char *);

hash* expand_table(hash *table){
    hash *new_table = create_hash_map(table->capacity<<1);
    sll* elements = get_key_value_pairs(table);
    
    kv_pair* f_elem = elements->first;
    while (f_elem != NULL){
        new_table = hash_append(new_table, f_elem->key, f_elem->value);
        f_elem = f_elem->next;
    }

    hash_free(table);
    free_sll(elements);

    return new_table;
}

hash* hash_append(hash* table, char *key, char *value){
    int pos = hash_key(table, key);

    // Handle possible collision
    while (table->elems[pos] != NULL && table->elems[pos]->key != NULL\
            && strcmp(table->elems[pos]->key, key)!=0){
        pos++;
        if (pos >= table->capacity) pos = 0; // The way its built it should ALWAYS have space
    }
    if (table->elems[pos] == NULL){
        table->elems[pos] = (hash_elem *)malloc(sizeof(hash_elem));
        if (table->elems[pos] == NULL){
            mem_alloc_error();
            return table;
        }
        table->elems[pos]->key = NULL;
        table->elems[pos]->value = NULL;
    }
    if (table->elems[pos]->key == NULL) table->elems[pos]->key = strdup(key);
    if (table->elems[pos]->value != NULL) free(table->elems[pos]->value);
    table->elems[pos]->value = strdup(value);
    table->size++;
    table->load_factor = (float)table->size / (float)table->capacity;
    if (table->load_factor > table->load_capacity){
        table = expand_table(table);
    }
    return table;
}

char* hash_get(hash *table, char *key){
    int pos = hash_key(table, key);

    while (table->elems[pos] != NULL && ( table->elems[pos]->key == NULL || strcmp(table->elems[pos]->key, key) != 0 )) pos++;
    if (table->elems[pos] == NULL) return NULL;
    return table->elems[pos]->value;
}

void hash_remove(hash* table ,char *key){
    int pos = hash_key(table, key);

    while (table->elems[pos] != NULL && ( table->elems[pos]->key == NULL || strcmp(table->elems[pos]->key, key) != 0 )) pos++;
    if (table->elems[pos] == NULL) return;
    if (table->elems[pos]->key) free(table->elems[pos]->key);
    if (table->elems[pos]->value) free(table->elems[pos]->value);
    table->elems[pos]->key = NULL;
    table->elems[pos]->value = NULL;
}

sll* get_key_value_pairs(hash* table){
    sll *out = new_sll();
    for (int i = 0; i < table->capacity; i++){
        if (table->elems[i] != NULL && table->elems[i]->key != NULL){
            sll_append(out, table->elems[i]->key, table->elems[i]->value);
        }
    }
    return out;
}

int hash_int_get(hash *table, char* key){
    char *val = hash_get(table, key);
    if (!val) return 0;
    int out = 0;
    for (int i = 0; val[i] != '\0'; i++){
        if (val[i] < '0' || val[i] > '9') return 0;
        out += val[i] - '0';
        if (val[i + 1] != '\0')out *= 10;
    }
    return out;
}

char *int_to_str(int num){
    int num_len = 0;
    int num_copy = abs(num);
    while (num_copy > 0){
        num_copy/=10;
        num_len++;
    }
    num_len++;
    if (num_len == 1) num_len++; // Handles a 0
    if (num < 0) num_len++;
    char *value_str = (char *)malloc(num_len);
    if (value_str == NULL){
        mem_alloc_error();
        return NULL;
    }
    sprintf(value_str, "%d", num);
    return value_str;
}

hash *hash_int_append(hash *table, char *key, int value){
    char *value_str = int_to_str(value);
    if (value_str == NULL) return table;
    hash* out = hash_append(table, key, value_str);
    free(value_str);
    return out;
}

void hash_save(hash* table, char *file_name){
    if (table == NULL) return;
    sll* kv_pairs = get_key_value_pairs(table);

    FILE *file = fopen(file_name, "w");

    kv_pair *c_pair = kv_pairs->first;
    while (c_pair){
        fprintf(file, "%s%c%s\n", c_pair->key, EQUAL_SIGN, c_pair->value);
        c_pair = c_pair->next;
    }

    fclose(file);

    free_sll(kv_pairs);
}

char *get_hash_line(FILE *file){
    char at = 'a';
    int size = 0;
    while (size == 0 && at != EOF) while ((at = fgetc(file)) != EOF && at != '\n') {
        size++;
    }
    if (size == 0 && at == EOF) return NULL;
    size++;
    char  *line_text = (char *)malloc(size + 1);
    if (line_text == NULL){
        mem_alloc_error();
        return NULL;
    }

    fseek(file, -size, SEEK_CUR);
    if (fgetc(file) != '\n') fseek(file, -1, SEEK_CUR); // De vez en cuando se para en el lugar erroneo
    int i = 0;
    while ((at = fgetc(file)) != EOF && at != '\n') line_text[i++] = at;
    line_text[i++] = '\0';
    return line_text;
}

hash *hash_read(hash* table, char *file_name){
    FILE *file = fopen(file_name, "r");
    if (file == NULL) return table;
    if (table == NULL) table = create_hash_map(11);

    int at;
    char *curr_line;
    while ((curr_line = get_hash_line(file)) != NULL){
        if (strlen(curr_line) == 0) {
            free(curr_line);
            continue;
        }
        if (curr_line[0] == '-') {
            free(curr_line);
            continue; // comment
        }
        char *key = curr_line;
        int pos = 0;
        while (curr_line[pos] != EQUAL_SIGN && curr_line[pos] != '\0') pos++;
        if (curr_line[pos] == '\0' && pos != 0) {
            printf("%s\n", curr_line);
            print_error("^^ Malformatted hash table!\n");
            free(curr_line);
            continue;
        }
        curr_line[pos] = '\0';
        char *value = curr_line + pos + 1;
        table = hash_append(table, key, value);
        free(curr_line);
    }

    fclose(file);
    return table;
}

// Returns if hash table A's keys and values are the same as B
int check_sub_hash(hash* a, hash* b){
    sll *kv_1 = get_key_value_pairs(a);

    kv_pair *c_k = kv_1->first;
    while (c_k != NULL){
        char *val = hash_get(b, c_k->key);
        if (val == NULL && c_k->value != NULL) {
            free_sll(kv_1);
            return 0;
        }
        if (strcmp(c_k->value, val) != 0){
            free_sll(kv_1);
            return 0;
        }
        c_k = c_k->next;
    }
    free_sll(kv_1);
    return 1;
}

// Checks if two hash tables are the same
int equal_hash(hash *a, hash *b){
    return check_sub_hash(a, b) && check_sub_hash(b, a);
}

// Gives target the keys that src has that target doesn't
// If target == NULL a copy of src will be created
hash* hash_merge_missing_keys(hash* src, hash* target){ 
    if (src == NULL)return NULL;
    if (target == NULL) target = create_hash_map(11);
    sll* kv_pairs = get_key_value_pairs(src);

    kv_pair *first = kv_pairs->first;
    while (first){
        if (hash_get(target,  first->key) == NULL){
            target = hash_append(target, first->key, first->value);
        }
        first = first->next;
    }
    free_sll(kv_pairs);
    return target;
}
