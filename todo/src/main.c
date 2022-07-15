#include "include/hl.h"
#include "include/macro.h"
#include "include/io.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
    if ( argc < 2 )
    {
        printf("[usage]: ./hl <file>");
        return 1;
    }

    hl_compile_file(argv[1]);

    return 0;
}
