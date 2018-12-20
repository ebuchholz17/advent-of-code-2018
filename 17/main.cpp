#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

enum direction {
    DIRECTION_UP,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
};

enum water_step_result {
    WATER_STEP_RESULT_SPAWNED_WATER,
    WATER_STEP_RESULT_DID_NOT_SPAWN_WATER,
    WATER_STEP_RESULT_REACHED_MAX_DEPTH
};

void settleWater (char *map, const int mapLength, int x, int y, direction nextDirection) {
    char tile = map[y * mapLength + x];
    if (tile == '|') {
        map[y * mapLength + x] = '~';
        if (nextDirection == DIRECTION_LEFT) {
            settleWater(map, mapLength, x-1, y, DIRECTION_LEFT);
        }
        else {
            settleWater(map, mapLength, x+1, y, DIRECTION_RIGHT);
        }
    }
}

water_step_result doWaterStep (char *map, const int mapLength, int x, int y, direction lastDirection, int maxY) {
    if (y > maxY) { return WATER_STEP_RESULT_REACHED_MAX_DEPTH; }

    char tile = map[y * mapLength + x];
    switch (tile) {
        default: 
            return WATER_STEP_RESULT_DID_NOT_SPAWN_WATER;
        case '+': 
        {
            return doWaterStep(map, mapLength, x, y+1, DIRECTION_UP, maxY);
        } break;
        case '.': 
        {
            map[y * mapLength + x] = '|';
            return WATER_STEP_RESULT_SPAWNED_WATER;
        } break;
        case '|': 
        {
            water_step_result spawnedWaterUnderneath = doWaterStep(map, mapLength, x, y+1, DIRECTION_UP, maxY);
            if (spawnedWaterUnderneath == WATER_STEP_RESULT_SPAWNED_WATER) { 
                return WATER_STEP_RESULT_SPAWNED_WATER;
            }
            else if (spawnedWaterUnderneath == WATER_STEP_RESULT_REACHED_MAX_DEPTH) { 
                return WATER_STEP_RESULT_REACHED_MAX_DEPTH;
            }
            else {
                water_step_result spawnedWaterRight = WATER_STEP_RESULT_DID_NOT_SPAWN_WATER;
                water_step_result spawnedWaterLeft = WATER_STEP_RESULT_DID_NOT_SPAWN_WATER;
                if (lastDirection != DIRECTION_RIGHT) {
                    spawnedWaterRight = doWaterStep(map, mapLength, x+1, y, DIRECTION_LEFT, maxY);
                }
                if (lastDirection != DIRECTION_LEFT) {
                    spawnedWaterLeft = doWaterStep(map, mapLength, x-1, y, DIRECTION_RIGHT, maxY);
                }

                if ((spawnedWaterLeft == WATER_STEP_RESULT_DID_NOT_SPAWN_WATER) &&
                    (spawnedWaterRight == WATER_STEP_RESULT_DID_NOT_SPAWN_WATER) && lastDirection == DIRECTION_UP) 
                {
                    map[y * mapLength + x] = '~';
                    settleWater(map, mapLength, x-1, y, DIRECTION_LEFT);
                    settleWater(map, mapLength, x+1, y, DIRECTION_RIGHT);
                }

                water_step_result result = WATER_STEP_RESULT_DID_NOT_SPAWN_WATER;
                if (spawnedWaterLeft == WATER_STEP_RESULT_SPAWNED_WATER || spawnedWaterRight == WATER_STEP_RESULT_SPAWNED_WATER) {
                    result = WATER_STEP_RESULT_SPAWNED_WATER;
                }
                else if (spawnedWaterLeft == WATER_STEP_RESULT_REACHED_MAX_DEPTH || spawnedWaterRight == WATER_STEP_RESULT_REACHED_MAX_DEPTH) {
                    result = WATER_STEP_RESULT_REACHED_MAX_DEPTH;
                }
                else {
                    result = WATER_STEP_RESULT_DID_NOT_SPAWN_WATER;
                }
                return result;
            }

        } break;
        case '#': 
        {
            return WATER_STEP_RESULT_DID_NOT_SPAWN_WATER;
        } break;
        case '~': 
        {
            return WATER_STEP_RESULT_DID_NOT_SPAWN_WATER;
        } break;
    }
}

void printMap (char *map, int mapLength, int maxY) {
    for (int i = 0; i < maxY+10; ++i) {
        for (int j = 0; j < mapLength; ++j) {
            printf("%c", map[i * mapLength + j]);
        }
        printf("\n");
    }
}

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

int main (int argc, char **argv) {
    const int mapLength = 2000;
    char *map = (char *)malloc(mapLength * mapLength * sizeof(char));
    memset(map, '.', mapLength * mapLength * sizeof(char));

    const int stringLength = 40;
    char line[stringLength];
    int lowestY = 2000;
    int highestY = 0;
    while (fgets(line, stringLength, stdin)) {
        char *currentLetter = line;
        char word[stringLength] = {};

        char firstDimension = *currentLetter;
        currentLetter += 2;
        currentLetter = readUntilCharacter(currentLetter, word, ',');
        int firstDimNum = atoi(word);

        currentLetter += 2;
        char secondDimension = *currentLetter;
        currentLetter += 2;
        currentLetter = readUntilCharacter(currentLetter, word, '.');
        int firstSpanNum = atoi(word);
        currentLetter += 2;
        currentLetter = readUntilCharacter(currentLetter, word, '\n');
        int secondSpanNum = atoi(word);

        if (firstDimension == 'x') {
            for (int i = firstSpanNum; i <= secondSpanNum; ++i) {
                map[i * mapLength + firstDimNum] = '#';
            } 
            if (firstSpanNum < lowestY) { lowestY = firstSpanNum; }
            if (secondSpanNum > highestY) { highestY = secondSpanNum; }
        }
        else {
            for (int i = firstSpanNum; i <= secondSpanNum; ++i) {
                map[firstDimNum * mapLength + i] = '#';
            } 
            if (firstDimNum < lowestY) { lowestY = firstDimNum; }
            if (firstDimNum > highestY) { highestY = firstDimNum; }
        }
    }
    map[500] = '+';
    printf("y range: %d-%d\n", lowestY, highestY);

    //printMap(map, mapLength, highestY);

    while (doWaterStep(map, mapLength, 500, 0, DIRECTION_UP, highestY) == WATER_STEP_RESULT_SPAWNED_WATER) {
        //printMap(map, mapLength, highestY);
        //printf("\n");
    }
    printMap(map, mapLength, highestY);

    int totalWater = 0;
    for (int i = lowestY; i < highestY+1; ++i) {
        for (int j = 0; j < mapLength; ++j) {
            char tile = map[i * mapLength + j];
            if (tile == '~') {
                ++totalWater;
            }
        }
    }
    printf("%d\n", totalWater);

    return 0;
}
