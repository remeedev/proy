#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../../src/headers/extras.h"

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define SPECIAL "\x1b[35m"

#define END "\x1b[0m"

typedef struct test {
    char *name;
    int passed; // 0 or 1
    int (*test_func)(void *);
    struct test *next_test;
} test;

typedef struct test_suite {
    void *test_obj; // pointer to any type
    test *first_test;
    int passed;
    int total;
} test_suite;

test_suite* new_test_suite(void *obj_tested){
    printf("Creating new test suite...\n");
    test_suite *out = (test_suite *)malloc(sizeof(test_suite));
    if (out == NULL) {
        mem_alloc_error();
        return NULL;
    }

    out->first_test = NULL;
    out->passed = 0;
    out->total = 0;
    out->test_obj = obj_tested;
    printf("Created and loaded a test suite!\n");
    return out;
}

void add_test(test_suite* suite, char*test_name,int (*func)(void *)){
    test *new_test = (test *)malloc(sizeof(test));
    if (new_test == NULL){
        mem_alloc_error();
        return;
    }
    new_test->next_test = NULL;
    new_test->passed = -1;
    new_test->name = strdup(test_name);
    new_test->test_func = func;
    if (suite->first_test == NULL) {
        suite->first_test = new_test;
    }else{
        test *tmp_test = suite->first_test;
        while (tmp_test->next_test != NULL) tmp_test = tmp_test->next_test;
        tmp_test->next_test = new_test;
    }
}

void lb(int w){
    for (int i = 0; i < w; i++) printf("=");
    printf("\n");
}

void run_test(test_suite * suite, test *to_test){
    printf("\n\n");
    lb(25);
    printf("Running %s test...\n", to_test->name);
    if (to_test->test_func(suite->test_obj)){
        suite->passed++;
        to_test->passed = 1;
        printf("%s[PASSED]%s %s test has passed!\n", GREEN, END, to_test->name);
    } else {
        to_test->passed = 0;
        printf("%s[FAILED]%s %s test has failed! Further information above...\n", RED, END, to_test->name);
    }
    suite->total++;
    lb(25);
    printf("\n");
}

void print_suite_summary(test_suite *suite){
    printf("%d tests have been ran!\n", suite->total);
    printf("%s[PASSED]%s %d\n", GREEN, END, suite->passed);
    printf("%s[FAILED]%s %d\n\n", RED, END, suite->total - suite->passed);

    if (suite->total == suite->passed) return;
    printf("%sFailed tests:\n", RED);
    test *curr_test = suite->first_test;
    while (curr_test != NULL) {
        if (curr_test->passed == 0){
            printf("%s\n", curr_test->name);
        }
        curr_test = curr_test->next_test;
    }
    printf("%s", END);
}

void run_all_tests(test_suite *suite){
    test*curr_test = suite->first_test;
    while (curr_test){
        run_test(suite, curr_test);
        curr_test = curr_test->next_test;
    }
}

void free_test_suite(test_suite *suite){
    test *_test = suite->first_test;
    while (_test){
        if (_test->name) free(_test->name);
        test *to_del = _test;
        _test = _test->next_test;
        free(to_del);
    }
    free(suite);
}

void run_n_dump_suite(test_suite *suite){
    run_all_tests(suite);
    print_suite_summary(suite);
}

int cmp_eq_int(int a, int b){
    if (a != b){
        printf("Expected %s%d%s but got %s%d%s\n", SPECIAL, b, END, SPECIAL, a, END);
    }
    return a == b;
}

int cmp_eq_str(char *a, char *b){
    if (!a && b != NULL) {
        printf("Expected %s%s%s but got NULL\n", SPECIAL, b, END);
        return 0;
    }
    int res = strcmp(a, b) == 0;
    if (!res){
        printf("Expected %s%s%s but got %s%s%s\n", SPECIAL, b, END, SPECIAL, a, END);
    }
    return res;
}

int cmp_lt_int(int a, int b){
    if (a >= b){
        printf("Expected value less than %s%d%s, but got %s%d%s\n", SPECIAL, b, END, SPECIAL, a, END);
    }
    return a < b;
}

int cmp_bt_int(int a, int b){
    if (a <= b) {
        printf("Expected value bigger than %s%d%s, but got %s%d%s\n", SPECIAL, b, END, SPECIAL, a, END);
    }
    return a > b;
}

int check_null(char *_){
    if (_ == NULL) return 1;
    printf("Expected %sNULL%s, but got %s%s%s instead\n", SPECIAL, END, SPECIAL, _, END);
    return 0;
}
