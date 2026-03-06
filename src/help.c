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
 * 
 * proy (/un)pin <proy_name>
 * proy
 *
 * proy track <proy_name|NULL> 
 * proy v-alias <alias> 
 * proy major ->Will set the version to the next major version 
 * proy minor ->Will set the version to the next minor version 
 * proy sub ->Will set the version to the next sub version 
 *
 */

#include <stdio.h>

#include "headers/extras.h"

void of_course(){
    print_command_overview("help", "shows this menu.");
}

void print_help(){
    printf("PROY help menu:\nUsage : 'proy <command_name> [options...]'\n");
    printf("commands with a * ");
    bprint("will");
    printf(" create a brand new bash shell inside your shell, make sure to run 'exit' to leave a project!\n");
    printf("The following is the command list:\n");
    of_course();
    print_command_overview("list", "Displays a list of all projects.");
    print_command_overview("this", "Displays information about the project the user is standing on.");
    print_command_ov_usage("*new", "Creates a new project and cd's into it.", "proy new <project_name> <template_name>");
    print_command_ov_usage("del", "Deletes a given project.", "proy del <project_name>");
    print_command_ov_usage("*open", "cd's into a project.", "proy open <project_name>");
    print_command_ov_usage("star", "Stars a given project. More information about stars in the README or do help README", "proy star <project_name>");
    print_command_ov_usage("set-star", "Sets the star index for a given project.", "proy set-star <project_name> <star_index>");
    print_command_ov_usage("unstar", "Removes a project from the starred list.", "proy unstar <project_name>");
    print_command_ov_usage("pin", "Pins a given project. More information about pinning in the README or do help README", "proy pin <project_name>");
    print_command_overview("unpin", "Removes the pin from a project.");
    print_command_ov_usage("template", "All options of templates, run 'proy template help' for more info.", "proy template [options...]");
    print_command_ov_usage("module", "All options of imports, run 'proy module help' for more info.", "proy module [options...]");
    print_command_ov_usage("track", "Stores version number inside the project's folder. More info on versioning is inside README or do help README", "proy track <project_name/null>");
    print_command_ov_usage("v-alias", "Sets an alias to the current project version.", "proy alias <alias_name> <project_name/null>");
}

void print_template_help(){
    printf("Templates help menu:\n");
    print_prev_declare("proy template");
    of_course();
    print_command_ov_usage("new", "Creates a brand new template from a dir.", "proy template new <template_name> <dir>");
    print_command_ov_usage("del", "Deletes a template.", "proy template del <template_name>");
    print_command_overview("list", "Displays a list of all templates created.");
    print_command_overview("who", "Displays the name of a template used for the project the user is standing on.");
}

/*
 * proy template add <template_name> <dir>
 * proy template del <template_name>
 * proy template list
 * proy template who
 *
 * proy import create <import_name> <files_to_store> 
 * proy import delete <import_name> 
 * proy import add <import_name> 
 * proy import remove <import_name> 
 * proy import which
 *
 */

void print_import_help(){
    printf("Import help menu:\n");
    print_prev_declare("proy module");
    of_course();
    print_command_ov_usage("create", "Creates a new module from the files fed in. The function does not allow for a folder to be fed in.", "proy module create <import_name> [path/to/files or Module Dependencies]");
    print_command_ov_usage("delete", "Deletes a saved module.", "proy module delete <import_name>");
    print_command_ov_usage("add", "Imports a saved module to the current project.", "proy module add <import_name>");
    print_command_ov_usage("remove", "Removes a module from the current project.", "proy module remove <import_name>");
    print_command_overview("which", "Displays all modules currently imported in the project.");
}

void print_versioning_help(){
    printf("Versioning help menu:\n");
    print_prev_declare("proy v");
    of_course();
    print_command_overview("major", "Increases project version by a major version.");
    print_command_overview("minor", "Increases project version by a minor version.");
    print_command_overview("sub", "Increases project version by a sub version.");
}

void print_configuration_help(){
    printf("Configuration help menu:\n");
    print_prev_declare("proy conf");
    of_course();
    print_command_ov_usage("get", "Displays the current value of a configuration option.", "proy conf get <conf_option>");
    print_command_ov_usage("set", "Sets a value in the configuration.", "proy conf set <conf_option> <value>");
    print_command_ov_usage("dump", "Writes the configuration values to the output file specified.", "proy conf dump <path/to/file.ext>");
}
