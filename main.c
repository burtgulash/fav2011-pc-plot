#include <stdio.h>
#include <stdlib.h>
#include "plot.h"


int main(int argc, char ** argv)
{
    if (argc == 2) {
        write_ps(stdout, argv[1], "limits");
    }

    return 0;
}
