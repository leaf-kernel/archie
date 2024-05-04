#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

void usage(char *s)
{
    printf("Usage: %s [OPTION]... [OUTPUT DIR]\n", s);
    printf("Archie, the new way to generate build systems for your kernel!\n");
    printf("Options:\n");
    printf("  -h, --help               display this help and exit\n");
    printf("  -v, --version            output version information and exit\n");
    printf("  -V, --verbose            runs Archie in verbose mode\n");
    printf("  -C, --config             sets the config file, default: \"config.archie\"\n");
}

int create_directory(const char *path)
{
    char *tmp_path = strdup(path);
    if (tmp_path == NULL)
    {
        perror("strdup");
        return 0;
    }

    char *last_slash = strrchr(tmp_path, '/');
    if (last_slash != NULL)
    {
        *last_slash = '\0';
        if (!create_directory(tmp_path))
        {
            free(tmp_path);
            return 0;
        }
        *last_slash = '/';
    }

    free(tmp_path);

    if (mkdir(path, 0777) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mkdir");
            return 0;
        }
    }

    return 1;
}