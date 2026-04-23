#include "pec15.h"
#include <stdio.h>

void Test_PEC15(void)
{
    uint8_t data[6] = {0xFC,0,0,0,0,0};
    uint16_t expected = 0x07C2;

    uint16_t calc = PEC15_Calculate(data, 6);

    if (calc == expected)
        printf("PEC PASS\n");
    else
        printf("PEC FAIL\n");
}