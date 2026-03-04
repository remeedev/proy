#include "extras.h"

#ifndef files
#define files

char **all_type_in_path(char *path, int type, int *count);
void free_type_list(char **list, int count);

void list_folders_in_path(char *path);
void list_files_in_path(char *path);

int check_or_create_folder(char *path, int silent);
int check_or_create_file(char *path, int silent);

void check_and_delete_folder(char *path, int silent);

int check_or_create_folder_s(char *path);
int check_or_create_file_s(char *path);

int check_or_create_folder_free(char *path);
int check_or_create_file_free(char *path);

#endif

