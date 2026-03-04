// Not an import list
#ifndef imports
#define imports

void unpack_import(char *fp);
void create_import_from_files(char* import_name, char **files, int file_count);
void import_new(char *import_name);
void remove_import(char *import_name);
void list_modules();
void delete_import(char *import_name);
void which_modules();

#endif
