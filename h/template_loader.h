
#ifndef TEMPLATE_LOADER_H
#define TEMPLATE_LOADER_H

#include "oslib/wimp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// You can also define constants or macros here
#define MAX_TEMPLATE_NAME_LEN 13

// Add your data structures
typedef struct
{
    char *window_name;
    wimp_window *window_def;
} template_entry;

// Function declarations
template_entry **load_templates(const char *filename);
//void free_templates(template_entry *templates, int count);

#endif // MYHEADER_H