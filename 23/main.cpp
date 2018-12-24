#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

struct octahedron {
    int x;
    int y;
    int z;
    int radius;
};

struct cube {
    int minX;
    int minY;
    int minZ;
    int maxX;
    int maxY;
    int maxZ;
};

struct grid_cell {
    cube bounds;
    int id;
    int numNanobotsInRange;
};

struct queue {
    int *entries;
    int firstIndex;
    int lastIndex;
};

grid_cell *globalGridCells;

int distance (int aX, int aY, int aZ, int bX, int bY, int bZ) {
    int xDiff = bX - aX;
    if (xDiff < 0) { xDiff = -xDiff; }

    int yDiff = bY - aY;
    if (yDiff < 0) { yDiff = -yDiff; }

    int zDiff = bZ - aZ;
    if (zDiff < 0) { zDiff = -zDiff; }

    return xDiff + yDiff + zDiff;
}

// sort by num nanobots in range, then distance to origin
int nodeSorter (const void *a, const void *b) {
    int *nodeIndexA = (int *)a;
    int *nodeIndexB = (int *)b;
    grid_cell *nodeA = &globalGridCells[*nodeIndexA];
    grid_cell *nodeB = &globalGridCells[*nodeIndexB];

    if (nodeA->numNanobotsInRange > nodeB->numNanobotsInRange) { return -1; }
    if (nodeA->numNanobotsInRange < nodeB->numNanobotsInRange) { return 1; }

    int aMinX = nodeA->bounds.minX < 0 ? -nodeA->bounds.minX : nodeA->bounds.minX;
    int aMaxX = nodeA->bounds.maxX < 0 ? -nodeA->bounds.maxX : nodeA->bounds.maxX;
    int aX = aMinX < aMaxX ? aMinX : aMaxX;

    int aMinY = nodeA->bounds.minY < 0 ? -nodeA->bounds.minY : nodeA->bounds.minY;
    int aMaxY = nodeA->bounds.maxY < 0 ? -nodeA->bounds.maxY : nodeA->bounds.maxY;
    int aY = aMinY < aMaxY ? aMinY : aMaxY;

    int aMinZ = nodeA->bounds.minZ < 0 ? -nodeA->bounds.minZ : nodeA->bounds.minZ;
    int aMaxZ = nodeA->bounds.maxZ < 0 ? -nodeA->bounds.maxZ : nodeA->bounds.maxZ;
    int aZ = aMinZ < aMaxZ ? aMinZ : aMaxZ;

    int bMinX = nodeB->bounds.minX < 0 ? -nodeB->bounds.minX : nodeB->bounds.minX;
    int bMaxX = nodeB->bounds.maxX < 0 ? -nodeB->bounds.maxX : nodeB->bounds.maxX;
    int bX = bMinX < bMaxX ? bMinX : bMaxX;

    int bMinY = nodeB->bounds.minY < 0 ? -nodeB->bounds.minY : nodeB->bounds.minY;
    int bMaxY = nodeB->bounds.maxY < 0 ? -nodeB->bounds.maxY : nodeB->bounds.maxY;
    int bY = bMinY < bMaxY ? bMinY : bMaxY;

    int bMinZ = nodeB->bounds.minZ < 0 ? -nodeB->bounds.minZ : nodeB->bounds.minZ;
    int bMaxZ = nodeB->bounds.maxZ < 0 ? -nodeB->bounds.maxZ : nodeB->bounds.maxZ;
    int bZ = bMinZ < bMaxZ ? bMinZ : bMaxZ;

    int aDistToOrigin = distance(aX, aY, aZ, 0, 0, 0);
    int bDistToOrigin = distance(bX, bY, bZ, 0, 0, 0);

    if (aDistToOrigin < bDistToOrigin) { return -1; }
    if (aDistToOrigin > bDistToOrigin) { return 1; }
    return 0;
}

void queueInit (queue *queue, int *memory) {
    queue->entries = memory;
    queue->firstIndex = 0;
    queue->lastIndex = 0;
}

void queuePush (int nodeIndex, queue *queue) {
    ASSERT(queue->lastIndex < 3000000);
    queue->entries[queue->lastIndex] = nodeIndex;
    queue->lastIndex++;
}

int queuePop (queue *queue) {
    int result = queue->entries[queue->firstIndex];
    queue->firstIndex++;
    return result;
}

