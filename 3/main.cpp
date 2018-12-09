#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

struct rectangle {
    int id;
    int left;
    int top;
    int width;
    int height;
};

void cutRectangle(rectangle rect, int *fabric) {
    for (int i = rect.top; i < rect.top + rect.height; ++i) {
        for (int j = rect.left; j < rect.left + rect.width; ++j) {
            fabric[i * 1000 + j]++;
        }
    }
}

int main (int argc, char **argv) {
    int *fabric = (int *)malloc(1000 * 1000 * sizeof(int));
    memset(fabric, 0, 1000 * 1000 * sizeof(int));

    rectangle *rectangles = (rectangle *)malloc(3000 * sizeof(rectangle));
    int numRectangles = 0;

    const int stringLength = 2000;
    char word[stringLength];
    while (fgets(word, stringLength, stdin)) {
        // parse rectangle
        char currentWord[10];
        char letterIndex = 0;
        char *currentLetter = word;
        currentLetter++; // skip #
        while (*currentLetter != ' ' && currentLetter != 0) {
            currentWord[letterIndex] = *currentLetter;
            letterIndex++;
            currentLetter++;
        }
        currentWord[letterIndex] = 0;
        int id = atoi(currentWord);

        // skip @
        currentLetter++;
        currentLetter++;
        letterIndex = 0;
        while (*currentLetter != ',' && currentLetter != 0) {
            currentWord[letterIndex] = *currentLetter;
            letterIndex++;
            currentLetter++;
        }
        currentWord[letterIndex] = 0;
        int left = atoi(currentWord);

        currentLetter++;
        letterIndex = 0;
        while (*currentLetter != ':' && currentLetter != 0) {
            currentWord[letterIndex] = *currentLetter;
            letterIndex++;
            currentLetter++;
        }
        currentWord[letterIndex] = 0;
        int top = atoi(currentWord);

        currentLetter++;
        currentLetter++;
        letterIndex = 0;
        while (*currentLetter != 'x' && currentLetter != 0) {
            currentWord[letterIndex] = *currentLetter;
            letterIndex++;
            currentLetter++;
        }
        currentWord[letterIndex] = 0;
        int width = atoi(currentWord);

        currentLetter++;
        letterIndex = 0;
        while (*currentLetter != '\n' && currentLetter != 0) {
            currentWord[letterIndex] = *currentLetter;
            letterIndex++;
            currentLetter++;
        }
        currentWord[letterIndex] = 0;
        int height = atoi(currentWord);

        rectangle *rect = &rectangles[numRectangles];
        rect->id = id;
        rect->left = left;
        rect->top = top;
        rect->width = width;
        rect->height = height;
        numRectangles++;
    }

    for (int i = 0; i < numRectangles; ++i) {
        cutRectangle(rectangles[i], fabric);
    }

    int numOverlappedInches = 0;
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 1000; ++j) {
            if (fabric[i * 1000 + j] > 1) {
                numOverlappedInches++;
            }
        }
    }
    //printf("%d", numOverlappedInches);

    for (int rectIndex = 0; rectIndex < numRectangles; ++rectIndex) {
        rectangle rect = rectangles[rectIndex];

        bool allOnes = true;
        for (int i = rect.top; i < rect.top + rect.height; ++i) {
            for (int j = rect.left; j < rect.left + rect.width; ++j) {
                if (fabric[i * 1000 + j] != 1) { 
                    allOnes = false;
                    goto loopDone; 
                }
            }
        }
loopDone:

        if (allOnes) {
            printf("%d", rect.id);
        }
    }
    return 0;
}
