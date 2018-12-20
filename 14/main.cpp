#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

int main (int argc, char **argv) {
    const int maxRecipes = 100000000;
    int *recipes = (int *)malloc(maxRecipes * sizeof(int));
    int numRecipes = 2;
    recipes[0] = 3;
    recipes[1] = 7;
    int firstRecipeIndex = 0;
    int secondRecipeIndex = 1;

    const int stringLength = 10;
    char line[stringLength];
    fgets(line, stringLength, stdin);

    int sequenceLength = 0;
    char *currentCharacter = line;
    while (*currentCharacter++ != 0) { ++sequenceLength; }

    int currentMatchingDigit = 0;
    int numRecipesBeforeMatchingSequence = 2;
    while (true) {
        //for (int i = 0; i < numRecipes; ++i) {
        //    printf("%d", recipes[i]);
        //}
        //printf("\n");
        int firstRecipeNum = recipes[firstRecipeIndex];
        int secondRecipeNum = recipes[secondRecipeIndex];
        int recipeSum = firstRecipeNum + secondRecipeNum;

        if (recipeSum >= 10) {
            int tensDigit = recipeSum / 10;
            ASSERT(numRecipes < maxRecipes);
            recipes[numRecipes] = tensDigit;
            ++numRecipes;
            if (recipes[numRecipes - 1] == line[currentMatchingDigit] - '0') {
                ++currentMatchingDigit;
                if (currentMatchingDigit == sequenceLength - 1) { 
                    break; 
                }
            }
            else {
                currentMatchingDigit = 0;
                numRecipesBeforeMatchingSequence = numRecipes;
            }
        }
        int onesDigit = recipeSum % 10;
        ASSERT(numRecipes < maxRecipes);
        recipes[numRecipes] = onesDigit;
        ++numRecipes;
        if (recipes[numRecipes - 1] == line[currentMatchingDigit] - '0') {
            ++currentMatchingDigit;
            if (currentMatchingDigit == sequenceLength - 1) { 
                break; 
            }
        }
        else {
            currentMatchingDigit = 0;
            numRecipesBeforeMatchingSequence = numRecipes;
        }
        
        firstRecipeIndex = (firstRecipeIndex + 1 + firstRecipeNum) % numRecipes;
        secondRecipeIndex = (secondRecipeIndex + 1 + secondRecipeNum) % numRecipes;
    }

    printf("%d", numRecipesBeforeMatchingSequence);

    return 0;
}
