#ifndef versions
#define versions

void init_project_version(char *project_name);
char *get_project_version(char *project_name);
void add_project_alias(char *project_name, char *version_alias);
void upgrade_sub(char *project_name);
void upgrade_major(char *project_name);
void upgrade_minor(char *project_name);

#endif
