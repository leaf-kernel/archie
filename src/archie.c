#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "lib/ini.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {

        usage(argv[0]);
        return 1;
    }

    char *out_dir = NULL;
    char *config_file = NULL;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printf("Archie (%s) %s\n", __DATE__, VERSION_STR);
            return 0;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            usage(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-C") == 0 || strcmp(argv[i], "--config") == 0)
        {
            config_file = argv[++i];
            return 0;
        }
        else
        {
            if (argv[i][0] == '-' || (argv[i][0] == '-' && argv[i][1] == '-'))
            {
                printf("[Archie] \x1B[31mERROR\x1B[0m Invalid option \"%s\"\n", argv[i]);
                return EXIT_FAILURE;
            }
            else
            {
                out_dir = argv[i];
            }
        }
    }

    if (out_dir == NULL)
    {
        printf("[Archie] \x1B[31mERROR\x1B[0m Missing output directory!\n");
        return EXIT_FAILURE;
    }

    if (config_file == NULL)
    {
        printf("[Archie] \x1B[34mINFO\x1B[0m No config file present will be defaulting to \"config.archie\"\n");
        config_file = "config.archie";
        if (access(config_file, F_OK) != 0)
        {
            printf("[Archie] \x1B[31mERROR\x1B[0m Config file \"%s\" does not exist!\n", config_file);
            return EXIT_FAILURE;
        }
    }

    if (mkdir(out_dir, 0755) != 0)
    {
        if (errno == EEXIST)
        {
            printf("[Archie] \x1B[34mINFO\x1B[0m Output directory \"%s\" already exists, will be using that\n", out_dir);
        }
        else
        {
            printf("[Archie] \x1B[31mERROR\x1B[0m Failed to create output directory \"%s\": %s\n", out_dir, strerror(errno));
            return EXIT_FAILURE;
        }
        return EXIT_FAILURE;
    }

    return 0;
}