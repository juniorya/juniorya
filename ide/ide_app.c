/**
 * @file ide_app.c
 * @brief Command line entry point for the desktop IDE utility.
 */

#include "ide_project.h"

#include <stdio.h>

/**
 * @brief Print command line usage information.
 */
static void print_usage(void)
{
    printf("cnc_ide <project.cncp> <output_dir>\n");
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        print_usage();
        return 1;
    }

    const char *project_path = argv[1];
    const char *output_dir = argv[2];

    ide_project project;
    if (!ide_project_load(project_path, &project))
    {
        fprintf(stderr, "Failed to load project: %s\n", project_path);
        return 1;
    }

    if (!ide_project_generate(&project))
    {
        fprintf(stderr, "Failed to generate plan for: %s\n", project.name);
        return 1;
    }

    if (!ide_project_export(&project, output_dir))
    {
        fprintf(stderr, "Failed to export artefacts for: %s\n", project.name);
        return 1;
    }

    printf("Project '%s' exported to %s\n", project.name, output_dir);
    return 0;
}
