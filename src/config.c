#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_config(void *user, const char *section, const char *name, const char *value)
{
    static char prev_section[50] = "";

    if (strcmp(section, prev_section))
    {
        printf("%s[%s]\n", (prev_section[0] ? "\n" : ""), section);
        strncpy(prev_section, section, sizeof(prev_section));
        prev_section[sizeof(prev_section) - 1] = '\0';
    }
    printf("%s = %s\n", name, value);
    return 1;
}