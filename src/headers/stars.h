#ifndef stars
#define stars

extern char *STARS_PATH;
void load_stars();
void unload_stars();

void add_star(char *proy_name);
void del_star(char *proy_name);

void open_starred(int);
void set_star(char *proy_name, int index);
void print_stars();

#endif
