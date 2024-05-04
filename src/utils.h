#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION_STR "v0.1.0"

void usage(char *s);
int create_directory(const char *path);

#endif // __UTILS_H__