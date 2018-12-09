#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

int processedStringLength (char *string) {
    char *currentLetter = string;
    int count = 0;
    while(*currentLetter) {
        if (*currentLetter != '[' && *currentLetter != '\n') {
            count++;
        }
        ++currentLetter;
    }
    return count;
}

// 'a' = 97
// 'A' = 65
int main (int argc, char **argv) {
    // read events
    char *polymer = (char *)malloc(60000 * sizeof(char));
    fgets(polymer, 60000, stdin);

    char letterPairs[][2] = {
        {'a', 'A'},
        {'b', 'B'},
        {'c', 'C'},
        {'d', 'D'},
        {'e', 'E'},
        {'f', 'F'},
        {'g', 'G'},
        {'h', 'H'},
        {'i', 'I'},
        {'j', 'J'},
        {'k', 'K'},
        {'l', 'L'},
        {'m', 'M'},
        {'n', 'N'},
        {'o', 'O'},
        {'p', 'P'},
        {'q', 'Q'},
        {'r', 'R'},
        {'s', 'S'},
        {'t', 'T'},
        {'u', 'U'},
        {'v', 'V'},
        {'w', 'W'},
        {'x', 'X'},
        {'y', 'Y'},
        {'z', 'Z'}
    };

    char *polymerCopy = (char *)malloc(60000 * sizeof(char));
    int bestLength = 100000;
    for (int i = 0; i < 26; ++i) {
        memcpy(polymerCopy, polymer, 60000);

        char *letterPair = letterPairs[i];
        char *currentLetter = polymerCopy;
        while (*currentLetter) {
            if (*currentLetter == letterPair[0] || *currentLetter == letterPair[1]) {
                *currentLetter = '[';
            }
            ++currentLetter;
        }

        bool changed = false; 
        do {
            changed = false;
            currentLetter = polymerCopy;
            while (*currentLetter) {
                char *nextLetter = currentLetter + 1;
                while (*nextLetter == '[') {
                    nextLetter++;
                }
                if ((*currentLetter > 90 && *nextLetter == *currentLetter - 32) ||
                    (*currentLetter <= 90 && *nextLetter == *currentLetter + 32)) 
                {
                    changed  = true;
                    *currentLetter = '['; // "empty" character
                    *nextLetter = '['; // "empty" character
                    nextLetter++;
                }
                currentLetter = nextLetter;
            }
        } while(changed);

        int length = processedStringLength(polymerCopy);
        if (length < bestLength) {
            bestLength = length;
        }
    }


    printf("%d\n", bestLength);
    return 0;
}
