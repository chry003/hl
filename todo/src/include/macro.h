#pragma once

#include <stdio.h>

#define MAX(a, b)\
    a > b ? a : b

#define MIN(a, b)\
    a < b ? a : b

#define err(x, msg) if(!x) { printf("\n[ERROR]: %s\n", msg); exit(1); }
