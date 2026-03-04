#define _GNU_SOURCE

#include "headers/extras.h"
#include "headers/config.h"
#include "headers/files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HOME_CHAR '~'
#define DATA_CHAR '$'
#define CONFIG_CHAR '+'
#define CURR_CHAR '.'

char *home_dir = NULL;
char *data_dir = NULL;
char *config_dir = NULL;
char *curr_dir = NULL;

char *get_data_dir();
char *get_config_dir();

int setup_paths(){
    home_dir = getenv("HOME");   
    char *data_ext = "/.local/share/proy";
    data_dir = (char *)malloc(strlen(home_dir) + strlen(data_ext) + 1);
    if (data_dir == NULL){
        print_error("There has been an error allocating space for data directory name!\n");
        goto curr_dir_setup;
    }
    strcpy(data_dir, home_dir);
    strcat(data_dir, data_ext);
curr_dir_setup:
    curr_dir = get_current_dir_name();
    char *config_ext = "/.config/proy";
    config_dir = (char *)malloc(strlen(home_dir) + strlen(config_ext) + 1);
    if (config_dir == NULL){
        print_error("There has been an error allocating space for config directory name!\n");
        return 1;
    }
    strcpy(config_dir, home_dir);
    strcat(config_dir, config_ext);
    int new_install = check_or_create_folder(get_config_dir(), 1);
    new_install &= check_or_create_folder(get_data_dir(), 1);
    if (new_install){
        printf("Welcome to PROY, a project management application!\n");
        printf("Missing files are about to be created...\n");
    }
    return new_install; 
}

char *get_home_dir(){
    return home_dir;
}

char *get_data_dir(){
    return data_dir;
}

char *get_curr_dir(){
    return curr_dir;
}

char *get_config_dir(){
    return config_dir;
}

void end_paths(){
    if (data_dir) free(data_dir);
    if (curr_dir) free(curr_dir);
    if (config_dir) free(config_dir);
    data_dir = NULL;
    curr_dir = NULL;
    config_dir = NULL;
}

char *resolve_path(char *path){
    if (path == NULL) return path;

    int size = strlen(path); // Should be - 1 if not all characters are inside search_area
    if  (path[0] == HOME_CHAR){
        size += strlen(get_home_dir());
    }else if (path[0] == DATA_CHAR){
        size += strlen(get_data_dir());
    }else if (path[0] == CURR_CHAR){
        size += strlen(get_curr_dir());
    }else if (path[0] == CONFIG_CHAR){
        size += strlen(get_config_dir());
    }else {
        size++;
    }
    // Doesnt need outside size++...
    char *out = (char *)malloc(size);
    if (out == NULL){
        print_error("Unable to allocate memory for resolved path!\n");
        return NULL;
    }
    out[0] = '\0';
    int out_pos = 0;
    for (int i = 0; path[i] != '\0' && out_pos < size; i++){
        if (i == 0 && (path[i] == HOME_CHAR ||\
                    path[i] == DATA_CHAR ||\
                    path[i] == CURR_CHAR ||\
                    path[i] == CONFIG_CHAR)){
            if (path[i] == HOME_CHAR){
                for (int j = 0; get_home_dir()[j] != '\0'; j++) out[out_pos++] = get_home_dir()[j];
            }else if (path[i] == DATA_CHAR){
                for (int j = 0; get_data_dir()[j] != '\0'; j++) out[out_pos++] = get_data_dir()[j];
            }else if (path[i] == CURR_CHAR){
                for (int j = 0; get_curr_dir()[j] != '\0'; j++) out[out_pos++] = get_curr_dir()[j];
            }else if (path[i] == CONFIG_CHAR){
                for (int j = 0; get_config_dir()[j] != '\0'; j++) out[out_pos++] = get_config_dir()[j];
            }
        }else {
            out[out_pos++] = path[i];
        }
    }
    out[out_pos] = '\0';
    return out;
}

// concats paths one with each other, then resolves the path
char *resolve_path_list(char **path_list, int path_count){
    int net_size = 0;
    for (int i = 0; i < path_count; i++){
        net_size += strlen(path_list[i]);
    }
    net_size++;
    char *full_path = (char *)malloc(net_size);
    if (!full_path) {
        mem_alloc_error();
        return NULL;
    }
    full_path[0] = '\0';
    for (int i = 0; i < path_count; i++) strcat(full_path, path_list[i]);
    char *out = resolve_path(full_path);
    free(full_path);
    return out;
}

