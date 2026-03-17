#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "headers/config.h"
#include "headers/paths.h"
#include "headers/extras.h"

#define BUF_SIZE 8192

void add_file_to_compress(struct archive *a, const char *path){
    struct archive_entry *ae;
    struct stat st;
    char buf[BUF_SIZE];
    int fd = 0;
    ssize_t len;

    printf("Adding %s to template!\n", path);
    stat(path, &st);
    ae = archive_entry_new();
    archive_entry_set_pathname(ae, path);
    archive_entry_set_size(ae, st.st_size);
    archive_entry_set_filetype(ae, AE_IFREG);
    archive_entry_set_perm(ae, 0644);
    archive_write_header(a, ae);

    fd = open(path, O_RDONLY);

    while ((len = read(fd, buf, BUF_SIZE)) > 0){
        archive_write_data(a, buf, len);
    }

    close(fd);
    archive_entry_free(ae);
}

void add_folder_to_compress(struct archive *a, const char *path){
    struct archive_entry *ae;

    ae = archive_entry_new();
    archive_entry_set_pathname(ae, path);
    archive_entry_set_size(ae, 0);
    archive_entry_set_filetype(ae, AE_IFDIR);
    archive_entry_set_perm(ae, 0755);
    archive_write_header(a, ae);

    archive_entry_free(ae);
}

void read_directory_to_archive(struct archive *a, const char* path, int offset);

int calculate_offset(const char *path){
    int i = strlen(path) - 1;
    return i;
}

void compress_directory(const char *from, const char *to){
    struct archive *a;
    a = archive_write_new();
    if (archive_write_set_format_zip(a) != ARCHIVE_OK){
        printf("There was an error creating archive!\n");
        archive_free(a);
        return;
    }
    if (archive_write_add_filter_none(a) != ARCHIVE_OK){
        printf("There was an error setting filter on archive!\n");
        archive_free(a);
        return;
    }

    archive_write_open_filename(a, to);

    int offset = calculate_offset(from);
    read_directory_to_archive(a, from, offset);

    archive_write_close(a);

    archive_free(a);
}

void read_directory_to_archive(struct archive *a, const char* path, int offset){
    struct dirent* d;
    DIR *directory;
    struct stat file_info;

    directory = opendir(path);
    if (directory == NULL) {
        printf("There has been an error opening %s\n", path);
        return;
    }
    while ((d = readdir(directory)) != NULL){
        if (d->d_name[0] == '.') continue;
        char *full_path = (char *)malloc(strlen(path) + strlen(d->d_name) + 2);
        if (full_path == NULL){
            mem_alloc_error();
            return;
        }
        strcpy(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, d->d_name);

        char *compress_path = (char *)malloc(strlen(full_path) - offset + 2);
        if (compress_path == NULL){
            mem_alloc_error();
            return;
        }
        strcpy(compress_path, ".");
        strcat(compress_path, &full_path[offset + 1]);

        if (stat(full_path, &file_info) != -1){
            int file_type = file_info.st_mode & S_IFMT;
            if (file_type == S_IFREG){
                add_file_to_compress(a, compress_path);
            }else if (file_type == S_IFDIR && strcmp(d->d_name, "..") != 0 && strcmp(d->d_name, ".")!=0){
                add_folder_to_compress(a, compress_path);
                read_directory_to_archive(a, full_path, offset);
            }
        }
        free(compress_path);
        free(full_path);
    }
    closedir(directory);
}

void decompress_file_to(char *from, char *to){
    if (access(to, F_OK) != 0){
        if (mkdir(to, 0777) != 0){
            printf("There has been an error trying to generate destination folder!\n");
            return;
        }
        printf("Created end destination folder!\n");
    }
    struct archive *a;
    a = archive_read_new();
    if (a == NULL) return;

    archive_read_support_format_zip(a);
    archive_read_open_filename(a, from, 10240);
    
    struct archive_entry *ae;
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK){
        const char *path_name = archive_entry_pathname(ae);
        int p_count = 0;
        for (int i = 0; path_name[i] != '\0' && p_count != 2; i++){
            if (path_name[i] == '.'){
                p_count++;
            }else{
                p_count = 0;
            }
        }
        if (p_count == 2) {
            printf("Skipping [%s]...\n", path_name);
            continue;
        }
        int file_type = archive_entry_filetype(ae);
        char *complete_path = expand_path(to, path_name);
        if (complete_path == NULL){
            printf("Error resolving path for %s\n", path_name);
            continue;
        }
        if (file_type == S_IFDIR){
            //printf("%s ->%s\n", path_name, complete_path);
            if (access(complete_path, F_OK) != 0){
                if (mkdir(complete_path, 0777) != 0){
                    printf("There has been an error uncompressing file!\n");
                }
            }
        }else if (file_type == S_IFREG){
            int file_size = archive_entry_size(ae);
            //printf("%s ->%s [%d]\n", path_name, complete_path, file_size);
            unsigned char *content = (unsigned char *)malloc(file_size);

            FILE *out_file = fopen(complete_path, "wba");
            if (content != NULL && archive_read_data(a, content, file_size) > 0){
                if (out_file == NULL){
                    printf("There has been an error creating file [%s]\n", complete_path);
                    free(complete_path);
                    continue;
                }
                fwrite(content, 1, file_size, out_file);
            }

            fclose(out_file);
            if (content) free(content);
        }
        free(complete_path);
    }

    archive_read_close(a);
    archive_free(a);
}

