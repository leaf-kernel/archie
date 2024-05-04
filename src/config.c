#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ConfigEntry *config_entries = NULL;
int config_entry_count = 0;

int parse_config(void *user, const char *section, const char *name, const char *value)
{
    (void)user;
    ConfigEntry *entry = realloc(config_entries, (config_entry_count + 1) * sizeof(ConfigEntry));
    if (entry == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        return 0;
    }
    config_entries = entry;

    config_entries[config_entry_count].section = strdup(section);
    config_entries[config_entry_count].name = strdup(name);
    config_entries[config_entry_count].value = strdup(value);
    config_entry_count++;

    return 1;
}

void print_config_entries()
{
    printf("Configuration:\n");
    for (int i = 0; i < config_entry_count; i++)
    {
        printf("[%s]\n", config_entries[i].section);
        printf("%s = %s\n", config_entries[i].name, config_entries[i].value);
    }
}

ConfigEntry *lookup_config_entry(const char *section, const char *name)
{
    for (int i = 0; i < config_entry_count; i++)
    {
        if (strcmp(config_entries[i].section, section) == 0 && strcmp(config_entries[i].name, name) == 0)
        {
            return &config_entries[i];
        }
    }
    return NULL; // Entry not found
}
ConfigEntry **get_section_ptr(const char *section)
{
    ConfigEntry **section_entries = malloc((config_entry_count + 1) * sizeof(ConfigEntry *));
    if (section_entries == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    int count = 0;
    int found = 0;
    for (int i = 0; i < config_entry_count; i++)
    {
        if (strcmp(config_entries[i].section, section) == 0)
        {
            section_entries[count++] = &config_entries[i];
            found = 1;
        }
    }

    if (!found)
    {
        free(section_entries);
        return NULL;
    }

    section_entries[count] = NULL;
    section_entries = realloc(section_entries, (count + 1) * sizeof(ConfigEntry *));
    if (section_entries == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    return section_entries;
}
