#include "headers/versions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "headers/extras.h"
#include "headers/paths.h"
#include "headers/config.h"

int has_version(char *project_name);

void init_project_version(char *project_name){
    if (has_version(project_name)){
        printf("Project '");
        bprint(project_name);
        printf("' already has version!\n");
        return;
    }
    char *project_path = get_project_path(project_name);
    char *version_file_path = expand_path(project_path, "./.version");
    FILE *version_file = fopen(version_file_path, "w");
    free(version_file_path);

    fprintf(version_file, "%s", config_get("default_version"));

    fclose(version_file);
    free(project_path);
}

char *get_project_version(char *project_name){
    char *project_path = get_project_path(project_name);
    char *version_file_path = expand_path(project_path, "./.version");

    char *out = NULL;
    if (access(version_file_path, F_OK) != 0){
        free(version_file_path);
        free(project_path);
        return NULL;
    }
    FILE *version_file = fopen(version_file_path, "r");
    free(version_file_path);

    if (version_file == NULL){
        print_error("There has been an error opening the version file!\n");
        return NULL;
    }

    char buffer[25];
    if (fgets(buffer, 25, version_file) == NULL){
        print_error("There has been an error reading the version file!\n");
        goto end;
    }else{
        out = strdup(buffer);
    }

end:
    fclose(version_file);
    free(project_path);
    return out;
}

int is_version(char *str);

int has_version(char *project_name){
    char *ver = get_project_version(project_name);
    int out = (ver ? is_version(ver) : 0);
    free(ver);
    return out;
}

int segment_version(char *version, int *major, int *minor, int *sub){
    *major = 0;
    *minor = 0;
    *sub = 0;
    if (!is_version(version)){
        print_error("Badly formatted version string!\n");
        bprint(version);
        return 0;
    }
    int i = 0;
    while (version[i] != '\0' && version[i] != '.'){
        *major*=10;
        *major+=(int)(version[i] - '0');
        i++;
    }
    i++;
    while (version[i] != '\0' && version[i] != '.'){
        *minor*=10;
        *minor+=(int)(version[i] - '0');
        i++;
    }
    i++;
    while (version[i] != '\0' && version[i] != ' '){
        *sub*=10;
        *sub+=(int)(version[i] - '0');
        i++;
    }
    return 1;
}

char *get_version_text(int major, int minor, int sub){
    char buff[25];
    sprintf(buff, "%d.%d.%d", major, minor, sub);
    return strdup(buff);
}

int is_version(char *str){
    int i = 0;
    int num_count = 1;
    while (str[i] != '\0' && str[i] != ' '){
        if (str[i] != '.' && (str[i] < '0' && str[i] > '9')) return 0;
        if (str[i] == '.') num_count++;
        i++;
    }
    return 1;
}

char *get_version_alias(char *project_name){
    char *version_text = get_project_version(project_name);
    char *version_alias = NULL;
    int i = 0;
    while (version_text[i] != ' ' && version_text[i] != '\0') i++;
    if (version_text[i] == '\0') goto end;
    version_alias = strdup(&version_text[++i]);
end:
    free(version_text);
    return version_alias;
}

char *get_version_num(char *project_name){
    char *version_text = get_project_version(project_name);
    char *out = NULL;
    int i = 0;
    while (version_text[i] != ' ' && version_text[i] != '\0') i++;
    if (version_text[i] == '\0') goto end;
    version_text[i] = '\0';
end:
    out = strdup(version_text);
    free(version_text);
    return out;
}

void add_project_alias(char *project_name, char *version_alias){
    char *project_path = get_project_path(project_name);
    if (!has_version(project_name)){
        printf("This project does not have a version!\n");
        printf("Do 'proy track %s' to start tracking it!\n", project_name);
        return;
    }
    if (!is_project(project_path)){
        print_project_does_not_exist(project_path);
        free(project_path);
        return;
    }
    char *proj_version = get_version_num(project_name);
    char *version_path = expand_path(project_path, "./.version");
    free(project_path);
    FILE *file = fopen(version_path, "w");
    free(version_path);
    if (file == NULL){
        print_error("There has been an error opening the version file!\n");
        return;
    }
    fprintf(file, "%s %s", proj_version, version_alias);
    fclose(file);
    free(proj_version);
    printf("Succesfully added version alias!\n");
}

void set_project_version(char *project_name, char *version){
    char *project_path = get_project_path(project_name);
    if (!has_version(project_name)){
        print_proy_has_no_version(project_name);
        return;
    }
    if (!is_project(project_path)){
        print_project_does_not_exist(project_path);
        free(project_path);
        return;
    }
    char *version_alias = get_version_alias(project_name);
    char *version_path = expand_path(project_path, "./.version");
    free(project_path);
    FILE *file = fopen(version_path, "w");
    free(version_path);
    if (file == NULL){
        print_error("There has been an error opening the version file!\n");
        return;
    }
    if (version_alias){
        fprintf(file, "%s %s", version, version_alias);
    }else {
        fprintf(file, "%s", version);
    }
    printf("v%s", version);
    fclose(file);
    if (version_alias)free(version_alias);
}

int v_check(char *project_name){
    if (!project_exists(project_name)){
        print_project_does_not_exist(project_name);
        return 0;
    }
    if (!has_version(project_name)){
        print_proy_has_no_version(project_name);
        return 0;
    }
    return 1;
}

void upgrade_sub(char *project_name){
    if (!v_check(project_name)) return;
    char *prev_version = get_project_version(project_name);
    int major, minor, sub;
    segment_version(prev_version, &major, &minor, &sub);
    sub++;
    char *new_ver = get_version_text(major, minor, sub);
    set_project_version(project_name, new_ver);
    free(new_ver);
    free(prev_version);
}

void upgrade_major(char *project_name){
    if (!v_check(project_name)) return;
    char *prev_version = get_project_version(project_name);
    int major, minor, sub;
    segment_version(prev_version, &major, &minor, &sub);
    major++;
    char *new_ver = get_version_text(major, minor, sub);
    set_project_version(project_name, new_ver);
    free(new_ver);
    free(prev_version);
}

void upgrade_minor(char *project_name){
    if (!v_check(project_name)) return;
    char *prev_version = get_project_version(project_name);
    int major, minor, sub;
    segment_version(prev_version, &major, &minor, &sub);
    minor++;
    char *new_ver = get_version_text(major, minor, sub);
    set_project_version(project_name, new_ver);
    free(new_ver);
    free(prev_version);
}
