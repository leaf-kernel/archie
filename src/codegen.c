#include "codegen.h"
#include "archie.h"
#include "utils.h"
#include "config.h"
#include "cmake.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

bool _str_to_bool(const char *str);

char *_get_arches()
{
    ConfigEntry **section_entries = get_section_ptr("Info");
    if (section_entries != NULL)
    {
        for (int i = 0; section_entries[i] != NULL; i++)
        {
            if (strcmp(section_entries[i]->name, "architectures") == 0)
            {
                return section_entries[i]->value;
            }
        }
        free(section_entries);
    }
    else
    {
        printf("[Archie] \x1B[31mERROR\x1B[0m Could not find an array of supported architectures.\n");
    }

    return NULL;
}

bool _does_target_use_cmake()
{

    ConfigEntry **section_entries = get_section_ptr("Compiling");
    if (section_entries != NULL)
    {
        for (int i = 0; section_entries[i] != NULL; i++)
        {
            if (strcmp(section_entries[i]->name, "cmake") == 0)
            {
                return _str_to_bool(section_entries[i]->value);
            }
        }
        free(section_entries);
    }
    else
    {
        printf("[Archie] \x1B[31mERROR\x1B[0m Could not find a flag for CMAKE defaulting to false.\n");
    }

    return false;
}

bool _does_target_use_make()
{

    ConfigEntry **section_entries = get_section_ptr("Compiling");
    if (section_entries != NULL)
    {
        for (int i = 0; section_entries[i] != NULL; i++)
        {
            if (strcmp(section_entries[i]->name, "make") == 0)
            {
                return _str_to_bool(section_entries[i]->value);
            }
        }
        free(section_entries);
    }
    else
    {
        printf("[Archie] \x1B[31mERROR\x1B[0m Could not find a flag for MAKE defaulting to false.\n");
    }

    return false;
}

char *_get_cmake_build_tool()
{
    char *tool = NULL;
    ConfigEntry **section_entries = get_section_ptr("Compiling");
    if (section_entries != NULL)
    {
        for (int i = 0; section_entries[i] != NULL; i++)
        {
            if (strcmp(section_entries[i]->name, "cmake_build_tool") == 0)
            {
                return section_entries[i]->value;
            }
        }
        free(section_entries);
    }
    else
    {
        printf("[Archie] \x1B[31mERROR\x1B[0m Could not find a cmake_build_tool.\n");
    }

    return tool;
}

char **_split_arches(const char *arches)
{
    int count = 1;
    const char *temp = arches;
    while (*temp)
    {
        if (*temp == ',')
            count++;
        temp++;

        if (*temp == ' ')
            temp++;
    }

    char **arch_array = (char **)malloc((count + 1) * sizeof(char *));
    if (arch_array == NULL)
    {
        return NULL;
    }

    char *token;
    int i = 0;
    char *arches_copy = strdup(arches);
    if (arches_copy == NULL)
    {
        free(arch_array);
        return NULL;
    }

    while (isspace(*arches_copy))
    {
        arches_copy++;
    }

    token = strtok(arches_copy, ",");
    while (token != NULL)
    {
        while (isspace(*token))
        {
            token++;
        }

        char *end = token + strlen(token) - 1;
        while (end > token && isspace(*end))
        {
            *end = '\0';
            end--;
        }

        arch_array[i++] = strdup(token);
        token = strtok(NULL, ",");
    }
    arch_array[i] = NULL;

    free(arches_copy);

    return arch_array;
}

bool _str_to_bool(const char *str)
{
    if (str == NULL)
    {
        return false;
    }

    char lowercase[strlen(str) + 1];
    strcpy(lowercase, str);
    for (int i = 0; lowercase[i]; i++)
    {
        lowercase[i] = tolower(lowercase[i]);
    }

    if (strcmp(lowercase, "true") == 0)
    {
        return true;
    }

    if (strcmp(lowercase, "false") == 0)
    {
        return false;
    }

    return false;
}

