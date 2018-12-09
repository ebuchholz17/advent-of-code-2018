#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

int main (int argc, char **argv) {
    const int stringLength = 2000;
    char (*words)[stringLength] = (char (*)[stringLength])malloc(10000 * sizeof(char[stringLength]));
    int numWords = 0;

    char word[stringLength];
    while (fgets(word, stringLength, stdin)) {
        strncpy_s(words[numWords], stringLength, word, stringLength);
        numWords++;
    }

    for (int i = 0; i < numWords; ++i) {

        char *firstWord = words[i];
        for (int j = i+1; j < numWords; ++j) {
            char *secondWord = words[j];

            bool oneDifference = false;
            int oneDifferenceIndex = 0;
            int letterIndex = 0;
            while (firstWord[letterIndex] != 0 && firstWord[letterIndex] != '\n' && letterIndex < stringLength) {
                int diff = secondWord[letterIndex] - firstWord[letterIndex];
                if (diff != 0) {
                    if (oneDifference) {
                        oneDifference = false;
                        break;
                    }
                    else {
                        oneDifference = true;
                        oneDifferenceIndex = letterIndex;
                    }
                }
                ++letterIndex;
            }

            if (oneDifference) {
                char resultString[stringLength];
                letterIndex = 0;
                int numLetters = 0;
                while(firstWord[letterIndex] != 0 && firstWord[letterIndex] != '\n' && letterIndex < stringLength) {
                    if (letterIndex != oneDifferenceIndex) { 
                        resultString[numLetters] = firstWord[letterIndex];
                        numLetters++;
                    }
                    letterIndex++;
                }
                printf(resultString);
                return 0;
            }
        }
    }
    return 0;
}