char *get_template_path(char *template_name){
    char *zip_path_list[] = {config_get("tmplt_lookup"), "/", template_name, ".zip"};
    char *zip_path = resolve_path_list(zip_path_list, 4);
    return zip_path;
}

char **get_template_list(size_t *size){
    struct dirent *d;
    char *tmplts_path = resolve_path(config_get("tmplt_lookup"));
    DIR *dir = opendir(tmplts_path);
    free(tmplts_path);
    if (dir == NULL){
        print_error("There was an error getting the template folder!\n");
        return NULL;
    }
    size_t tmplt_count = 0;
    while ((d = readdir(dir)) != NULL){
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;
        tmplt_count++;
    }
    char **out = (char **)malloc(sizeof(char *)*tmplt_count);
    if (out == NULL){
        mem_alloc_error();
        return NULL;
    }
    int i = 0;
    rewinddir(dir);
    while ((d = readdir(dir)) != NULL){
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;
        size_t tmplt_size = 0;
        while (d->d_name[tmplt_size] != '\0' && d->d_name[tmplt_size] != '.') tmplt_size++;
        char *tmplt_name = (char *)malloc(tmplt_size + 1);
        for (int j = 0; j < tmplt_size + 1; j++) tmplt_name[j] = d->d_name[j];
        tmplt_name[tmplt_size] = '\0';
        out[i++] = tmplt_name;
    }
    *size = tmplt_count;
    closedir(dir);
    return out;
}

void free_template_list(char **tmplt_list, size_t size){
    for (int i = 0; i < size; i++) free(tmplt_list[i]);
    free(tmplt_list);
}

void print_template_list(){
    size_t size = 0;
    char **template_list = get_template_list(&size);
    if (size > 0) printf("You have %d templates saved:\n", (int)size);
    for (int i = 0; i < size; i++) printf("%s\n", template_list[i]);
    if (size == 0){
        printf("You have ");
        bprint("0");
        printf(" saved templates.\n");
        printf("Do:\n\tproy template new <template_name> <path/to/template>\n");
        printf("To create a new template!\n");
    }
    free_template_list(template_list, size);
}

int check_template_exists(char *template_name){
    char *template_path = get_template_path(template_name);

    int exists = 0;
    if (access(template_path, F_OK) == 0) exists = 1;

    free(template_path);
    return exists;
}

int compare_template_to_directory(char *template_name, char *directory_path){
    if (!check_template_exists(template_name)){
        print_template_does_not_exist(template_name);
        return 0;
    }
    struct archive *a;
    a = archive_read_new();
    if (a == NULL) return 0;

    char *template_path = get_template_path(template_name);
    archive_read_support_format_zip(a);
    archive_read_open_filename(a, template_path, 10240);
    free(template_path);
    
    struct archive_entry *ae;
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK){
        const char *path_name = archive_entry_pathname(ae);
        char *full_path = expand_path(directory_path, path_name);
        if (access(full_path, F_OK) != 0) {
            print_lacks_path(full_path);
            free(full_path);
            archive_read_close(a);
            archive_free(a);
            return 0;
        }
        free(full_path);
    }

    archive_read_close(a);
    archive_free(a);
    return 1;
}

char *get_template_from_dir(char *directory_path){
    size_t size = 0;
    char **templates = get_template_list(&size);
    for (int i = 0; i < size; i++){
        if (compare_template_to_directory(templates[i], directory_path)){
            char *out = strdup(templates[i]);
            free_template_list(templates, size);
            return out;
        }
    }
    return NULL;
}

void generate_template_from_directory(char *template_name, char *directory_path){
    if (access(directory_path, F_OK) != 0){
        printf("No such path: ");
        bprint(directory_path);
        printf("...\nExiting...\n");
        return;
    }
    char *zip_path = get_template_path(template_name);
    printf("Generating template from: %s\n", directory_path);
    printf("Writing template to: %s...\n", zip_path);
    compress_directory(directory_path, zip_path);
    printf("Succesfully generated template '%s'!\n", template_name);
    free(zip_path);
}

void delete_template(char *template_name){
    if (check_template_exists(template_name) == 0){
        print_template_does_not_exist(template_name);
        return;
    }
    char *template_path = get_template_path(template_name);

    remove(template_path);
    printf("Succesfully removed the template ");
    bprint(template_name);
    printf("!\n");

    free(template_path);
}

int unpack_template_to_dir(char *template_name, char *proy_path){
    if (check_template_exists(template_name) == 0){
        print_template_does_not_exist(template_name);
        return 0;
    }
    char *template_path = get_template_path(template_name);
    decompress_file_to(template_path, proy_path);
    free(template_path);
    return 1;
}
