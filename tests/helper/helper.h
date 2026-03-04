#ifndef helper
#define helper

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

test_suite* new_test_suite(void *obj_tested);
void add_test(test_suite* suite, char*test_name,int (*func)(void *));
void run_test(test_suite * suite, test *to_test);
void print_suite_summary(test_suite *suite);
void run_all_tests(test_suite *suite);
void free_test_suite(test_suite *suite);
void run_n_dump_suite(test_suite *suite);

int cmp_eq_int(int a, int b);
int cmp_eq_str(char *a, char *b);
int cmp_lt_int(int a, int b);
int cmp_bt_int(int a, int b);
int check_null(char *);

#endif
