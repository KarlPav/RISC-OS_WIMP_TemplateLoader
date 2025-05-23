#include "oslib/os.h"
#include "oslib/wimp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sflib/debug.h"

#include "template_loader.h"

/*This code will cycle through the templates in the template file
It does so by iterating through the context values.
Note that when a template is found its name is returned in the "name" argument
thats why its reset back to "*"
The function returns a struct that holds the template name and window definition*/

/*Eventually I will try to make a "wimp_window" class that:
contains all the window and icon defintions indexed to name
has functions to return current window and icon states
as well as set states*/

template_entry **load_templates(const char *filename)
{
    os_error *pError;
    byte *pInd_data = NULL;
    wimp_window *pWindow_definition = NULL;
    template_entry **pTemplates = NULL;

    int i = 0;
    int def_size;
    int ind_size;
    int context = 0;
    int context_out;

    char name[MAX_TEMPLATE_NAME_LEN]; // 12 characters + null terminator
    memset(name, 0, sizeof(name));
    strcpy(name, "*"); // Wildcard to match all templates

    // Open the template file
    debug_printf("Opening template file: %s\n", filename);
    pError = xwimp_open_template(filename);
    if (pError)
    {
        debug_printf("Error opening template: %s\n", pError->errmess);
        return NULL;
    }

    pTemplates = malloc(sizeof(template_entry *));
    if (!pTemplates)
    {
        debug_printf("Memory allocation for template entry failed.\n");
        return NULL;
    }

    while (TRUE)
    {

        // First call to determine required memory sizes
        pError = xwimp_load_template(wimp_GET_SIZE, 0, 0, wimp_NO_FONTS, name,
                                     context, &def_size, &ind_size, &context_out);
        if (pError)
        {
            debug_printf("Error probing template: %s\n", pError->errmess);
            goto cleanup;
        }

        if (context_out == 0)
        {
            debug_printf("No more templates to load.\n");
            // No more templates to load
            break;
        }

        // Allocate memory for window and icon data
        pWindow_definition = malloc(def_size);
        if (!pWindow_definition)
        {
            debug_printf("Memory allocation failed.\n");
            goto cleanup;
        }
        pInd_data = malloc(ind_size);
        if (!pInd_data && ind_size > 0)
        {
            debug_printf("Memory allocation for icon data failed.\n");
            goto cleanup;
        }

        // Load the window template
        pError = xwimp_load_template(pWindow_definition, (char *)pInd_data, (char const *)pInd_data + ind_size, wimp_NO_FONTS, name,
                                     context, NULL, NULL, &context_out);
        if (pError)
        {
            debug_printf("Failed to load template: %s\n", pError->errmess);
            goto cleanup;
        }

        // Allocate memory for template entry
        template_entry **pTemplates_tmp = realloc(pTemplates, (sizeof(template_entry *) * (i + 1)));
        if (!pTemplates_tmp)
        {
            debug_printf("Memory allocation for template entry failed.\n");
            goto cleanup;
        }

        pTemplates = pTemplates_tmp;
        pTemplates_tmp = NULL;
        pTemplates[i] = malloc(sizeof(template_entry));
        if (!pTemplates[i])
        {
            debug_printf("Memory allocation for template entry failed.\n");
            goto cleanup;
        }

        pTemplates[i]->window_name = malloc(strlen(name) + 1);
        if (!pTemplates[i]->window_name)
        {
            debug_printf("Memory allocation for window name failed.\n");
            goto cleanup;
        }
        name[strcspn(name, "\r\n")] = '\0'; // Remove any trailing newline characters
        strcpy(pTemplates[i]->window_name, name);

        pTemplates[i]->window_def = malloc(sizeof(pWindow_definition));
        if (!pTemplates[i]->window_def)
        {
            debug_printf("Memory allocation for window definition failed.\n");
            goto cleanup;
        }

        pTemplates[i]->window_def = pWindow_definition;
        pWindow_definition = NULL; // Prevent double free
        free(pInd_data);
        pInd_data = NULL;

        strcpy(name, "*"); // Wildcard to match all templates
        context = context_out;

        i++;
    }

    xwimp_close_template();

    return pTemplates;

cleanup:
    // Centralized cleanup
    if (pTemplates)
    {
        for (int j = 0; j < i; j++)
        {
            if (pTemplates[j])
            {
                free(pTemplates[j]->window_name);
                free(pTemplates[j]->window_def);
                free(pTemplates[j]);
            }
        }
        free(pTemplates);
    }
    free(pWindow_definition);
    free(pInd_data);
    xwimp_close_template();
    return NULL;
};