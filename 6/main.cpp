#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

struct coordinate {
    int x;
    int y;
};

char *readUntilCharacter (char *currentLetter, char *currentWord, char character) {
    int letterIndex = 0;
    while (*currentLetter != character && currentLetter != 0) {
        currentWord[letterIndex] = *currentLetter;
        letterIndex++;
        currentLetter++;
    }
    currentWord[letterIndex] = 0;
    return currentLetter;
}

#define GRID_SIZE 400

int main (int argc, char **argv) {
    int *closestCoordinatesGrid = (int *)malloc(GRID_SIZE * GRID_SIZE * sizeof(int));
    coordinate *coordinates = (coordinate *)malloc(50 * sizeof(coordinate));
    int numCoordinates = 0;

    const int stringLength = 10;
    char word[stringLength];
    while (fgets(word, stringLength, stdin)) {
        coordinate *coord = &coordinates[numCoordinates];
        numCoordinates++;

        char *currentLetter = word;
        char coordNum[10];
        currentLetter = readUntilCharacter(currentLetter, coordNum, ',');
        coord->x = atoi(coordNum);
        currentLetter += 2;

        currentLetter = readUntilCharacter(currentLetter, coordNum, '\n');
        coord->y = atoi(coordNum);
    }

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            int x = j;
            int y = i;

            for (int coordIndex = 0; coordIndex < numCoordinates; ++coordIndex) {
                coordinate *coord = &coordinates[coordIndex];

                int xDist = coord->x - x;
                int yDist = coord->y - y;
                if (xDist < 0) { xDist = -xDist; }
                if (yDist < 0) { yDist = -yDist; }
                int dist = xDist + yDist;

                closestCoordinatesGrid[i * GRID_SIZE + j] += dist;
            }

        }
    }

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            int totalDist = closestCoordinatesGrid[i * GRID_SIZE + j];
            if (totalDist >= 10000) {
                closestCoordinatesGrid[i * GRID_SIZE + j] = 0;
            }
            else {
                closestCoordinatesGrid[i * GRID_SIZE + j] = 1;
            }
        }
    }

    int regionSize = 0;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            regionSize += closestCoordinatesGrid[i * GRID_SIZE + j];
        }
    }
    printf("%d\n", regionSize);

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            printf("%d", closestCoordinatesGrid[i * GRID_SIZE + j]);
        }
        printf("\n");
    }

    //int bestSize = 0;
    //int bestIndex = 0;
    //for (int i = 0; i < numCoordinates; ++i) {
    //    int size = areaSizes[i];
    //    if (size > bestSize) {
    //        bestSize = size;
    //        bestIndex = i;
    //    }
    //}
    //printf("%d, %d\n", bestSize, bestIndex);

    //char letters[] = {
    //    'a',
    //    'b',
    //    'c',
    //    'd',
    //    'e',
    //    'f',
    //    'g',
    //    'h',
    //    'i',
    //    'j',
    //    'k',
    //    'l',
    //    'm',
    //    'n',
    //    'o',
    //    'p',
    //    'q',
    //    'r',
    //    's',
    //    't',
    //    'u',
    //    'v',
    //    'w',
    //    'x',
    //    'y',
    //    'z',
    //    'A',
    //    'B',
    //    'C',
    //    'D',
    //    'E',
    //    'F',
    //    'G',
    //    'H',
    //    'I',
    //    'J',
    //    'K',
    //    'L',
    //    'M',
    //    'N',
    //    'O',
    //    'P',
    //    'Q',
    //    'R',
    //    'S',
    //    'T',
    //    'U',
    //    'V',
    //    'W',
    //    'X',
    //    'Y',
    //    'Z'
    //};
    //int currentLetter = 0;
    //for (int i = 0; i < GRID_SIZE; ++i) {
    //    for (int j = 0; j < GRID_SIZE; ++j) {
    //        bool printedLetter= false;
    //        for (int c = 0; c < numCoordinates; ++c) {
    //            coordinate *coord = &coordinates[c];
    //            if (i == coord->y && j == coord->x) {
    //                printf(" %c", letters[currentLetter]);
    //                currentLetter++;
    //                printedLetter = true;
    //                break;
    //            }
    //        }
    //        if (!printedLetter) {
    //            int bestCoord = closestCoordinatesGrid[i * GRID_SIZE + j];
    //            if (bestCoord == -1) {
    //                printf("..");
    //            }
    //            else {
    //                if (bestCoord < 10) {
    //                    printf("%d", 0);
    //                }
    //                printf("%d", bestCoord);
    //            }
    //        }
    //    }
    //    printf("\n");
    //}

    //for (int i = 0; i < numCoordinates; ++i) {
    //    coordinate *coord = &coordinates[i];
    //    printf("%d, %d\n", coord->x, coord->y);
    //}

    return 0;
}
