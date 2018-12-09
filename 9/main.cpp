#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

struct marble_node {
    long long value;
    marble_node *next;
    marble_node *prev;
};

void addMarbleNode (marble_node *currentMarble, marble_node *newMarble) {
    marble_node *nextMarble = currentMarble->next;
    currentMarble->next = newMarble;
    nextMarble->prev = newMarble;

    newMarble->prev = currentMarble;
    newMarble->next = nextMarble;
}

void removeMarbleNode (marble_node *marble) {
    marble_node *prevMarble = marble->prev;
    marble_node *nextMarble = marble->next;

    prevMarble->next = nextMarble;
    nextMarble->prev = prevMarble;
}

void *allocateSize (int size, void *memory, int *used, int capacity) {
    ASSERT(*used + size < capacity);

    void *result = (char *)memory + *used;
    *used += size;

    return result;
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
    int memoryUsed = 0;
    int memoryCapacity = 1024 * 1024 * 1000;
    void *marbleMemory = malloc(memoryCapacity);

    const int stringLength = 100;
    char wholeInput[stringLength];
    fgets(wholeInput, stringLength, stdin);

    char *currentLetter = wholeInput;
    char word[stringLength];

    int numPlayers, numMarbles;
    currentLetter = readUntilCharacter(currentLetter, word, ' ');
    numPlayers = atoi(word);
    currentLetter++;

    // find the h in "worth" 
    currentLetter = readUntilCharacter(currentLetter, word, 'h');
    currentLetter += 2;

    currentLetter = readUntilCharacter(currentLetter, word, ' ');
    numMarbles = (atoi(word) * 100) + 1;

    long long *playerScores = (long long *)malloc(numPlayers * sizeof(long long));
    memset(playerScores, 0, sizeof(long long) * 10);
    int currentPlayerIndex = 0;

    marble_node *currentMarble = 
        (marble_node *)allocateSize(sizeof(marble_node), marbleMemory, &memoryUsed, memoryCapacity);
    *currentMarble = {};
    currentMarble->value = 0;
    currentMarble->next = currentMarble;
    currentMarble->prev = currentMarble;

    for (long long marbleValue = 1; marbleValue < numMarbles; ++marbleValue) {
        if (marbleValue % 23 == 0) {
            playerScores[currentPlayerIndex] += marbleValue;
            for (int i = 0; i < 7; ++i) {
                currentMarble = currentMarble->prev;
            }
            playerScores[currentPlayerIndex] += currentMarble->value;
            marble_node *newCurrentMarble = currentMarble->next;
            removeMarbleNode(currentMarble);
            currentMarble = newCurrentMarble;
        }
        else {
            marble_node *newMarble = 
                (marble_node *)allocateSize(sizeof(marble_node), marbleMemory, &memoryUsed, memoryCapacity);
            *newMarble = {};
            newMarble->value = marbleValue;
            addMarbleNode(currentMarble->next, newMarble);
            currentMarble = newMarble;
        }
        currentPlayerIndex = (++currentPlayerIndex) % numPlayers;
    }

    long long bestScore = 0;
    for (int i = 0; i < numPlayers; ++i) {
        if (playerScores[i] > bestScore) {
            bestScore = playerScores[i];
        }
    }
    printf("%lld", bestScore);

    return 0;
}
