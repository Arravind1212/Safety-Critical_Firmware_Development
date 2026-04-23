#include <stdio.h>

extern double score(double *input);

void Test_SOC_AI(void)
{
    double input[3] = {3.7, 1.5, 25.0};

    double soc = score(input);

    printf("Predicted SOC = %f\n", soc);
}