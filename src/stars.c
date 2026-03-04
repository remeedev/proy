#include "headers/extras.h"
#include "headers/config.h"
#include "headers/paths.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * proy (/un)star <proy_name>
 * proy starred 
 * proy set-star <proy_name> <index> TODO
 * 
 */

typedef struct constellation{
    char **stars;
    int stars_size;
}constellation;

constellation* stars = NULL;
char *STARS_PATH = NULL;

int stars_size(){
    return stars->stars_size;
}

void print_stars(){
    int size = stars->stars_size;
    for (int i = 0; i < size; i++) print_index_elem(i, stars->stars[i]);
}

void load_stars(){
    STARS_PATH = config_get("starred_path");
    FILE *ptr;
    char *stars_name = resolve_path(STARS_PATH);
    ptr = fopen(stars_name, "r");
    free(stars_name);
    if (ptr == NULL) {
        print_error("The stars file has not been created!");
        return;
    }
    char buff[256];
    int line_count = 0;
    while (fgets(buff, sizeof(buff), ptr) != NULL) if (strlen(buff) - 1 > 0) line_count++;
    fseek(ptr, 0, SEEK_SET);
    stars = (constellation *)malloc(sizeof(constellation));
    if (stars == NULL){
        mem_alloc_error();
        fclose(ptr);
        return;
    }
    stars->stars_size = line_count;
    stars->stars = (char **)malloc(sizeof(char *)*line_count);
    if (stars->stars == NULL){
        mem_alloc_error();
        fclose(ptr);
        free(stars);
        return;
    }
    int i = 0;
    while (fgets(buff, sizeof(buff), ptr) != NULL) {
        int line_size = strlen(buff);
        if (line_size - 1 == 0) continue;
        stars->stars[i] = (char *)malloc(line_size);
        for (int j = 0; j < line_size - 1; j++) stars->stars[i][j] = buff[j];
        stars->stars[i][line_size - 1] = '\0';
        i++;
    }
    fclose(ptr);
}

void save_stars();

void expand_stars(){
    int curr_size = stars_size() + 1;
    void *waa = realloc(stars->stars, sizeof(void *)*curr_size);
    if (waa == NULL){
        mem_realloc_error();
        return;
    }
    stars->stars = waa;
}

void save_stars(){
    FILE *ptr;
    char *stars_name = resolve_path(STARS_PATH);
    ptr = fopen(stars_name, "w");
    free(stars_name);
    if (ptr == NULL) {
        print_error("The stars file has not been created!");
        return;
    }
    int size = stars_size();
    for (int i = 0; i < size; i++){
        fprintf(ptr, "%s\n", stars->stars[i]);
    }
    fclose(ptr);
}

// Returns if star exists or not
int check_star(char *proy_name){
    int size = stars_size();
    for (int i = 0; i < size; i++){
        if (strcmp(proy_name, stars->stars[i]) == 0) return 1;
    }
    return 0;
}

void add_star(char *proy_name){
    if (!project_exists(proy_name)){
        print_project_does_not_exist(proy_name);
        return;
    }
    if (check_star(proy_name)){
        print_project_already_starred(proy_name);
        return;
    }
    int pos = stars_size();
    expand_stars();
    stars->stars[pos] = strdup(proy_name);
    stars->stars_size++;
    printf("Project [%s] was succesfully starred!\n", proy_name);
    save_stars();
}

void set_star(char *proy_name, int index){
    if (config_get_i("star_reorder")){
        printf("Star reorder is turned off!\nThe project index might not persist over time.\n");
    }
    if (index < 0){
        print_error("Index must be positive or 0.");
    }
    int max_capacity = stars_size();
    if (!check_star(proy_name)){
        if (index >= max_capacity + 1){
            printf("The index must be less than %d!\n", max_capacity + 1);
            return;
        }
        max_capacity++;
        add_star(proy_name);
    }else{
        if (index >= max_capacity + 1){
            printf("The index must be less than %d\n", max_capacity);
            return;
        }
    }
    int i = 0;
    int size = stars_size();
    while (strcmp(proy_name, stars->stars[i]) != 0) i++;
    void *tmp = stars->stars[index];
    stars->stars[index] = stars->stars[i];
    stars->stars[i] = tmp;
    save_stars();
}

void del_star(char *proy_name){
    char **new_stars = (char **)malloc(sizeof(char **)*(stars->stars_size - 1));
    if (new_stars == NULL){
        mem_alloc_error();
        return;
    }
    int size = stars->stars_size; 
    int i = 0;
    for (int _=0; _ < size; _++) {
        if (strcmp(stars->stars[_], proy_name) != 0) {
            new_stars[i++] = stars->stars[_];
        }else {
            free(stars->stars[_]);
        }
    }
    free(stars->stars);
    stars->stars = new_stars;
    stars->stars_size--;
    save_stars();
}

void unload_stars(){
    int size = stars_size();
    for (int i = 0; i < size; i++){
        free(stars->stars[i]);
    }
    free(stars->stars);
    free(stars);
}

void open_starred(int pos){
    if (pos < 0 || pos >= stars->stars_size){
        char buff[12];
        sprintf(buff, "*%d", pos);
        print_project_does_not_exist(buff);
        return;
    }
    char *full_star_path = get_project_path(stars->stars[pos]);
    printf("%s", full_star_path);
    free(full_star_path);
    if (config_get_i("star_reorder") && pos != 0) {
        char *tmp = stars->stars[pos];
        for (int i = pos; i > 0; i--){
            stars->stars[i] = stars->stars[i - 1];
        }
        stars->stars[0] = tmp;
        save_stars();
    }
}
