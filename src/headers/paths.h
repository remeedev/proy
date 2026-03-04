#ifndef paths
#define paths

int setup_paths();
void end_paths();

char *get_home_dir();
char *get_curr_dir();
char *get_data_dir();
char *get_config_dir();

int is_path(char *str);
int is_file(char *str);

char *resolve_path(char *);
char *resolve_path_list(char **, int);
char *get_project_path(char *project_name);
int project_exists(char *project_name);
char *get_project_name(char *path);
char *get_path_relative_to_proy(char *path);
int is_project(char *path);
char *expand_path(char *prepended, const char *path);

#endif
