#include <stdio.h> 
#include <stdlib.h> 

int main (int argc, char **argv) {
    const int stringLength = 2000;
    char input[stringLength];

    int *seenValues = (int *)malloc(1000000 * sizeof(int));
    seenValues[500000] = 1;
    int result = 0;

    int *values = (int *)malloc(10000 * sizeof(int));
    int numValues = 0;

    while (fgets(input, stringLength, stdin)) {
        values[numValues] = atoi(input);
        numValues++;
    }
    int i = 0;
    while (true) {
        int num = values[i];
        result += num;
        seenValues[result+500000] += 1;
        if (seenValues[result+500000] >= 2) {
            break;
        }
        ++i;
        if (i == numValues) { i = 0; }
    }
    printf("%d", result);
    return 0;
}
