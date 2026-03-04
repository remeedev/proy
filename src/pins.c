#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "headers/config.h"
#include "headers/paths.h"
#include "headers/extras.h"
#include "headers/paths.h"

char *curr_pin = NULL;
char *PIN_PATH =NULL;

void load_pin(){
    PIN_PATH = config_get("pin_path");
    char *resolved_path = resolve_path(PIN_PATH);
    FILE *file = fopen(resolved_path, "r");
    free(resolved_path);
    if (file == NULL){
        print_error("The pin file has not been created!");
        return;
    }
    char buff[256];
    if (fgets(buff, sizeof(buff), file)){
        curr_pin = strdup(buff);
    }
    fclose(file);
}

void unload_pin(){
    if (curr_pin) free(curr_pin);
    curr_pin = NULL;
}

void pin_proy(char *proy_name){
    if (!project_exists(proy_name)){
        print_project_does_not_exist(proy_name);
        return;
    }
    unload_pin();
    curr_pin = strdup(proy_name);
    char *res_path = resolve_path(PIN_PATH);
    FILE *file = fopen(res_path, "w");
    free(res_path);
    if (file == NULL){
        print_error("There has been an error opening the pin file!\n");
        return;
    }
    fprintf(file, "%s", curr_pin);
    fclose(file);
    printf("[%s] has been succesfully set as the pinned project!\n", proy_name);
}

void unpin_proy(){
    if (curr_pin == NULL){
        printf("There is currently no pinned project!\n");
        printf("Pin a project by doing the following command:\n");
        printf("\tproy pin <project_name>\n");
        return;
    }
    printf("Unpinning %s...\n", curr_pin);
    
    unload_pin();

    char *res_path = resolve_path(PIN_PATH);
    FILE *file = fopen(res_path, "w");
    free(res_path);
    if (file == NULL){
        print_error("There has been an error opening the pin file!\n");
        return;
    }
    fclose(file);
    printf("Succesfully unpinned project!\n");
}

char *get_curr_pin(){
    return curr_pin;
}
