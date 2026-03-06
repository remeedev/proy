#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "headers/extras.h"
#include "headers/paths.h"
#include "headers/config.h"
#include "headers/files.h"

typedef struct import{
    char **file_names;
    char **file_contents;
    int file_count;
}import;

void import_new(char *import_name);

char *get_import_path(char *import_name){
    char *import_list[3] = {config_get("import_dir"), "/", import_name};
    char *out = resolve_path_list(import_list, 3);
    return out;
}

void unpack_import(char *fp){
    FILE *file = fopen(fp, "r");
    if (file == NULL){
        print_error("There has been an error opening the file.");
        printf("%s [%s]\n", strerror(errno), fp);
        return;
    }

    char *proy_name = get_project_name(".");
    char *proy_path = get_project_path(proy_name);
    free(proy_name);
    char buffer[1024];

    FILE *curr_file = NULL;
    while (fgets(buffer, sizeof(buffer), file) != NULL){
        if (buffer[0] == '!') {
            buffer[strlen(buffer) - 1] = '\0'; // remove trailing \n
            char *import_path = get_import_path(&buffer[1]);
            unpack_import(import_path);
            free(import_path);
            continue;
        }
        if (!is_path(buffer)){
            if (curr_file == NULL){
                print_error("Badly formatted import template!\n");
                fclose(file);
                free(proy_path);
                return;
            }
            fprintf(curr_file, "%s", buffer);
        }else{
            if (curr_file != NULL) fclose(curr_file);
            curr_file = NULL;

            int size = strlen(buffer);
            char *file_name= (char *)malloc(size);
            if (file_name== NULL){
                mem_alloc_error();
                fclose(file);
                free(proy_path);
                return;
            }
            for (int i = 0; i < size; i++)file_name[i] = buffer[i];
            file_name[size - 1] = '\0';
            char *file_path = expand_path(proy_path, file_name);
            curr_file = fopen(file_path, "w");

            if (curr_file == NULL){
                print_error("Possibly project doesn't follow template structure.");
                printf("%s [%s]\n", strerror(errno), file_path);
                fclose(file);
                free(proy_path);
                return;
            }

            free(file_path);
            free(file_name);
        }
    }
    free(proy_path);
    if (curr_file != NULL) fclose(curr_file);
    fclose(file);
}



int import_exists(char *import_name){
    char *import_path = get_import_path(import_name);

    int out = access(import_path, F_OK) == 0;
    free(import_path);
    return out;
}

char **get_import_paths(char *import_name, int *size){
    if (!import_exists(import_name)){
        *size = 0;
        return NULL;
    }
    char *import_path = get_import_path(import_name);
    int count = 0;

    FILE *import_file = fopen(import_path, "r");
    if (import_file == NULL){
        print_error("There has been an error opening the import path!\n");
        free(import_path);
        return NULL;
    }
    char buffer[1024];
    while (fgets(buffer, 1024, import_file) != NULL){
        if (is_path(buffer) || buffer[0] == '!') count++;
    }
    rewind(import_file);
    char **out = (char **)malloc(sizeof(char *)*count);
    int i = 0;

    while (fgets(buffer, 1024, import_file) != NULL){
        if (!is_path(buffer) && buffer[0] != '!'){
            continue;
        }
        int buff_size = strlen(buffer);
        out[i] = (char *)malloc(buff_size);
        if (out[i] == NULL){
            mem_alloc_error();
            fclose(import_file);
            for (int j = 0; j < i; j++) free(out[j]);
            free(out);
            return NULL;
        }
        for (int j = 0; j < buff_size; j++) out[i][j] = buffer[j];
        out[i][buff_size - 1] = '\0';
        i++;
    }

    free(import_path);
    fclose(import_file);
    *size = count;
    return out;
}

void free_path_list(char **path_list, int path_count){
    for (int i = 0; i < path_count; i++) free(path_list[i]);
    free(path_list);
}

int check_if_can_import(char *module_name){
    int count = 0;
    char **path_list = get_import_paths(module_name, &count);
    if (path_list == NULL){
        mem_alloc_error();
        free_path_list(path_list, count);
        return 0;
    }
    char *proy_name = get_project_name(".");
    char *proy_path = get_project_path(proy_name);
    free(proy_name);

    for (int i = 0; i < count;  i++){
        if (path_list[i][0] == '!'){
            if (!check_if_can_import(&path_list[i][1])){
                return 0;
            }
            continue;
        }
        int j = strlen(path_list[i]) - 1;
        while (j > 0 && path_list[i][j] != '/') j--;
        path_list[i][j] = '\0';
        char *full_path = expand_path(proy_path, path_list[i]);
        if (access(full_path, F_OK) != 0){
            free_path_list(path_list, count);
            free(full_path);
            return 0;
        }
        free(full_path);
    }
    free(proy_path);

    free_path_list(path_list, count);

    return 1;
}

