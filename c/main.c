#include "oslib/os.h"
#include "oslib/wimp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sflib/debug.h"

#include "template_loader.h"

int main(void)
{
    wimp_t task;
    template_entry **pTemplates;
    wimp_block block;
    wimp_event_no reason;
    osbool quit = FALSE;

    os_error *pError = xwimp_initialise(wimp_VERSION_RO38, "MyApp", NULL, NULL, &task);
    if (pError != NULL)
    {
        fprintf(stderr, "Failed to initialise Wimp\n");
        return 1;
    }

    pTemplates = load_templates("<WIMPApp$Dir>.Templates");

    if (pTemplates == NULL)
    {
        debug_printf("Failed to load templates\n");
        return 1;
    }

    int found_index = -1;               // Default: not found
    size_t search_len = strlen("Main"); // Length of the name to match

    for (int i = 0; pTemplates[i] != NULL; i++)
    { // Assumes NULL-terminated array
        // Compare first 'search_len' chars (ignores trailing \r or other garbage)
        if (strncmp(pTemplates[i]->window_name, "Main", search_len) == 0)
        {
            found_index = i;
            break; // Exit early when found
        }
    }

    if (found_index != -1)
    {
        debug_printf("Found at index %d\n", found_index);
    }
    else
    {
        debug_printf("Not found!\n");
        return 1;
    }

    wimp_w window_handle = wimp_create_window(pTemplates[found_index]->window_def);
    if (window_handle == NULL)
    {
        debug_printf("Failed to create window\n");
        return 1;
    }

    wimp_window_state state;
    state.w = window_handle;
    state.next = wimp_TOP;
    wimp_open_window((wimp_open *)&state);
    debug_printf("Window opened successfully\n");
    debug_printf("Template name %s\n", pTemplates[0]->window_name);
    debug_printf("has %x icons\n", pTemplates[0]->window_def->icon_count);

    while (!quit)
    {
        reason = wimp_poll(wimp_MASK_NULL | wimp_MASK_ENTERING |
                               wimp_MASK_LEAVING | wimp_MASK_GAIN |
                               wimp_MASK_LOSE | wimp_MASK_POLLWORD,
                           &block, NULL);

        switch (reason)
        {
        case wimp_USER_MESSAGE:
        case wimp_USER_MESSAGE_RECORDED:
            if (block.message.action == message_QUIT)
                quit = TRUE;
            break;
        }
    }

    return 0;
}
