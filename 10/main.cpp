#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

struct light_point {
    int x;
    int y;
    int dx;
    int dy;
};

char *readUntilCharacter (char *currentLetter, char *currentWord, char character) {
    int letterIndex = 0;
    while (*currentLetter != character && *currentLetter != '\n' && currentLetter != 0) {
        currentWord[letterIndex] = *currentLetter;
        letterIndex++;
        currentLetter++;
    }
    currentWord[letterIndex] = 0;
    return currentLetter;
}

char *readPastCharacters (char *currentLetter, char *currentWord, char character) {
    int letterIndex = 0;
    while (*currentLetter == character && *currentLetter != '\n' && currentLetter != 0) {
        currentWord[letterIndex] = *currentLetter;
        letterIndex++;
        currentLetter++;
    }
    currentWord[letterIndex] = 0;
    return currentLetter;
}

int main (int argc, char **argv) {
    light_point *lightPoints = (light_point *)malloc(1000 * sizeof(light_point));
    int numLightPoints = 0;

    const int stringLength = 100;
    char wholeInput[stringLength];

    while(fgets(wholeInput, stringLength, stdin)) {
        char *currentLetter = wholeInput;
        char word[stringLength] = {};

        currentLetter = readUntilCharacter(currentLetter, word, '<');
        ++currentLetter;
        currentLetter = readPastCharacters(currentLetter, word, ' ');
        //bool negative = false;
        //if (*currentLetter == '-') {
        //    negative = true;
        //}

        currentLetter = readUntilCharacter(currentLetter, word, ',');
        int x = atoi(word);
        ++currentLetter;

        ++currentLetter;
        currentLetter = readPastCharacters(currentLetter, word, ' ');
        currentLetter = readUntilCharacter(currentLetter, word, '>');
        int y = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, '<');
        ++currentLetter;
        currentLetter = readPastCharacters(currentLetter, word, ' ');
        currentLetter = readUntilCharacter(currentLetter, word, ',');
        int dx = atoi(word);

        ++currentLetter;
        currentLetter = readPastCharacters(currentLetter, word, ' ');
        currentLetter = readUntilCharacter(currentLetter, word, '>');
        int dy = atoi(word);

        ASSERT(numLightPoints < 1000);
        light_point *lightPoint = &lightPoints[numLightPoints];
        numLightPoints++;

        lightPoint->x = x;
        lightPoint->y = y;
        lightPoint->dx = dx;
        lightPoint->dy = dy;
    }

    //for (int i = 0; i < numLightPoints; ++i) {
    //    light_point *point = &lightPoints[i];
    //    printf("pos: %d, %d  vel: %d, %d\n", point->x, point->y, point->dx, point->dy);
    //}

    //int skyDim = 100;
    //bool *sky = (bool *)malloc(skyDim * skyDim * sizeof(bool));
    //for (int t = 0; t < 1000; ++t) {
    //    memset(sky, 0, skyDim * skyDim * sizeof(bool));
    //    for (int lightIndex = 0; lightIndex < numLightPoints; ++lightPoints) {
    //        light_point *point = &lightPoints[lightIndex];
    //        int x = point->x + skyDim;
    //        int y = point->y + skyDim;
    //        if (x >= 0 && x < skyDim && y >= 0 && y < skyDim) {
    //            sky[y * skyDim + x] = true;
    //        }

    //        point->x += point->dx;
    //        point->y += point->dy;
    //    }

    //    for (int y = 0; y < skyDim; ++y) {
    //        for (int x = 0; x < skyDim; ++x) {
    //            if (sky[y * skyDim + x]) {
    //                printf("#");
    //            }
    //            else {
    //                printf(".");
    //            }
    //        }
    //        printf("\n");
    //    }
    //}

    int skyHalfWidth = 200;
    bool *skyRow = (bool *)malloc(skyHalfWidth * 2 * sizeof(bool));
    for (int t = 0; t < 20000; ++t) {
        printf("time: %d\n", t);
        for (int row = 0; row < skyHalfWidth*2; ++row) {
            memset(skyRow, 0, skyHalfWidth * 2 * sizeof(bool));

            bool written = false;
            for (int lightIndex = 0; lightIndex < numLightPoints; ++lightIndex) {
                light_point *point = &lightPoints[lightIndex];
                int x = point->x + skyHalfWidth;
                int y = point->y + skyHalfWidth;
                if (y == row) {
                    skyRow[x] = true;
                    written = true;
                }
            }

            if (written) {
                for (int x = 0; x < skyHalfWidth * 2; ++x) {
                    if (skyRow[x]) {
                        printf("#");
                    }
                    else {
                        printf(".");
                    }
                }

                printf("\n");
            }
        }
        for (int lightIndex = 0; lightIndex < numLightPoints; ++lightIndex) {
            light_point *point = &lightPoints[lightIndex];
            point->x += point->dx;
            point->y += point->dy;
        }
    }

    //for (int t = 0; t < 5; ++t) {
    //    for (int y = 0; y < skyDim; ++y) {
    //        for (int x = 0; x < skyDim; ++x) {
    //            bool lit = false;
    //            for (int i = 0; i < numLightPoints; ++i) {
    //                light_point *lightPoint = &lightPoints[i];
    //                if (lightPoint->x == x && lightPoint->y == y) {
    //                    lit = true;
    //                    break;
    //                }
    //            }
    //            if (lit) {
    //                printf("#");
    //            }
    //            else {
    //                printf(".");
    //            }
    //        }
    //        printf("\n");
    //    }
    //    for (int i = 0; i < numLightPoints; ++i) {
    //        light_point *lightPoint = &lightPoints[i];
    //        lightPoint->x += lightPoint->dx;
    //        lightPoint->y += lightPoint->dy;
    //    }
    //}
    return 0;
}
