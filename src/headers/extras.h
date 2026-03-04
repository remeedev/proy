#ifndef extras
#define extras

void print_index_elem(int index, char *elem);
void print_command_overview(char *command_name, char *description);
void print_command_ov_usage(char *command_name, char *description, char *command_usage);
void print_prev_declare(char *prev);
void bprint(char *);
void must_be_project();
void print_proy_has_no_version(char *project_name);
void print_template_does_not_exist(char *tmplt_name);
void print_module_does_not_exist(char *module_name);
void print_module_already_imported(char *module_name);
void print_module_not_imported(char *module_name);
void print_project_does_not_exist(char *proy);
void print_project_already_exists(char *proy);
void print_project_already_starred(char *proy);
void print_command_not_recognized(char *cmd);
void print_incorrect_usage(char *usage);
void print_error(char *error);
void print_lacks_path(char *path);
void mem_alloc_error();
void mem_realloc_error();

#endif