int check_if_imported(char *module_name){
    int count = 0;
    char **path_list = get_import_paths(module_name, &count);
    if (path_list == NULL){
        mem_alloc_error();
        free_path_list(path_list, count);
        return 0;
    }

    char *proy_name = get_project_name(".");
    char *proy_path = get_project_path(proy_name);
    free(proy_name);
    for (int i = 0; i < count;  i++){
        if (path_list[i][0] == '!'){
            if (!check_if_imported(&path_list[i][1])){
                return 0;
            }
            continue;
        }
        char *full_path = expand_path(proy_path, path_list[i]);
        if (access(full_path, F_OK) != 0){ // Needs some fixes
            free_path_list(path_list, count);
            free(full_path);
            free(proy_path);
            return 0;
        }
        free(full_path);
    }
    free(proy_path);

    free_path_list(path_list, count);

    return 1;
}

void import_new(char *import_name){
    if (!is_project(".")){
        must_be_project();
        return;
    }
    if (!import_exists(import_name)){
        print_module_does_not_exist(import_name);
        return;
    }
    if (check_if_imported(import_name)){
        print_module_already_imported(import_name);
        return;
    }
    if (!check_if_can_import(import_name)){
        printf("Cannot import '");
        bprint(import_name);
        printf("', project does not follow import structure!\n");
        return;
    }
    char *import_path = get_import_path(import_name);
    printf("Import path: %s\n", import_path);

    unpack_import(import_path);

    free(import_path);
    printf("Succesfully imported '");
    bprint(import_name);
    printf("'!\n");
}

void remove_import(char *import_name){
    if (!is_project(".")){
        must_be_project();
        return;
    }
    if (!import_exists(import_name)){
        print_module_does_not_exist(import_name);
        return;
    }
    if (!check_if_imported(import_name)){
        print_module_not_imported(import_name);
        return;
    }
    int count = 0;
    char **path_list = get_import_paths(import_name, &count);
    if (path_list == NULL){
        mem_alloc_error();
        return;
    }
    
    char *proy_name = get_project_name(".");
    char *proy_path = get_project_path(proy_name);
    free(proy_name);
    for (int i = 0; i < count; i++){
        if (path_list[i][0] == '!'){
            remove_import(&path_list[i][1]);
        }
        char *path_to_file = expand_path(proy_path, path_list[i]);
        if (access(path_to_file, F_OK) == 0) remove(path_to_file);
        free(path_to_file);
    }
    free(proy_path);

    printf("Succesfully removed module '");
    bprint(import_name);
    printf("'!\n");
    free_path_list(path_list, count);
}

void delete_import(char *import_name){
    if (!import_exists(import_name)){
        print_module_does_not_exist(import_name);
        return;
    }
    char *import_path = get_import_path(import_name);

    remove(import_path);

    free(import_path);
    printf("Succesfully deleted module '");
    bprint(import_name);
    printf("'!\n");
}

void list_modules(){
    char *modules_path = resolve_path(config_get("import_dir"));

    int count = 0;
    char **saved_modules = all_type_in_path(modules_path, S_IFREG, &count);
    if (count > 0){
        printf("Saved modules:\n");
        for (int i = 0; i < count; i++) {
            bprint(saved_modules[i]);
            printf("\n");
        }
    }

    printf("You have %d saved modules!\n", count);
    free_type_list(saved_modules, count);

    free(modules_path);
}

// Gets what modules are currently imported
void which_modules(){
    if (!is_project(".")){
        must_be_project();
        return;
    }
    char *modules_path = resolve_path(config_get("import_dir"));

    int count = 0;
    char **saved_modules = all_type_in_path(modules_path, S_IFREG, &count);
    int imported = 0;
    if (count > 0){
        for (int i = 0; i < count; i++) {
            if (check_if_imported(saved_modules[i])){
                printf("'");
                bprint(saved_modules[i]);
                printf("'\n");
                imported++;
            }
        }
    }
    printf("This project has a total of %d modules imported!\n", imported);

    free_type_list(saved_modules, count);

    free(modules_path);
}

void create_import_from_files(char* import_name, char **file_list, int file_count){
    if (import_exists(import_name)){
        printf("The module '");
        bprint(import_name);
        printf("' already exists!\n");
        return;
    }
    if (!is_project(".")){
        must_be_project();
        return;
    }
    char *import_path = get_import_path(import_name);
    
    FILE *import_file = fopen(import_path, "w");
    if (import_file == NULL){
        print_error("Unable to create the import file!\n");
        free(import_path);
        return;
    }
    for (int i = 0; i < file_count; i++){
        if (!is_path(file_list[i])){
            if (!import_exists(file_list[i])){
                print_module_does_not_exist(file_list[i]);
                fclose(import_file);
                free(import_path);
                return;
            }
            fprintf(import_file, "!%s\n", file_list[i]);
            continue;
        }
        char *rel = get_path_relative_to_proy(file_list[i]);
        fprintf(import_file, "%s\n", rel);
        free(rel);
        FILE *imported_file = fopen(file_list[i], "r");
        if (imported_file == NULL){
            print_error("There has been an error reading a file!");
            bprint(file_list[i]);
            free(import_path);
            fclose(import_file);
            return;
        }
        char buffer[1024];
        while (fgets(buffer, 1024, imported_file) != NULL){
            fprintf(import_file, "%s", buffer);
        }
        fclose(imported_file);
    }

    fclose(import_file);

    free(import_path);
    printf("Succesfully created module '");
    bprint(import_name);
    printf("'!\n");
}
