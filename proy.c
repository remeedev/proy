/*
 * THIS IS NOT THE PROGRAM YOU RUN
 * THIS IS THE PROGRAM WITH THE LOGIC BUT YOU RUN PROY_RUN
 * 
 * Usage:
 *  proy <option>
 *
 * proy help
 *
 * proy list
 * proy this
 *
 * proy new <proy_name>
 * proy del <proy_name>
 * proy open <proy_name>
 * 
 * proy (/un)star <proy_name> 
 * proy starred 
 * proy set-star <proy_name> <index>
 * 
 * proy (/un)pin <proy_name>
 * proy
 *
 * proy template add <template_name> <dir>
 * proy template del <template_name>
 * proy template list
 * proy template who
 * proy template help
 *
 * proy import create <import_name> <files_to_store> 
 * proy import delete <import_name> 
 * proy import add <import_name> 
 * proy import remove <import_name> 
 * proy import which
 * proy import help
 *
 * proy track <proy_name|NULL> 
 * proy v-alias <alias> 
 * proy major ->Will set the version to the next major version 
 * proy minor ->Will set the version to the next minor version 
 * proy sub ->Will set the version to the next sub version 
 *
 * proy conf get <config_option>
 * proy conf set <config_option> <value>
 * proy conf dump <path>
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/headers/extras.h"
#include "src/headers/files.h"
#include "src/headers/imports.h"
#include "src/headers/paths.h"
#include "src/headers/config.h"
#include "src/headers/stars.h"
#include "src/headers/utils.h"
#include "src/headers/pins.h"
#include "src/headers/templates.h"
#include "src/headers/versions.h"
#include "src/headers/help.h" // Not _really_ needed 

void create_proy(int argc, char *argv[]){
    if (argc != 2 && argc != 1){
        print_incorrect_usage("proy new <project_name> <project_type|NULL>");
        return;
    }
    if (project_exists(argv[0])){
        print_project_already_exists(argv[0]);
        return;
    }
    char *full_proy_path = get_project_path(argv[0]);
    check_or_create_folder_s(full_proy_path);
    if (argc >= 2){
        if (!unpack_template_to_dir(argv[1], full_proy_path)){
            printf("There has been an error unpacking the template to the project!\n");
            remove(full_proy_path);
            return;
        }
    }
    printf("%s", full_proy_path);
    free(full_proy_path);
}

void open_proy(int argc, char *argv[]){
    if (argc != 1){
        print_incorrect_usage("proy open <project_name>");
        return;
    }
    if (is_int(argv[0])){
        open_starred(str2int(argv[0]));
        return;
    }
    if (!project_exists(argv[0])){
        print_project_does_not_exist(argv[0]);
        return;
    }
    char *fpp = get_project_path(argv[0]);
    printf("%s", fpp);
    free(fpp);
}

void del_proy(int argc, char *argv[]){
    if (argc != 1){
        print_incorrect_usage("proy del <project_name>");
        return;
    }
    if (!project_exists(argv[0])){
        print_project_does_not_exist(argv[0]);
        return;
    }
    char *proy_path = get_project_path(argv[0]);
    check_and_delete_folder(proy_path, 0);
    free(proy_path);
}

int is(char *a, char *b){
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0'){
        if (a[i] != b[i]) return 0;
        i++;
    }
    if (a[i] != b[i]) return 0;
    return 1;
}

void redirect_add_star(int argc, char **argv){
    if (argc != 1){
        print_incorrect_usage("proy star <project_name>");
        return;
    }
    add_star(argv[0]);
}

void redirect_del_star(int argc, char **argv){
    if (argc != 1){
        print_incorrect_usage("proy unstar <project_name>");
        return;
    }
    del_star(argv[0]);
}

void redirect_pin_proy(int argc, char **argv){
    if (argc != 1){
        print_incorrect_usage("proy pin <project_name>");
        return;
    }
    pin_proy(argv[0]);
}

void redirect_templates(int argc, char **argv){
    if (argc == 0){
        print_incorrect_usage("proy templates <command> [Options...]");
        return;
    }
    if (is(argv[0], "new")){
        if (argc < 3){
            print_incorrect_usage("proy templates new <template_name> <directory>");
            return;
        }
        generate_template_from_directory(argv[1], argv[2]);
    }else if (is(argv[0], "del")){
        if (argc < 2){
            print_incorrect_usage("proy templates del <template_name>");
            return;
        }
        delete_template(argv[1]);
    }else if (is(argv[0], "list")){
        print_template_list();
    }else if (is(argv[0], "cmp")){
        if (argc < 3){
            print_incorrect_usage("proy template cmp <template_name> <project_name>");
            return;
        }
        if (!project_exists(argv[2])){
            print_project_does_not_exist(argv[2]);
        }
        char *path = get_project_path(argv[2]);

        printf("The template ");
        bprint(argv[1]);
        printf(" does ");
        if (!compare_template_to_directory(argv[1], path)){
            bprint("not ");
        }
        printf("match the structure of ");
        bprint(path);
        printf("!\n");

        free(path);
    } else if (is(argv[0], "who")){
        if (argc < 2){
            print_incorrect_usage("proy template who <project_name>");
            return;
        }
        if (!project_exists(argv[1])){
            print_project_does_not_exist(argv[1]);
            return;
        }
        char *project_path = get_project_path(argv[1]);
        char *template = get_template_from_dir(project_path);
        if (template == NULL){
            printf("No template matches the contents of %s\n", argv[1]);
        }else{
            printf("The template ");
            bprint(template);
            free(template);
            printf(" matches the contents of %s\n", argv[1]);
        }
    }else if (is(argv[0], "help")){
        print_template_help();
    }else{
        print_command_not_recognized(argv[0]);
    }
}

void redirect_modules(int argc, char *argv[]){
    if (argc < 1){
        print_incorrect_usage("proy module <command_name> [options]");
        return;
    }
    if (is(argv[0], "create")){
        if (argc < 3){
            print_incorrect_usage("proy module create <module_name> [paths/to/files]");
            return;
        }
        create_import_from_files(argv[1], &argv[2], argc - 2);
    }else if (is(argv[0], "delete")){
        if (argc < 2){
            print_incorrect_usage("proy module delete <module_name>");
            return;
        }
        delete_import(argv[1]);
    }else if (is(argv[0], "add")){
        if (argc < 2){
            print_incorrect_usage("proy module add <module_name>");
            return;
        }
        import_new(argv[1]);
    }else if (is(argv[0], "remove")){
        if (argc < 2){
            print_incorrect_usage("proy module remove <module_name>");
            return;
        }
        remove_import(argv[1]);
    }else if (is(argv[0], "list")){
        list_modules();
    }else if (is(argv[0], "which")){
        which_modules();
    }else if(is(argv[0], "help")){
        print_import_help();
    }else {
        print_command_not_recognized(argv[0]);
    }
}

void redirect_version_start(int argc, char *argv[]){
    if (argc > 0){
        char *project_path = get_project_path(argv[0]);
        if (!is_project(project_path)){
            print_project_does_not_exist(argv[0]);
            free(project_path);
            return;
        }
        init_project_version(argv[0]);

        free(project_path);
    }else{
        if (!is_project(".")){
            print_incorrect_usage("proy track <project_name>");
            return;
        }
        char *proy_name = get_project_name(".");
        init_project_version(proy_name);
        free(proy_name);
    }
}

void redirect_version_alias(int argc, char *argv[]){
    if (argc == 0){
        print_incorrect_usage("proy v-alias [project_name/] <alias>");
        return;
    }
    char *project_name = NULL;
    char *version_alias = NULL;
    if (argc < 2){
        if (!is_project(".")){
            must_be_project();
            return;
        }
        project_name = get_project_name(".");
        version_alias = argv[0];
        add_project_alias(project_name, version_alias);
        free(project_name);
    }else{
        project_name = argv[0];
        version_alias = argv[1];
        add_project_alias(project_name, version_alias);
    }
}

void redirect_version_upgrade(int argc, char *argv[]){
    if (argc == 0){
        print_incorrect_usage("proy v <type> [proy_name/]");
        return;
    }
    if (argc < 2){
        if (!is_project(".")){
            must_be_project();
            return;
        }
        char *proy_name = get_project_name(".");

        if (is(argv[0], "major")){
            upgrade_major(proy_name);
        }else if(is(argv[0], "minor")){
            upgrade_minor(proy_name);
        }else if(is(argv[0], "sub")){
            upgrade_sub(proy_name);
        }else if(is(argv[0], "help")){
            print_versioning_help();
        }

        free(proy_name);
    }else{
        char *proy_name = argv[1];
        if (is(argv[0], "major")){
            upgrade_major(proy_name);
        }else if(is(argv[0], "minor")){
            upgrade_minor(proy_name);
        }else if(is(argv[0], "sub")){
            upgrade_sub(proy_name);
        }
    }
}

void redirect_set_star(int argc, char *argv[]){
    if (argc < 2 || !project_exists(argv[0]) || !is_int(argv[1])){
        print_incorrect_usage("proy set-star <project_name> <star_index>");
        return;
    }
    int index = str2int(argv[1]);
    set_star(argv[0], index);
}

void redirect_configuration(int argc, char *argv[]){
    if (argc < 1){
        print_incorrect_usage("proy conf <conf_option> [options...]");
        return;
    }
    if (is(argv[0], "get")){
        if (argc < 2){
            print_incorrect_usage("proy conf get <config_option>");
            return;
        }
        char *out = config_get(argv[1]);
        if (out){
            printf("%s\n", out);
        }else {
            printf("There is no configuration option called '");
            bprint(argv[1]);
            printf("'\n");
        }
    }else if(is(argv[0], "set")){
        if (argc < 3){
            print_incorrect_usage("proy conf set <conf_option> <value>");
            return;
        }
        change_config(argv[1], argv[2]);
    }else if(is(argv[0], "dump")){
        if (argc < 2 || !is_path(argv[1]) || !is_file(argv[1])){
            print_incorrect_usage("proy conf dump <path/to/file>");
        }
        save_config_to(argv[1]);
    }else if (is(argv[0], "help")){
        print_configuration_help();
    }
}

int main(int argc, char *argv[]){
    if(setup_paths()){
        setup_config();
        end_config();
        end_paths();
        return 0;
    }
    // For windows: Userdir
    if (!get_home_dir()){
        printf("There is no home directory! Set it to start development!\n");
        return 1;
    }

    setup_config();
    load_stars();
    load_pin();

    if (argc >= 2){
        if (is(argv[1], "new")){
            create_proy(argc - 2, &argv[2]);
        }else if (is(argv[1], "open")){
            open_proy(argc - 2, &argv[2]);
        }else if (is(argv[1], "del")){
            del_proy(argc - 2, &argv[2]);
        }else if(is(argv[1], "star")){
            redirect_add_star(argc - 2, &argv[2]);
        }else if(is(argv[1], "starred")){
            print_stars();
        }else if(is(argv[1], "unstar")){
            redirect_del_star(argc - 2, &argv[2]);
        }else if (is(argv[1], "list")){
            char *project_path = resolve_path(config_get("project_path"));
            int count = 0;
            char **all = all_type_in_path(project_path,S_IFDIR, &count);
            if (count == 0){
                printf("There are no projects in %s!\n", project_path);
            }else{
                printf("All projects created so far:\n");
                for (int i = 0; i < count; i++) printf("%s\n", all[i]);
                printf("You have a total of %d projects!\n", count);
            }
            free_type_list(all, count);
            free(project_path);
        }else if (is(argv[1], "pin")){
            redirect_pin_proy(argc - 2, &argv[2]);
        }else if (is(argv[1], "unpin")){
            unpin_proy();
        }else if (is(argv[1], "template")){
            redirect_templates(argc - 2, &argv[2]);
        }else if (is(argv[1], "resolve")){
            char *out = resolve_path(argv[2]);
            printf("%s\n", out);
            free(out);
        }else if(is(argv[1], "this")){
            char *proy_name = get_project_name(".");
            if (proy_name != NULL){
                bprint(proy_name);
                char *project_version = get_project_version(proy_name);
                printf(" v%s\n", project_version);
                free(project_version);
                free(proy_name);
            }else{
                printf("Current directory is not inside a project!\n");
            }
        }else if (is(argv[1], "module")){
            redirect_modules(argc - 2, &argv[2]);
        }else if (is(argv[1], "track")){
            redirect_version_start(argc - 2, &argv[2]);
        } else if (is(argv[1], "v-alias")){
            redirect_version_alias(argc - 2, &argv[2]);
        }else if (is(argv[1], "v")){
            redirect_version_upgrade(argc - 2, &argv[2]);
        }else if (is(argv[1], "help")){
            print_help();
        }else if (is(argv[1], "set-star")){
            redirect_set_star(argc - 2, &argv[2]);
        }else if (is(argv[1], "conf")){
            redirect_configuration(argc - 2, &argv[2]);
        }else{
            print_command_not_recognized(argv[1]);
        }
    }else{
        if (get_curr_pin() == NULL){
            print_error("There is no currently pinned project!\n");
        }else{
            char *proy_path = get_project_path(get_curr_pin());
            printf("%s", proy_path);
            free(proy_path);
        }
    }

    unload_pin();
    unload_stars();
    end_config();
    end_paths();
    return 0;
}
