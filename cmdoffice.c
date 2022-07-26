#include "cmdoffice.h"

int office_register_command(char *cmd_name, cmd_ptr ptr)
{
    if(cmd_counts == MAX_CMD) return -1;

    cmd_names[cmd_counts] = cmd_name;
    ptrs[cmd_counts++] = ptr;
    return 1;
}

void office_work(char *argv0)
{
    app_info_init(argv0);
    create_commands();
}