char *get_project_path(char *project_name){
    char *path_list[3] = {config_get("project_path"), "/", project_name};
    char *fpp = resolve_path_list(path_list, 3);
    return fpp;
}

int project_exists(char *project_name){
    char *project_path = get_project_path(project_name);
    int exists = access(project_path, F_OK) == 0;
    free(project_path);
    return exists;
}

int is_project(char *path){
    char *resolved = resolve_path(path);
    char *proy_paths = resolve_path(config_get("project_path"));
    int i = 0;
    while (resolved[i] != '\0' && proy_paths[i] != '\0') {
        if (resolved[i] != proy_paths[i]){
            free(resolved);
            free(proy_paths);
            return 0;
        }
        i++;
    }
    int out = proy_paths[i] == '\0' && resolved[i] == '/'; 
    free(resolved);
    free(proy_paths);
    return out;
}

char *get_project_name(char *path){
    char *resolved = resolve_path(path);
    char *proy_paths = resolve_path(config_get("project_path"));
    int i = 0;
    while (resolved[i] != '\0' && proy_paths[i] != '\0') {
        if (resolved[i] != proy_paths[i]){
            free(resolved);
            free(proy_paths);
            return NULL;
        }
        i++;
    }
    int out = proy_paths[i] == '\0' && resolved[i] == '/';
    if (out == 0) {
        free(resolved);
        free(proy_paths);
        return NULL;
    }
    i++;
    int size = 0;
    while (resolved[i + size] != '/' && resolved[i + size] != '\0') size++;
    char *proy_name = (char *)malloc(size + 1);
    if (proy_name == NULL){
        mem_alloc_error();
        free(resolved);
        free(proy_paths);
        return NULL;
    }
    for (int j = 0; j < size; j++){
        proy_name[j] = resolved[i + j];
    }
    proy_name[size] = '\0';
    free(resolved);
    free(proy_paths);
    return proy_name;
}

char *get_path_relative_to_proy(char *path){
    char *resolved = NULL;
    if (path[0] != '.' && path[0] != '/'){
        char *path_list[2] = {"./", path};
        resolved = resolve_path_list(path_list, 2);
    }else{
        resolved = resolve_path(path);
    }
    char *proy_paths = resolve_path(config_get("project_path"));
    int i = 0;
    while (resolved[i] != '\0' && proy_paths[i] != '\0') {
        if (resolved[i] != proy_paths[i]){
            free(resolved);
            free(proy_paths);
            return NULL;
        }
        i++;
    }
    int out = proy_paths[i] == '\0' && resolved[i] == '/';
    if (out == 0) {
        free(resolved);
        free(proy_paths);
        return NULL;
    }
    i++;
    while (resolved[i] != '/' && resolved[i] != '\0') i++;
    i++; // Skips project name
    int size = 0;
    while (resolved[i + size] != '\0') size++;
    char *relative_path= (char *)malloc(size + 3);
    if (relative_path== NULL){
        mem_alloc_error();
        free(resolved);
        free(proy_paths);
        return NULL;
    }
    relative_path[0] = '.';
    relative_path[1] = '/';
    for (int j = 2; j < size + 2; j++){
        relative_path[j] = resolved[i + j - 2];
    }
    relative_path[size + 2] = '\0';
    free(resolved);
    free(proy_paths);
    return relative_path;
}

// must be freed
char *expand_path(char *prepended, const char *path){
    if (prepended == NULL && path==NULL) return NULL;
    char *full_path = NULL;
    int size = 0;
    if (prepended != NULL && path[0] == '.') size = strlen(prepended) + strlen(path);
    if (size == 0) return strdup(path);
    full_path = (char *)malloc(size);
    int ppend_size = strlen(prepended);
    if (full_path == NULL){
        mem_alloc_error();
        return NULL;
    }
    strcpy(full_path, prepended);
    for (int i = 1; path[i] != '\0'; i++) full_path[ppend_size + i - 1] = path[i];
    full_path[size - 1] = '\0';
    return full_path;
}

int is_path(char *str){
    int size = strlen(str);
    for (int i = 0; i < size; i++){
        if (i < 2){
            if (str[i] != "./"[i] && (i == 0 ? str[i] != '/' : 0)) return 0;
        }
    }
    return 1;
}

int is_file(char *str){
    if (!is_path(str)) return 0;
    int i = strlen(str) - 1;
    while (i >= 0 && str[i] != '/') {
        if (str[i] == '.') return 1;
        i--;
    }
    return 0;
}
