#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include <windows.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

void printMap (char *map, int mapDim) {
    for (int i = 0; i < mapDim; ++i) {
        for (int j = 0; j < mapDim; ++j) {
            printf("%c", map[i * mapDim + j]);
        }
        printf("\n");
    }
}

void getAdjacentTileTypeCounts (char *map, int mapDim, int x, int y, 
                                int *openCount, int *treeCount, int *lumberYardCount) 
{
    *openCount = 0;
    *treeCount = 0;
    *lumberYardCount = 0;
    for (int i = y - 1; i <= y + 1; ++i) {
        if (i >= 0 && i < mapDim) {
            for (int j = x - 1; j <= x + 1; ++j) {
                if (j >= 0 && j < mapDim) {
                    if (i == y && j == x) { continue; }
                    char tileType = map[i * mapDim + j];
                    switch (tileType) {
                    case '.':
                        (*openCount)++;
                        break;
                    case '|':
                        (*treeCount)++;
                        break;
                    case '#':
                        (*lumberYardCount)++;
                        break;
                    }
                }
            }
        }
    }
}

int main (int argc, char **argv) {
    int mapDim = 50;
    //int mapDim = 10;
    char *map = (char *)malloc(mapDim * mapDim * sizeof(char));
    int numTiles = 0;

    const int stringLength = 60;
    char line[stringLength];
    while (fgets(line, stringLength, stdin)) {
        char *currentLetter = line;
        while (*currentLetter != '\n' && *currentLetter != 0) {
            map[numTiles] = *currentLetter;
            ++numTiles;
            ++currentLetter;
        }
    }
    char *swapMap = (char *)malloc(mapDim * mapDim * sizeof(char));

    int sleepDuration = 100;
    system("cls");
    printMap(map, mapDim);
    Sleep(sleepDuration);

    long long numIterations = 1000000000;
    char *currentMap = map;
    char *nextMap = swapMap;
    int openCount = 0;
    int treeCount = 0;
    int lumberYardCount = 0;
    for (long long iteration= 0; iteration < numIterations; ++iteration) {
        system("cls");
        printf("iteration: %lld\n", iteration);

        for (int i = 0; i < mapDim; ++i) {
            for (int j = 0; j < mapDim; ++j) {
                char tile = currentMap[i * mapDim + j];
                char newTile = tile;
                getAdjacentTileTypeCounts(currentMap, mapDim, j, i, &openCount, &treeCount, &lumberYardCount);
                switch (tile) {
                    case '.':
                    {
                        if (treeCount >= 3) {
                            newTile = '|';
                        }
                    } break;
                    case '|':
                    {
                        if (lumberYardCount >= 3) {
                            newTile = '#';
                        }
                    } break;
                    case '#':
                    {
                        if (lumberYardCount >= 1 && treeCount >= 1) {
                            // stay as a lumberyard
                        }
                        else {
                            newTile = '.';
                        }
                    } break;
                }
                nextMap[i * mapDim + j] = newTile;
            }
        }

        char *temp = nextMap;
        nextMap = currentMap;
        currentMap = temp;

        printMap(currentMap, mapDim);
        Sleep(sleepDuration);

    }

    int totalTreeCount = 0;
    int totalLumberYardCount = 0;
    for (int i = 0; i < mapDim; ++i) {
        for (int j = 0; j < mapDim; ++j) {
            char tileType = currentMap[i * mapDim + j];
            if (tileType == '|') { totalTreeCount++; } 
            else if (tileType == '#') { totalLumberYardCount++; }
        }
    }
    printf("%d\n", totalTreeCount * totalLumberYardCount);

    return 0;
}
