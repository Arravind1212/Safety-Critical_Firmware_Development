#include "diag_spi.h"
#include <stdio.h>

void Test_SPI(void)
{
    if (Diag_SpiCheck())
        printf("SPI PASS\n");
    else
        printf("SPI FAIL\n");
}