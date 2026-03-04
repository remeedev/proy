#include <stdio.h>

#define RED "\033[31m"
#define BOLD "\033[1m"
#define END_COLOR "\033[0m"

void print_command_overview(char *command_name, char *description){
    printf("\t%s%s%s\n\t\t%s\n",BOLD, command_name,END_COLOR, description);
}

void print_command_ov_usage(char *command_name, char *description, char *command_usage){
    print_command_overview(command_name,description);
    printf("\t\tUsage:\n\t\t\t%s\n", command_usage);
}

void print_prev_declare(char *prev){
    printf("The following commands, to run, require to be started by the following:\n\t%s%s%s\n\n", BOLD, prev, END_COLOR);
}

void bprint(char *a){
    printf("%s%s%s", BOLD, a, END_COLOR);
}

void print_proy_has_no_version(char *project_name){
    printf("This project does not have a version!\n");
    printf("Do 'proy track %s' to start tracking it!\n", project_name);
}

void must_be_project(){
    printf("You must be inside of a project to do that!\n");
}

void print_module_does_not_exist(char *module_name){
    printf("Module %s'%s'%s does not exist!\n", BOLD, module_name, END_COLOR);
}

void print_module_not_imported(char *module_name){
    printf("Module %s'%s'%s has not been imported to this project!\n", BOLD, module_name, END_COLOR);
}

void print_module_already_imported(char *module_name){
    printf("Module %s'%s'%s has already been imported to this project!\n", BOLD, module_name, END_COLOR);
}

void print_index_elem(int index, char *elem){
    printf("%s[%d]%s %s\n", BOLD, index, END_COLOR, elem);
}

void print_template_does_not_exist(char *tmplt_name){
    printf("The template %s%s%s does not exist!\n", BOLD, tmplt_name, END_COLOR);
}

void print_project_already_starred(char *proy){
    printf("%s%s%s is an already starred project!\n", BOLD, proy, END_COLOR);
}

void print_project_already_exists(char *proy){
    printf("A project with the name %s%s%s already exists!\n", BOLD, proy, END_COLOR);
}

void print_project_does_not_exist(char *proy){
    printf("The project %s%s%s does %snot%s exist!\n", BOLD, proy, END_COLOR, BOLD, END_COLOR);
}

void print_incorrect_usage(char *usage){
    printf("Incorrect command usage!\n");
    printf("\tCommand usage:\n");
    printf("\t\t'%s'\n", usage);
    printf("Or run 'proy help' to get help.\n");
}

void print_command_not_recognized(char *cmd){
    printf("Command '%s' not recognized.\n", cmd);
    printf("Run 'proy help' to get a list of all commands!\n");
}

void print_error(char *error){
    printf("%s[Error] %s%s\n", RED, error, END_COLOR);
}

void print_lacks_path(char *path){
    printf("%s does not exist!\n", path);
}

void mem_error(char *type){
    char err_msg[256];
    sprintf(err_msg, "There has been an error %sing memory!\n", type);
    print_error(err_msg);
}

void mem_alloc_error(){
    mem_error("allocat");
}

void mem_realloc_error(){
    mem_error("reallocat");
}
