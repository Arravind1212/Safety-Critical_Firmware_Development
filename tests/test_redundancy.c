#include "diag_redundancy.h"
#include <stdio.h>

void Test_Redundancy(void)
{
    if (Diag_RedundancyCheck())
        printf("Redundancy PASS\n");
    else
        printf("Redundancy FAIL\n");
}