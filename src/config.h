#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAX_SECTION_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_VALUE_LENGTH 100
typedef struct
{
    char *section;
    char *name;
    char *value;
} ConfigEntry;

int parse_config(void *user, const char *section, const char *name, const char *value);
void print_config_entries();
ConfigEntry *lookup_config_entry(const char *section, const char *name);
ConfigEntry **get_section_ptr(const char *section);

#endif // __CONFIG_H__