#include "../src/headers/hash_map.h"
#include "helper/helper.h"
#include <stdio.h>

void setup_tests(test_suite *t_s){
    int i = 0;
    t_s->test_obj = hash_append(t_s->test_obj, "A", "Testing");
    t_s->test_obj = hash_append(t_s->test_obj, "B", "Bailarina");
    t_s->test_obj = hash_append(t_s->test_obj, "C", "Chocolate");
    t_s->test_obj = hash_append(t_s->test_obj, "D", "Distance");
    t_s->test_obj = hash_append(t_s->test_obj, "Longer", "Testing");
    t_s->test_obj = hash_int_append(t_s->test_obj, "Number", 18);
}

int all_value_check(void *_){
    hash *table = (hash *)_;
    int res = 1;
    res &= cmp_eq_str(hash_get(table, "A"), "Testing");
    res &= cmp_eq_str(hash_get(table, "B"), "Bailarina");
    res &= cmp_eq_str(hash_get(table, "C"), "Chocolate");
    res &= cmp_eq_str(hash_get(table, "D"), "Distance");
    res &= cmp_eq_str(hash_get(table, "Longer"), "Testing");
    return res;
}

int test_capacity_bigger_than_11(void *table){
    return cmp_eq_int(((hash *)table)->capacity, 22);
}

int test_number_saved(void *table){
    return cmp_eq_int(hash_int_get(table, "Number"), 18);
}

int test_size(void *_){
    hash * table = (hash *)_;
    return cmp_eq_int(table->size, 6);
}

int extra_test(void *_){
    hash *table = (hash *)_;
    table = hash_append(table, "majo", "morado");
    hash_remove(table, "A");
    int res = cmp_eq_str(hash_get(table, "majo"), "morado");
    hash_remove(table, "majo");
    res &= check_null(hash_get(table, "majo"));
    res &= check_null(hash_get(table, "A"));
    return res;
}

int test_save(void *_){
    hash *table = (hash *)_;
    hash_save(table, "./saved.conf");
    hash *new_table = hash_read(NULL, "./saved.conf");
    int res = equal_hash(new_table, table);

    new_table = hash_append(new_table, "Empty", "new");
    res &= equal_hash(new_table, table) == 0;

    hash_free(new_table);
    if (remove("./saved.conf") == 0){
        printf("Cleaned up temporary test files!\n");
    }
    return res;
}

int test_weird_formatting(void *_){
    FILE *file = fopen("./tests/test.conf", "w");
    if (file == NULL){
        printf("There has been an error opening to write the test.conf file!\n");
        return 0;
    }
    fprintf(file, "number:18\n\n\nquestion:32\n\n--This is a comment\n\n\n\nExtra:extra\n\n\n\nA:B\n\n\n");
    fclose(file);
    hash *test_table = hash_read(NULL, "./tests/test.conf");
    int res = cmp_eq_int(hash_int_get(test_table, "number"), 18);
    res &= cmp_eq_int(hash_int_get(test_table, "question"), 32);
    res &= cmp_eq_str(hash_get(test_table, "Extra"), "extra");
    res &= cmp_eq_str(hash_get(test_table, "A"), "B");
    if (remove("./tests/test.conf") == 0){
        printf("Cleaned up temporary test file!\n");
    }
    hash_free(test_table);
    return res;
}

int test_table_merge(void *_){
    hash *table1 = create_hash_map(11);
    table1 = hash_append(table1, "only", "testing");
    int res = cmp_eq_str(hash_get(table1, "only"), "testing"); // Sanity check
    hash *table2 = (hash *)_;
    table1 = hash_merge_missing_keys(table2, table1);
    res &= cmp_eq_str(hash_get(table1, "C"), "Chocolate");
    hash_free(table1);
    return res;
}

int main(int argc, char *argv[]){
    test_suite *HMTS = new_test_suite(create_hash_map(11));
    setup_tests(HMTS);

    add_test(HMTS, "Expanded capacity", &test_capacity_bigger_than_11);
    add_test(HMTS, "Number Saved", &test_number_saved);
    add_test(HMTS, "Check all values", &all_value_check);
    add_test(HMTS, "Check for correct size", &test_size);
    add_test(HMTS, "Check collisions", &extra_test);
    add_test(HMTS, "Check save hash table", &test_save);
    add_test(HMTS, "Check custom made config", &test_weird_formatting);
    add_test(HMTS, "Check table merge", &test_table_merge);

    run_n_dump_suite(HMTS);

    hash_free(HMTS->test_obj);
    free_test_suite(HMTS);
    return 0;
}
