#include "diag_openwire.h"
#include <stdio.h>

void Test_OpenWire(void)
{
    if (Diag_OpenWireCheck())
        printf("OpenWire PASS\n");
    else
        printf("OpenWire FAIL\n");
}