bool queueItemExists (int index, queue *queue) {
    for (int i = queue->firstIndex; i < queue->lastIndex; ++i) {
        if (queue->entries[i] == index) { return true; }
    }
    return false;
}

int queueItemCount (queue *queue) {
    return queue->lastIndex - queue->firstIndex;
}

void queueSort (queue *queue) {
    int numQueueItems = queueItemCount(queue);
    int *start = &queue->entries[queue->firstIndex];
    qsort(start, numQueueItems, sizeof(int), nodeSorter);
}

grid_cell *addGridCell (grid_cell *gridCells, int *numGridCells, const int maxGridCells) {
    ASSERT(*numGridCells < maxGridCells);
    grid_cell *newGridCell = &gridCells[*numGridCells];
    newGridCell->bounds.minX = -536870912;
    newGridCell->bounds.minY = -536870912;
    newGridCell->bounds.minZ = -536870912;
    newGridCell->bounds.maxX = 536870912;
    newGridCell->bounds.maxY = 536870912;
    newGridCell->bounds.maxZ = 536870912;
    newGridCell->numNanobotsInRange = 0;
    newGridCell->id = *numGridCells;
    ++(*numGridCells);

    return newGridCell;
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

void countNanobotIntersections (grid_cell *gridCell, octahedron *octahedrons, int numOctahedrons) {
    cube bounds = gridCell->bounds;
    for (int i = 0; i < numOctahedrons; ++i) {
        octahedron *currentOctahedron = &octahedrons[i];
        int cubeDistToOcatahedron = 0;
        if ((bounds.maxX-1) < currentOctahedron->x) { cubeDistToOcatahedron += currentOctahedron->x - (bounds.maxX-1); }
        if (bounds.minX > currentOctahedron->x) { cubeDistToOcatahedron += bounds.minX - currentOctahedron->x; }
        if ((bounds.maxY-1) < currentOctahedron->y) { cubeDistToOcatahedron += currentOctahedron->y - (bounds.maxY-1); }
        if (bounds.minY > currentOctahedron->y) { cubeDistToOcatahedron += bounds.minY - currentOctahedron->y; }
        if ((bounds.maxZ-1) < currentOctahedron->z) { cubeDistToOcatahedron += currentOctahedron->z - (bounds.maxZ-1); }
        if (bounds.minZ > currentOctahedron->z) { cubeDistToOcatahedron += bounds.minZ - currentOctahedron->z; }

        if (cubeDistToOcatahedron <= currentOctahedron->radius) {
            gridCell->numNanobotsInRange++;
        }
    }
}

int main (int argc, char **argv) {
    octahedron *octahedrons = (octahedron *)malloc(1200 * sizeof(octahedron));
    int numOctahedrons = 0;

    const int stringLength = 60;
    char line[stringLength];

    char word[stringLength] = {};
    while (fgets(line, stringLength, stdin)) {
        octahedron *nanobot = &octahedrons[numOctahedrons];
        ++numOctahedrons;

        char *currentLetter = line;
        currentLetter += 5;
        currentLetter = readUntilCharacter(currentLetter, word, ',');
        nanobot->x = atoi(word);

        ++currentLetter;
        currentLetter = readUntilCharacter(currentLetter, word, ',');
        nanobot->y = atoi(word);

        ++currentLetter;
        currentLetter = readUntilCharacter(currentLetter, word, '>');
        nanobot->z = atoi(word);

        currentLetter += 5;
        currentLetter = readUntilCharacter(currentLetter, word, '\n');
        nanobot->radius = atoi(word);
    }

    //for (int i = 0; i < numOctahedrons; ++i) {
    //    octahedron *nanobot = &octahedrons[i];
    //    printf("x:%d, y:%d, z:%d, radius:%d\n", nanobot->x, nanobot->y, nanobot->z, nanobot->radius);
    //}


    int biggestRadius = 0;
    int indexOfNanobotWithBiggestRadius = -1;
    for (int i = 0; i < numOctahedrons; ++i) {
        octahedron *nanobot = &octahedrons[i];
        if (nanobot->radius > biggestRadius) {
            biggestRadius = nanobot->radius;
            indexOfNanobotWithBiggestRadius = i;
        }
    }

    int numNanobotsInRange = 0;
    octahedron *biggestRadiusNanobot = &octahedrons[indexOfNanobotWithBiggestRadius];
    for (int i = 0; i < numOctahedrons; ++i) {
        //if (i == indexOfNanobotWithBiggestRadius) { continue; }
        octahedron *nanobot = &octahedrons[i];
        int xDiff = nanobot->x - biggestRadiusNanobot->x;
        if (xDiff < 0) { xDiff = -xDiff; }
        int yDiff = nanobot->y - biggestRadiusNanobot->y;
        if (yDiff < 0) { yDiff = -yDiff; }
        int zDiff = nanobot->z - biggestRadiusNanobot->z;
        if (zDiff < 0) { zDiff = -zDiff; }
        if (xDiff + yDiff + zDiff <= biggestRadius) {
            numNanobotsInRange++;
        }
    }

    printf("num nanobots in range:%d\n", numNanobotsInRange);

    const int maxGridCells = 1000000;
    grid_cell *gridCells = (grid_cell *)malloc(maxGridCells * sizeof(grid_cell));
    int numGridCells = 0;
    globalGridCells = gridCells;

    grid_cell *firstGridCell = addGridCell(gridCells, &numGridCells, maxGridCells);

    queue cellQueue = {};
    int *queueMemory = (int *)malloc(3000000 * sizeof(int));
    queueInit(&cellQueue, queueMemory);

    queuePush(firstGridCell->id, &cellQueue);

    int shortestManhattanDistance = 0;
    while (queueItemCount(&cellQueue) > 0) {
        queueSort(&cellQueue);
        int gridCellIndex = queuePop(&cellQueue);
        grid_cell *currentCell = &gridCells[gridCellIndex];

        // if we are down to a 1x1 cell we should be done
        if (currentCell->bounds.maxX - currentCell->bounds.minX == 1) { 
            int aMinX = currentCell->bounds.minX < 0 ? -currentCell->bounds.minX : currentCell->bounds.minX;
            int aMaxX = currentCell->bounds.maxX < 0 ? -currentCell->bounds.maxX : currentCell->bounds.maxX;
            int aX = aMinX < aMaxX ? aMinX : aMaxX;

            int aMinY = currentCell->bounds.minY < 0 ? -currentCell->bounds.minY : currentCell->bounds.minY;
            int aMaxY = currentCell->bounds.maxY < 0 ? -currentCell->bounds.maxY : currentCell->bounds.maxY;
            int aY = aMinY < aMaxY ? aMinY : aMaxY;

            int aMinZ = currentCell->bounds.minZ < 0 ? -currentCell->bounds.minZ : currentCell->bounds.minZ;
            int aMaxZ = currentCell->bounds.maxZ < 0 ? -currentCell->bounds.maxZ : currentCell->bounds.maxZ;
            int aZ = aMinZ < aMaxZ ? aMinZ : aMaxZ;

            shortestManhattanDistance = distance(aX, aY, aZ, 0, 0, 0);
            break; 
        }
        
        // subdivide it into 8 and count the nanobot intersections for each one
        cube bounds = currentCell->bounds;
        int midX = (bounds.minX + bounds.maxX) / 2;
        int midY = (bounds.minY + bounds.maxY) / 2;
        int midZ = (bounds.minZ + bounds.maxZ) / 2;

        grid_cell *bottomFrontLeftCell = addGridCell(gridCells, &numGridCells, maxGridCells);
        bottomFrontLeftCell->bounds.minX = bounds.minX;
        bottomFrontLeftCell->bounds.minY = bounds.minY;
        bottomFrontLeftCell->bounds.minZ = bounds.minZ;
        bottomFrontLeftCell->bounds.maxX = midX;
        bottomFrontLeftCell->bounds.maxY = midY;
        bottomFrontLeftCell->bounds.maxZ = midZ;

        grid_cell *bottomFrontRightCell = addGridCell(gridCells, &numGridCells, maxGridCells);
        bottomFrontRightCell->bounds.minX = midX;
        bottomFrontRightCell->bounds.minY = bounds.minY;
        bottomFrontRightCell->bounds.minZ = bounds.minZ;
        bottomFrontRightCell->bounds.maxX = bounds.maxX;
        bottomFrontRightCell->bounds.maxY = midY;
        bottomFrontRightCell->bounds.maxZ = midZ;

        grid_cell *topFrontLeftCell = addGridCell(gridCells, &numGridCells, maxGridCells);
        topFrontLeftCell->bounds.minX = bounds.minX;
        topFrontLeftCell->bounds.minY = midY;
        topFrontLeftCell->bounds.minZ = bounds.minZ;
        topFrontLeftCell->bounds.maxX = midX;
        topFrontLeftCell->bounds.maxY = bounds.maxY;
        topFrontLeftCell->bounds.maxZ = midZ;

        grid_cell *topFrontRightCell = addGridCell(gridCells, &numGridCells, maxGridCells);
        topFrontRightCell->bounds.minX = midX;
        topFrontRightCell->bounds.minY = midY;
        topFrontRightCell->bounds.minZ = bounds.minZ;
        topFrontRightCell->bounds.maxX = bounds.maxX;
        topFrontRightCell->bounds.maxY = bounds.maxY;
        topFrontRightCell->bounds.maxZ = midZ;

        grid_cell *bottomBackLeftCell = addGridCell(gridCells, &numGridCells, maxGridCells);
        bottomBackLeftCell->bounds.minX = bounds.minX;
        bottomBackLeftCell->bounds.minY = bounds.minY;
        bottomBackLeftCell->bounds.minZ = midZ;
        bottomBackLeftCell->bounds.maxX = midX;
        bottomBackLeftCell->bounds.maxY = midY;
        bottomBackLeftCell->bounds.maxZ = bounds.maxZ;

        grid_cell *bottomBackRightCell = addGridCell(gridCells, &numGridCells, maxGridCells);
        bottomBackRightCell->bounds.minX = midX;
        bottomBackRightCell->bounds.minY = bounds.minY;
        bottomBackRightCell->bounds.minZ = midZ;
        bottomBackRightCell->bounds.maxX = bounds.maxX;
        bottomBackRightCell->bounds.maxY = midY;
        bottomBackRightCell->bounds.maxZ = bounds.maxZ;

        grid_cell *topBackLeftCell = addGridCell(gridCells, &numGridCells, maxGridCells);
        topBackLeftCell->bounds.minX = bounds.minX;
        topBackLeftCell->bounds.minY = midY;
        topBackLeftCell->bounds.minZ = midZ;
        topBackLeftCell->bounds.maxX = midX;
        topBackLeftCell->bounds.maxY = bounds.maxY;
        topBackLeftCell->bounds.maxZ = bounds.maxZ;

        grid_cell *topBackRightCell = addGridCell(gridCells, &numGridCells, maxGridCells);
        topBackRightCell->bounds.minX = midX;
        topBackRightCell->bounds.minY = midY;
        topBackRightCell->bounds.minZ = midZ;
        topBackRightCell->bounds.maxX = bounds.maxX;
        topBackRightCell->bounds.maxY = bounds.maxY;
        topBackRightCell->bounds.maxZ = bounds.maxZ;

        countNanobotIntersections(bottomFrontLeftCell, octahedrons, numOctahedrons);
        countNanobotIntersections(bottomFrontRightCell, octahedrons, numOctahedrons);
        countNanobotIntersections(topFrontLeftCell, octahedrons, numOctahedrons);
        countNanobotIntersections(topFrontRightCell, octahedrons, numOctahedrons);
        countNanobotIntersections(bottomBackLeftCell, octahedrons, numOctahedrons);
        countNanobotIntersections(bottomBackRightCell, octahedrons, numOctahedrons);
        countNanobotIntersections(topBackLeftCell, octahedrons, numOctahedrons);
        countNanobotIntersections(topBackRightCell, octahedrons, numOctahedrons);

        queuePush(bottomFrontLeftCell->id, &cellQueue);
        queuePush(bottomFrontRightCell->id, &cellQueue);
        queuePush(topFrontLeftCell->id, &cellQueue);
        queuePush(topFrontRightCell->id, &cellQueue);
        queuePush(bottomBackLeftCell->id, &cellQueue);
        queuePush(bottomBackRightCell->id, &cellQueue);
        queuePush(topBackLeftCell->id, &cellQueue);
        queuePush(topBackRightCell->id, &cellQueue);
    }
    printf("shortest distance: %d\n", shortestManhattanDistance);

    return 0;
}
