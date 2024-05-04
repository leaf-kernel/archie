#include "utils.h"

void usage(char *s)
{
    printf("Usage: %s [OPTION]... [OUTPUT DIR]\n", s);
    printf("Archie, the new way to generate build systems for your kernel!\n");
    printf("Options:\n");
    printf("  -h, --help               display this help and exit\n");
    printf("  -v, --version            output version information and exit\n");
    printf("  -C, --config             sets the config file, default: \"config.archie\"\n");
}