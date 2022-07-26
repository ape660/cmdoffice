#include <stdio.h>

#include "officeprint.h"
#include "cmdoffice.h"


void back(int argc, char* argv[])
{
    printf("a");
}


int main(int argc, char* argv[])
{
    // 
    office_register_command("命令1", back);
    office_register_command("命令2", back);
    office_work(argv[0]);
}