bool codegen(const char *out_dir)
{
    char *arches = _get_arches();

    if (arches == NULL)
    {
        printf("[Archie] \x1B[31mERROR\x1B[0m Could find list of architectures! Make sure to add a \"architectures\" entry to Info\n");
        return false;
    }

    bool make = _does_target_use_make();
    bool cmake = _does_target_use_cmake();

    if (cmake)
    {
        printf("[Archie] \x1B[34mINFO\x1B[0m Configuring to use CMAKE as build system!\n");
    }
    else
    {
        printf("[Archie] \x1B[33mWARNING\x1B[0m Not using CMAKE as build system, checking for MAKE!\n");
        if (make)
        {
            printf("[Archie] \x1B[34mINFO\x1B[0m Configuring to use MAKE as build system!\n");
        }
        else
        {
            printf("[Archie] \x1B[31mERROR\x1B[0m Didn't find any build systems!\n");
            return false;
        }
    }
    if (!cmake)
    {
        printf("[Archie] \x1B[31mERROR\x1B[0m Archie only supports build file generation for CMAKE!\n");
        return false;
    }

    char *tool = _get_cmake_build_tool();

    if (_verbose)
        printf("[Archie] \x1B[34mINFO\x1B[0m Build system: %s, Build tool: %s\n", cmake ? "CMAKE" : "MAKE", tool);

    char **arch_array = _split_arches(arches);
    if (arch_array == NULL)
    {
        printf("[Archie] \x1B[31mERROR\x1B[0m Couldn't split architectures!\n");
        return false;
    }

    for (int i = 0; arch_array[i] != NULL; i++)
    {
        char *arch = arch_array[i];
        char *target_full = (char *)malloc(strlen("target.") + strlen(arch) + 2);

        char *cc = NULL;
        char *cxx = NULL;
        char *as = NULL;
        char *ld = NULL;
        char *ld_file = NULL;
        char *nasm_obj = NULL;

        bool nasm = false;

        if (target_full == NULL)
        {
            printf("[Archie] \x1B[31mERROR\x1B[0m Failed to allocate memory for the target_full array!\n");
            free(arch_array);
            return false;
        }

        sprintf(target_full, "arch.%s", arch);

        if (_verbose)
            printf("[Archie] \x1B[34mINFO\x1B[0m TARGET_FULL: %s\n", target_full);

        ConfigEntry **section_entries = get_section_ptr(target_full);
        if (section_entries != NULL)
        {
            for (int i = 0; section_entries[i] != NULL; i++)
            {
                if (strcmp(section_entries[i]->name, "cc") == 0)
                {
                    cc = section_entries[i]->value;
                }
                else if (strcmp(section_entries[i]->name, "cxx") == 0)
                {
                    cxx = section_entries[i]->value;
                }
                else if (strcmp(section_entries[i]->name, "as") == 0)
                {
                    as = section_entries[i]->value;
                }
                else if (strcmp(section_entries[i]->name, "ld") == 0)
                {
                    ld = section_entries[i]->value;
                }
                else if (strcmp(section_entries[i]->name, "ld_file") == 0)
                {
                    ld_file = section_entries[i]->value;
                }
                else if (strcmp(section_entries[i]->name, "nasm_obj") == 0)
                {
                    nasm_obj = section_entries[i]->value;
                }
            }

            free(section_entries);
        }
        else
        {
            cc = (char *)malloc(strlen(arch) + 10);
            cxx = (char *)malloc(strlen(arch) + 10);
            as = (char *)malloc(strlen(arch) + 9);
            ld = (char *)malloc(strlen(arch) + 9);
            ld_file = (char *)malloc(strlen(arch) + 11);

            if (cc == NULL || cxx == NULL || as == NULL || ld == NULL || ld_file == NULL)
            {
                printf("[Archie] \x1B[31mERROR\x1B[0m Memory allocation failed!\n");
                return false;
            }

            snprintf(cc, strlen(arch) + 10, "%s-elf-gcc", arch);
            snprintf(cxx, strlen(arch) + 10, "%s-elf-gcc", arch);
            snprintf(as, strlen(arch) + 9, "%s-elf-as", arch);
            snprintf(ld, strlen(arch) + 9, "%s-elf-ld", arch);
            snprintf(ld_file, strlen(arch) + 11, "%slinker.ld", arch);

            printf("[Archie] \x1B[33mWARNING\x1B[0m Could not find \"%s\" in config file. Adding it anyways since its in the architecture array\n", target_full);
        }

        printf("[Archie] \x1B[37mDBG\x1B[0m %s CC: %s\n", arch, cc);
        printf("[Archie] \x1B[37mDBG\x1B[0m %s CXX: %s\n", arch, cxx);
        printf("[Archie] \x1B[37mDBG\x1B[0m %s AS: %s\n", arch, as);
        printf("[Archie] \x1B[37mDBG\x1B[0m %s LD: %s\n", arch, ld);
        printf("[Archie] \x1B[37mDBG\x1B[0m %s LD_FILE: %s\n", arch, ld_file);

        if (strcmp(as, "nasm") == 0)
        {
            printf("[Archie] \x1B[37mDBG\x1B[0m %s ASM_TYPE: ASM_NASM\n", arch);
            nasm = true;
        }
        else
        {
            nasm = false;
        }

        printf(".........................\n");

        char *toolchain_file_path;
        asprintf(&toolchain_file_path, "%s/arch/%s/toolchain.cmake", out_dir, arch);
        char *arch_path;
        asprintf(&arch_path, "%s/arch/%s", out_dir, arch);
        if (!create_directory(arch_path))
        {
            printf("[Archie] \x1B[31mERROR\x1B[0m Failed to create directory %s!\n", arch_path);
            return false;
        }

        if (toolchain_file_path == NULL)
        {
            printf("[Archie] \x1B[31mERROR\x1B[0m Failed to allocate memory for the toolchain file path!\n");
            return false;
        }

        printf("[Archie] \x1B[34mINFO\x1B[0m %s\n", toolchain_file_path);
        printf("[Archie] \x1B[34mINFO\x1B[0m %s\n", arch_path);
        FILE *toolchain_file = fopen(toolchain_file_path, "w");

        if (toolchain_file == NULL)
        {
            printf("[Archie] \x1B[31mERROR\x1B[0m Failed to create toolchain file for %s!\n", arch);
            return false;
        }

        // Generic CMAKE flags for cross compiling
        fprintf(toolchain_file, "%s", _set_cmake_flag("CMAKE_SYSTEM_NAME", "Generic"));
        fprintf(toolchain_file, "%s", _set_cmake_flag("CMAKE_CROSSCOMPILING", "TRUE"));
        fprintf(toolchain_file, "%s", _set_cmake_flag("CMAKE_C_COMPILER_WORKS", "TRUE"));
        fprintf(toolchain_file, "%s", _set_cmake_flag("CMAKE_CXX_COMPILER_WORKS", "TRUE"));
        fprintf(toolchain_file, "%s", _set_cmake_flag("CMAKE_ASM_COMPILER_WORKS", "TRUE"));

        if (cc != NULL)
        {
            fprintf(toolchain_file, "%s", _set_cmake_flag("CMAKE_C_COMPILER", cc));
        }

        if (cxx != NULL)
        {
            fprintf(toolchain_file, "%s", _set_cmake_flag("CMAKE_CXX_COMPILER", cxx));
        }

        if (ld != NULL)
        {
            fprintf(toolchain_file, "%s", _set_cmake_flag("CMAKE_LINKER", ld));
        }

        if (nasm && nasm_obj != NULL)
        {
            fprintf(toolchain_file, "%s", _set_cmake_flag("CMAKE_ASM_NASM_COMPILER", "nasm"));
            fprintf(toolchain_file, "%s", _set_cmake_flag("CMAKE_ASM_NASM_OBJECT_FORMAT", nasm_obj));
        }
        else if (nasm_obj == NULL && nasm)
        {
            printf("[Archie] \x1B[31mERROR\x1B[0m NASM Was specified as the assembler (for %s), but no object format was specified!\n", arch);
            return false;
        }

        // Close the file since we are done
        fclose(toolchain_file);
        printf(".........................\n\n");

        free(target_full);
    }
    free(arch_array);
    return true;
}
