#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

int main (int argc, char **argv) {
    const int stringLength = 10;
    char line[stringLength];
    fgets(line, stringLength, stdin);
    int numRecipesRequired = atoi(line);
    int totalRecipesRequired = numRecipesRequired + 10;

    int *recipes = (int *)malloc(1000000 * sizeof(int));
    int numRecipes = 2;
    recipes[0] = 3;
    recipes[1] = 7;
    int firstRecipeIndex = 0;
    int secondRecipeIndex = 1;

    while (numRecipes < totalRecipesRequired) {
        //for (int i = 0; i < numRecipes; ++i) {
        //    printf("%d", recipes[i]);
        //}
        //printf("\n");
        int firstRecipeNum = recipes[firstRecipeIndex];
        int secondRecipeNum = recipes[secondRecipeIndex];
        int recipeSum = firstRecipeNum + secondRecipeNum;

        if (recipeSum >= 10) {
            int tensDigit = recipeSum / 10;
            ASSERT(numRecipes < 1000000);
            recipes[numRecipes] = tensDigit;
            ++numRecipes;
        }
        int onesDigit = recipeSum % 10;
        ASSERT(numRecipes < 1000000);
        recipes[numRecipes] = onesDigit;
        ++numRecipes;
        
        firstRecipeIndex = (firstRecipeIndex + 1 + firstRecipeNum) % numRecipes;
        secondRecipeIndex = (secondRecipeIndex + 1 + secondRecipeNum) % numRecipes;
    }

    for (int i = numRecipesRequired; i < numRecipesRequired + 10; ++i) {
        printf("%d", recipes[i]);
    }
    printf("\n");

    return 0;
}
