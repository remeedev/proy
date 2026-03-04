#ifndef proy_configuration
#define proy_configuration

void setup_config();
void change_config(char *config_opt, char *setting);
char *config_get(char *property);
int config_get_i(char *property);
void end_config();
void save_config_to(char *path);

#endif
