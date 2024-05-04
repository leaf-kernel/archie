#include "cmake.h"

char *_set_cmake_flag(const char *flag, const char *value)
{
    char *result;
    asprintf(&result, "set(%s %s)\n", flag, value);
    return result;
}