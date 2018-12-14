#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

int main (int argc, char **argv) {
    const int stringLength = 5;
    char wholeInput[stringLength];

    fgets(wholeInput, stringLength, stdin);
    int serialNumber = atoi(wholeInput);

    // 300 possible grid sizes including the cells themselves
    const int gridDim = 300;
    int **squareGrids = (int **)malloc(gridDim * sizeof(int *));
    for (int i = 0; i < gridDim; ++i) {
        int squareGridDim = gridDim - i;
        int squareGridSize = squareGridDim * squareGridDim;

        int *squareGrid = (int *)malloc(squareGridSize * sizeof(int));
        memset(squareGrid, 0, squareGridSize * sizeof(int));

        squareGrids[i] = squareGrid;
    }

    // calculate the cell power levels
    int *baseGrid = squareGrids[0];
    for (int y = 0; y < gridDim; ++y) {
        for (int x = 0; x < gridDim; ++x) {
            int xCoord = x + 1;
            int yCoord = y + 1;
            int rackID = xCoord + 10;
            int powerLevel = rackID * yCoord;
            powerLevel += serialNumber;
            powerLevel *= rackID;
            powerLevel /= 100;
            powerLevel = powerLevel % 10;
            powerLevel -= 5;

            baseGrid[y * gridDim + x] = powerLevel;
        }
    }

    // brute force was fast enough
    for (int i = 1; i < gridDim; ++i) {
        int squareGridDim = gridDim - i;
        int *squareGrid = squareGrids[i];
        int *prevSquareGrid = squareGrids[i-1];
        for (int y = 0; y < squareGridDim; ++y) {
            for (int x = 0; x < squareGridDim; ++x) {
                //int squarePowerTotal = 0;
                //for (int gridY = y; gridY <= y + i; ++gridY) {
                //    for (int gridX = x; gridX <= x + i; ++gridX) {
                //        squarePowerTotal += baseGrid[gridY * gridDim + gridX];
                //    }
                //}
                //squareGrid[y * squareGridDim + x] = squarePowerTotal;

                int squarePowerTotal = prevSquareGrid[y * (squareGridDim+1) + x];
                
                for (int gridY = y; gridY <= y + i; ++gridY) {
                    squarePowerTotal += baseGrid[gridY * gridDim + x+i];
                }
                for (int gridX = x; gridX <= x + (i-1); ++gridX) {
                    squarePowerTotal += baseGrid[(y+i) * gridDim + gridX];
                }
                squareGrid[y * squareGridDim + x] = squarePowerTotal;
            }
        }
    }

    int bestX = 0;
    int bestY = 0;
    int bestPower = 0;
    int bestGridSize = 0;
    for (int i = 0; i < gridDim; ++i) {
        int squareGridDim = gridDim - i;
        int *squareGrid = squareGrids[i];
        for (int y = 0; y < squareGridDim; ++y) {
            for (int x = 0; x < squareGridDim; ++x) {
                int squarePowerTotal = squareGrid[y * squareGridDim + x];
                if (squarePowerTotal > bestPower) {
                    bestPower = squarePowerTotal;
                    bestX = x;
                    bestY = y;
                    bestGridSize = i;
                }
                
            }
        }
    }
    printf("%d, %d, %d, %d", bestX + 1, bestY + 1, bestGridSize + 1, bestPower);

    return 0;
}
