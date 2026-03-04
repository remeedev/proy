/*
 *
 * Config File Attr
 *
 * proy_folder=/path/to/folder/to/save/files
 *
 *
 *
 */
#include "headers/config.h"
#include "headers/single-linked-list.h"
#include "headers/utils.h"
#include "stdlib.h"

#include "headers/files.h"
#include "headers/hash_map.h"
#include "headers/paths.h"
#include "headers/extras.h"
#include <stdio.h>
#include <unistd.h>

hash* config = NULL;

int check_for_config_at(char *path){
    config = hash_read(NULL, path);
    if (config == NULL) return 0;
    return 1;
}

void ensure_directories(){
    if (!config){
        print_error("The configuration setup has not been called and therefore no config has been loaded!\n");
        return;
    }
    sll* kv_pairs = get_key_value_pairs(config);
    kv_pair* kv = kv_pairs->first;
    while (kv){
        char *_path_ = resolve_path(kv->value);
        if (is_path(_path_)){
            if (is_file(_path_)){
                check_or_create_file(_path_, 0);
            }else{
                check_or_create_folder(_path_, 0);
            }
        }
        free(_path_);
        kv = kv->next;
    }
    free_sll(kv_pairs);
}

char *config_get(char *property);

void change_config(char *config_opt, char *setting){
    if (!config){
        print_error("Configuration setup has not been called!\n");
        return;
    }
    if (hash_get(config, config_opt) == NULL){
        printf("%s was not recognized as a config option.\n", config_opt);
        return;
    }
    char *_check = resolve_path(config_get(config_opt));
    int path_confirmed = 0;
    if (is_path(_check)) path_confirmed++;
    if (path_confirmed){
        if (access(_check, F_OK) == 0){
            bprint("DETECTED PATH CHANGE!\n");
            printf("This program does not automatically move folder/file contents, you might lose saved content by changing the");
            printf(" configuration, if you would not like that to happen:\n");
            printf("Do:\n\t");
            bprint("mv ");
            bprint(_check);
            char *out_path = resolve_path(setting);
            printf(" ");
            bprint(out_path);
            printf("\n");
            free(out_path);
        }
    }
    free(_check);
    if (!is_int(setting)){
        config = hash_append(config, config_opt, setting);
    }else {
        config = hash_int_append(config, config_opt, str2int(setting));
    }
    char *config_path = resolve_path("+/proy.conf");
    save_config_to(config_path);
    printf("Saved new configuration to ");
    bprint(config_path);
    printf("\n");
    free(config_path);
}

hash *generate_default_config(){
    hash *out = create_hash_map(11);
    if (out == NULL) return NULL;

    // Path dictations: 
    /*
#define HOME_CHAR '~'
#define DATA_CHAR '$'
#define CONFIG_CHAR '+'
#define CURR_CHAR '.'
     */
    out = hash_append(out, "project_path", "~/proy");
    out = hash_append(out, "pin_path","$/pin.txt"); 
    out = hash_append(out, "starred_path", "$/starred.txt");
    out = hash_append(out, "tmplt_lookup", "$/templates");
    out = hash_append(out, "import_dir", "$/imports");
    out = hash_append(out, "default_version", "0.0.0");
    out = hash_int_append(out, "project_handling", 0);
    out = hash_int_append(out, "star_reorder", 1);

    return out;
}

void save_config_to(char *path){
    hash_save(config, path);
}

void setup_config(){
    char *config_path = resolve_path("+/proy.conf");
    int config_file_found = check_for_config_at(config_path);
    free(config_path);
    if (!config_file_found) {
        config = generate_default_config();
        ensure_directories();
        return;
    }
    hash *def_config = generate_default_config();
    hash_merge_missing_keys(def_config, config);

    hash_free(def_config);    

    ensure_directories();
}

char *config_get(char *property){
    if (!config){
        print_error("The configuration setup has not been called an therefore no config has been loaded!\n");
        return NULL;
    }
    return hash_get(config, property);
}

int config_get_i(char *property){
    if (!config){
        print_error("The configuration setup has not been called an therefore no config has been loaded!\n");
        return -1;
    }
    return hash_int_get(config, property);
}

void end_config(){
    if (config) hash_free(config);
    config = NULL;
}
