#include <stdlib.h>
#include <stdio.h>

#include "helper/helper.h"
#include "../src/headers/templates.h"
#include "../src/headers/config.h"
#include "../src/headers/paths.h"

int compression_test(void *_){
    generate_template_from_directory("test_template", "./tests/example_template");
    int res = 1;
    res &= cmp_eq_int(check_template_exists("test_template"), 1);
    delete_template("test_template");
    res &= cmp_eq_int(check_template_exists("test_template"), 0);
    return 1;
}

int main(int argc, char *argv[]){
    test_suite* TMPL_T = new_test_suite(NULL);

    setup_paths();
    setup_config();
    printf("Config has been setup");
    
    add_test(TMPL_T, "Template compression", &compression_test);

    run_n_dump_suite(TMPL_T);

    end_config();
    end_paths();
    free_test_suite(TMPL_T);
    return 0;
}
