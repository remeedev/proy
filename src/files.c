#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "headers/extras.h"

int check_or_create_folder(char *path, int silent){
    if (access(path, F_OK) != 0){
        if (!silent)print_lacks_path(path);
        mkdir(path, 0777);
        return 1;
    }
    return 0;
}

char **all_type_in_path(char *path, int type, int *count){
    struct dirent* d;
    DIR *dir;
    struct stat finfo;

    dir = opendir(path);
    if (dir == NULL){
        print_lacks_path(path);
        return NULL;
    }
    int out_count = 0;
    while ((d = readdir(dir)) != NULL){
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;
        char *full_path = (char *)malloc(strlen(path) + strlen(d->d_name) + 2);
        if (full_path == NULL){
            print_error("There has been an error reading a directory!\n");
            return NULL;
        }
        strcpy(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, d->d_name);

        if (stat(full_path, &finfo) != -1){
            int file_type = finfo.st_mode & S_IFMT;
            if (file_type == type){
                out_count++;
            }
        }

        free(full_path);
    }
    rewinddir(dir);
    char **out = (char **)malloc(sizeof(char *)*out_count);
    int i = 0;
    while ((d = readdir(dir)) != NULL){
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;
        char *full_path = (char *)malloc(strlen(path) + strlen(d->d_name) + 2);
        if (full_path == NULL){
            print_error("There has been an error reading a directory!\n");
            return NULL;
        }
        strcpy(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, d->d_name);

        if (stat(full_path, &finfo) != -1){
            int file_type = finfo.st_mode & S_IFMT;
            if (file_type == type){
                out[i++] = strdup(d->d_name);
            }
        }

        free(full_path);
    }
    *count = out_count;
    closedir(dir);
    return out;
}

void free_type_list(char **list, int count){
    for (int i = 0; i < count; i++) free(list[i]);
    free(list);
}

void list_type_in_path(char *path, int type){
    int count = 0;
    char **all = all_type_in_path(path, type, &count);
    for (int i = 0; i < count; i++) printf("%s\n", all[i]);
    free_type_list(all, count);
}

void list_folders_in_path(char *path){
    list_type_in_path(path, S_IFDIR);
}

void list_files_in_path(char *path){
    list_type_in_path(path, S_IFREG);
}

void delete_folder(char *path, int silent){
    struct dirent* d;
    DIR *directory;
    struct stat file_info;

    directory = opendir(path);
    if (directory == NULL){
        printf("Folder to delete (%s) is non existent!\n", path);
        return;
    }
    while ((d = readdir(directory)) != NULL){
        char *full_path = (char *)malloc(strlen(path) + strlen(d->d_name) + 2);
        if (full_path == NULL){
            printf("There has been an error reading a directory!\n");
            return;
        }
        strcpy(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, d->d_name);

        if (stat(full_path, &file_info) != -1){
            int file_type = file_info.st_mode & S_IFMT;
            if (file_type == S_IFREG){
                remove(full_path);
                if (!silent) printf("%s has been deleted!\n", full_path);
            }else if (file_type == S_IFDIR){
                if (strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, ".") == 0) {
                    free(full_path);
                    continue;
                }
                delete_folder(full_path, silent);
            }
        } 
        free(full_path);
    }

    closedir(directory);
    remove(path);
    if (!silent) printf("%s has been deleted!\n", path);
}

void check_and_delete_folder(char *path, int silent){
    if (access(path, F_OK) == 0){
        delete_folder(path, silent);
    }
}

int check_or_create_file(char *path, int silent){
    if (access(path, F_OK) != 0){
        if (!silent) print_lacks_path(path);
        FILE *tmp = fopen(path, "w");
        if (!tmp){
            print_error("There has been an error creating file!\n");
            return 0;
        }
        fclose(tmp);
        return 1;
    }
    return 0;
}

int check_or_create_folder_s(char *path){
    return check_or_create_folder(path, 1);
}

int check_or_create_file_s(char *path){
    return check_or_create_file(path, 1);
}

int check_or_create_folder_free(char *path){
    int out = check_or_create_folder(path, 0);
    free(path);
    return out;
}

int check_or_create_file_free(char *path){
    int out = check_or_create_file(path, 0);
    free(path);
    return out;
}
