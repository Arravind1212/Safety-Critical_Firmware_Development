#include "diag_short.h"
#include <stdio.h>

void Test_Short(void)
{
    if (Diag_ShortCheck())
        printf("Short PASS\n");
    else
        printf("Short FAIL\n");
}