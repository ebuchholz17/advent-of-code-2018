#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

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

struct plant_pattern {
    char pattern[6];
    bool producesPlant;
};

void clearPlantState (char *plantState, int plantStateLength) {
    for (long long i = 0; i < plantStateLength; ++i) {
        plantState[i] = '.';
    }
}

int main (int argc, char **argv) {
    const int stringLength = 200;
    char line[stringLength];

    const int maxPlantPatterns = 50;
    plant_pattern plantPatterns[maxPlantPatterns];
    int numPlantPatterns = 0;

    const long long plantStateLength = 300;
    char *plantState = (char *)malloc(plantStateLength * sizeof(char));
    clearPlantState(plantState, plantStateLength);
    char *nextPlantState = (char *)malloc(plantStateLength * sizeof(char));
    clearPlantState(nextPlantState, plantStateLength);
    
    fgets(line, stringLength, stdin);
    char *inputStart = line + 15;
    char *currentLetter = inputStart;
    long long plantStartIndex = 30;
    char *currentWord = plantState + plantStartIndex;
    currentLetter = readUntilCharacter(currentLetter, currentWord, '\n');

    // put a '.' where the null terminator is
    plantState[currentLetter - inputStart + plantStartIndex] = '.'; 

    // skip a line
    fgets(line, stringLength, stdin);

    while (fgets(line, stringLength, stdin)) {
        ASSERT(numPlantPatterns < maxPlantPatterns);
        plant_pattern *plantPattern = &plantPatterns[numPlantPatterns];
        numPlantPatterns++;
        *plantPattern = {};

        currentLetter = line;
        currentWord = plantPattern->pattern;
        currentLetter = readUntilCharacter(currentLetter, currentWord, ' ');
        currentLetter += 4;
        plantPattern->producesPlant = *currentLetter == '#';
    }

    for (int i = 0; i < plantStateLength; ++i) {
        printf("%c", plantState[i]);
    }
    printf("\n");

    long long numIterations = 120;
    char *currentPlantState = plantState;
    char *followingPlantState = nextPlantState;
    for (long long i = 0; i < numIterations; ++i) {
        //printf("%lld\n", i);
        // ignore 2 leftmost and rightmost pots
        for (int plantIndex = 2; plantIndex < plantStateLength - 2; ++plantIndex) {
            char *patternStart = currentPlantState + plantIndex - 2;

            bool foundPattern = false;
            if (plantIndex == 50) {
                int t = 5;
            }
            for (int patternIndex = 0; patternIndex < numPlantPatterns; ++patternIndex) {
                plant_pattern *plantPattern = &plantPatterns[patternIndex];
                char *pattern = plantPattern->pattern;
                bool match = true;
                for (int potIndex = 0; potIndex < 5; ++potIndex) {
                    if (*(patternStart + potIndex) != *(pattern + potIndex)) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    foundPattern = true;
                    if (plantPattern->producesPlant) {
                        *(followingPlantState + plantIndex) = '#';
                    }
                    else {
                        *(followingPlantState + plantIndex) = '.';
                    }
                }
            }

            if (!foundPattern) {
                *(followingPlantState + plantIndex) = '.';
            }
        }

        char *temp = currentPlantState;
        currentPlantState = followingPlantState;
        followingPlantState = temp;

        for (int letterIndex = 0; letterIndex < plantStateLength; ++letterIndex) {
            printf("%c", currentPlantState[letterIndex]);
        }
        printf("\n");

    }
    long long potIndexSum = 0;
    for (long long c = 0; c < plantStateLength; ++c) {
        if (plantState[c] == '#') { 
            potIndexSum += c - plantStartIndex;
        }
    }
    printf("%lld\n", potIndexSum);

    // from 120 onward, the plants shift over forever, so add 42 every time
    long long remainingIterations = 50000000000 - 120;
    long long movementEveryIteration = 42; 
    potIndexSum += remainingIterations * movementEveryIteration;
    printf("%lld\n", potIndexSum);

    //for (int i = 0; i < plantStateLength; ++i) {
    //    printf("%c", plantState[i]);
    //}
    //printf("\n\n");
    //for (int i = 0; i < numPlantPatterns; ++i) {
    //    plant_pattern *plantPattern = &plantPatterns[i];
    //    char plantLetter = plantPattern->producesPlant ? '#' : '.';
    //    printf("%s => %c\n", plantPattern->pattern, plantLetter);
    //}

    return 0;
}
