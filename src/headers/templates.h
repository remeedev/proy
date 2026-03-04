#ifndef tmplts
#define tmplts

void generate_template_from_directory(char *template_name, char *directory_path);
void free_template_list(char **tmplt_list, int size);
char **get_template_list(int *size);
void print_template_list();
int check_template_exists(char *template_name);
void delete_template(char *template_name);
int unpack_template_to_dir(char *template_name, char *proy_path);
int compare_template_to_directory(char *template_name, char *directory_path);
char *get_template_from_dir(char *directory_path);

#endif
