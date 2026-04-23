#include "runtime_diagnostics.h"
#include <stdio.h>

void Test_RuntimeDiag(void)
{
    if (RuntimeDiagnostics_RunPeriodic())
        printf("Runtime Diagnostics PASS\n");
    else
        printf("Runtime Diagnostics FAIL\n");
}