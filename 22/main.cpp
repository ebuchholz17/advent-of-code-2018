#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

enum cave_equipment {
    CAVE_EQUIPMENT_NONE,
    CAVE_EQUIPMENT_TORCH,
    CAVE_EQUIPMENT_GEAR,
    CAVE_EQUIPMENT_COUNT
};

struct cave_search_node {
    int id;
    int x;
    int y;
    cave_equipment equipment;
    int cost;
    int prevNodeID;
};

struct queue {
    int *entries;
    int firstIndex;
    int lastIndex;
};

cave_search_node *globalNodes;


int distance (int aX, int aY, int bX, int bY) {
    int xDiff = bX - aX;
    if (xDiff < 0) { xDiff = -xDiff; }

    int yDiff = bY - aY;
    if (yDiff < 0) { yDiff = -yDiff; }

    return xDiff + yDiff;
}

int nodeSorter (const void *a, const void *b) {
    int *nodeIndexA = (int *)a;
    int *nodeIndexB = (int *)b;
    cave_search_node *nodeA = &globalNodes[*nodeIndexA];
    cave_search_node *nodeB = &globalNodes[*nodeIndexB];

    if (nodeA->cost < nodeB->cost) { return -1; }
    if (nodeA->cost > nodeB->cost) { return 1; }

    int distToA = distance(nodeA->x, nodeA->y, 10, 715);
    int distToB = distance(nodeB->x, nodeB->y, 10, 715);

    if (distToA < distToB) { return -1; }
    if (distToA > distToB) { return 1; }
    //if (nodeA->pos.y < nodeB->pos.y) { return -1; }
    //if (nodeA->pos.y > nodeB->pos.y) { return 1; }
    //if (nodeA->pos.x < nodeB->pos.x) { return -1; }
    //if (nodeA->pos.x > nodeB->pos.x) { return 1; }
    return 0;
}

void queueInit (queue *queue, int *memory) {
    queue->entries = memory;
    queue->firstIndex = 0;
    queue->lastIndex = 0;
}

void queuePush (int nodeIndex, queue *queue) {
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

cave_search_node *addNode (cave_search_node *searchNodes, int *numSearchNodes, 
                           int x, int y, cave_equipment equipment, int cost) 
{
    cave_search_node *newNode = &searchNodes[*numSearchNodes];
    newNode->id = *numSearchNodes;
    newNode->x = x;
    newNode->y = y;
    newNode->equipment = equipment;
    newNode->cost = cost;

    (*numSearchNodes)++;
    return newNode;
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

void makeNodeSeen (cave_search_node *node, cave_search_node *seenList, int *seenListLength) {
    cave_search_node *newSeenNode = &seenList[*seenListLength];
    (*seenListLength)++;
    newSeenNode->x = node->x;
    newSeenNode->y = node->y;
    newSeenNode->equipment = node->equipment;
}

bool nodeSeen (int x, int y, cave_equipment equipment, cave_search_node *seenList, int seenListLength) {
    for (int i = 0; i < seenListLength; ++i) {
        cave_search_node *seenNode = &seenList[i];
        if (x == seenNode->x &&
            y == seenNode->y &&
            equipment == seenNode->equipment) 
        {
            return true;
        }
    }
    return false;
}

void tryAddNodeAtCoord(int prevID, int x, int y, char *cave, int caveWidth, int caveHeight, 
                       cave_equipment equipment, int cost, 
                       cave_search_node *searchNodes, int *numSearchNodes, 
                       cave_search_node *seenList, int *seenListLength, queue *searchQueue)
{
    // cant go out of bounds
    if (x < 0 || x >= caveWidth) { return; }
    if (y < 0 || y >= caveHeight) { return; }

    // check for incompatible equipment
    char terrainType = cave[y * caveWidth + x];
    if (terrainType == '.' && equipment == CAVE_EQUIPMENT_NONE) { return; }
    if (terrainType == '=' && equipment == CAVE_EQUIPMENT_TORCH) { return; }
    if (terrainType == '|' && equipment == CAVE_EQUIPMENT_GEAR) { return; }

    // check if already seen
    if (nodeSeen(x, y, equipment, seenList, *seenListLength)) { return; }

    // check if already queued
    bool alreadyQueued = false;
    for (int i = searchQueue->firstIndex; i < searchQueue->lastIndex; ++i) {
        cave_search_node *queuedNode = &searchNodes[searchQueue->entries[i]];
        if (queuedNode->x == x && queuedNode->y == y && queuedNode->equipment == equipment) {
            alreadyQueued = true;
            if (cost+1 < queuedNode->cost) { 
                queuedNode->cost = cost+1;
                queuedNode->prevNodeID = prevID;
            }
            break;
        }
    }
    // make a new node and queue it
    if (!alreadyQueued) {
        cave_search_node *newNode = addNode(searchNodes, numSearchNodes, x, y, equipment, cost+1);
        newNode->prevNodeID = prevID;
        queuePush(newNode->id, searchQueue);
    }
}

void tryChangeEquipment(int prevID, int x, int y, char *cave, int caveWidth, int caveHeight, 
                       cave_equipment equipment, int cost, 
                       cave_search_node *searchNodes, int *numSearchNodes, 
                       cave_search_node *seenList, int *seenListLength, queue *searchQueue)
{
    // check for incompatible equipment
    char terrainType = cave[y * caveWidth + x];
    if (terrainType == '.' && equipment == CAVE_EQUIPMENT_NONE) { return; }
    if (terrainType == '=' && equipment == CAVE_EQUIPMENT_TORCH) { return; }
    if (terrainType == '|' && equipment == CAVE_EQUIPMENT_GEAR) { return; }

    if (nodeSeen(x, y, equipment, seenList, *seenListLength)) { return; }

    // check if already queued
    bool alreadyQueued = false;
    for (int i = searchQueue->firstIndex; i < searchQueue->lastIndex; ++i) {
        cave_search_node *queuedNode = &searchNodes[searchQueue->entries[i]];
        if (queuedNode->x == x && queuedNode->y == y && queuedNode->equipment == equipment) {
            alreadyQueued = true;
            if (cost+7 < queuedNode->cost) { 
                queuedNode->cost = cost+7;
                queuedNode->prevNodeID = prevID;
            }
            break;
        }
    }
    // make a new node and queue it
    if (!alreadyQueued) {
        cave_search_node *newNode = addNode(searchNodes, numSearchNodes, x, y, equipment, cost+7);
        newNode->prevNodeID = prevID;
        queuePush(newNode->id, searchQueue);
    }
}

void printPath (cave_search_node *node) {
    if (node->prevNodeID != -1) {
        cave_search_node *prevNode = &globalNodes[node->prevNodeID];
        printPath(prevNode);
    }
    printf("x:%d, y:%d, equipment:%d\n", node->x, node->y, node->equipment);
}

int main (int argc, char **argv) {
    const int stringLength = 40;
    char line[stringLength];

    fgets(line, stringLength, stdin);
    char word[stringLength] = {};
    char *currentLetter = line;

    currentLetter += 7;
    currentLetter = readUntilCharacter(currentLetter, word, '\n');
    int caveDepth = atoi(word);

    fgets(line, stringLength, stdin);
    currentLetter = line;

    currentLetter += 8;
    currentLetter = readUntilCharacter(currentLetter, word, ',');
    int targetX = atoi(word);

    currentLetter++;
    currentLetter = readUntilCharacter(currentLetter, word, '\n');
    int targetY = atoi(word);

    printf("cave depth: %d\n", caveDepth);
    printf("target: %d,%d\n", targetX, targetY);

    int caveWidth = targetX + 60;
    int caveHeight = targetY + 10;

    char *cave = (char *)malloc(caveWidth * caveHeight * sizeof(char));
    int numTiles = 0;
    long long *erosionLevels = (long long *)malloc(caveWidth * caveHeight * sizeof(long long));
    int numErosionLevels = 0;

    for (int i = 0; i < caveHeight; ++i) {
        for (int j = 0; j < caveWidth; ++j) {
            int x = j;
            int y = i;
            long long geologicIndex = 0;
            if (x == 0 && y == 0) {
                geologicIndex = 0;
            }
            else if (x == targetX && y == targetY) {
                geologicIndex = 0;
            }
            else if (y == 0) {
                geologicIndex = x * 16807;
            }
            else if (x == 0) {
                geologicIndex = y * 48271;
            }
            else {
                long long prevXErosionLevel = erosionLevels[y * caveWidth + (x-1)];
                long long prevYErosionLevel = erosionLevels[(y-1) * caveWidth + x];
                geologicIndex = prevXErosionLevel * prevYErosionLevel;
            }
            ASSERT(geologicIndex >= 0);
            long long erosionLevel = (geologicIndex + caveDepth) % 20183;
            erosionLevels[numErosionLevels] = erosionLevel;
            ++numErosionLevels;

            char type = '.';
            int typeNum = erosionLevel % 3;
            switch (typeNum) {
                case 0:
                    type = '.';
                    break;
                case 1:
                    type = '=';
                    break;
                case 2:
                    type = '|';
                    break;
            }
            cave[numTiles] = type;
            ++numTiles;
        }
    }

    for (int i = 0; i < caveHeight; ++i) {
        for (int j = 0; j < caveWidth; ++j) {
            printf("%c", cave[i * caveWidth + j]);
        }
        printf("\n");
    }

    int riskLevel = 0;
    for (int i = 0; i <= targetY; ++i) {
        for (int j = 0; j <= targetX; ++j) {
            char type = cave[i * caveWidth + j];
            switch (type) {
                case '.':
                    riskLevel += 0;
                    break;
                case '=':
                    riskLevel += 1;
                    break;
                case '|':
                    riskLevel += 2;
                    break;
            }
        }
    }
    printf("risk level: %d\n", riskLevel);

    cave_search_node *searchNodes = 
        (cave_search_node *)malloc(caveWidth * caveHeight * CAVE_EQUIPMENT_COUNT * sizeof(cave_search_node));
    int numSearchNodes = 0;
    globalNodes = searchNodes;

    cave_search_node *seenList = 
        (cave_search_node *)malloc(caveWidth * caveHeight * CAVE_EQUIPMENT_COUNT * sizeof(cave_search_node));
    int seenListLength = 0;

    queue searchQueue = {};
    int *queueMemory = (int *)malloc(1000000 * sizeof(int));
    queueInit(&searchQueue, queueMemory);

    cave_search_node *startingNode = addNode(searchNodes, &numSearchNodes, 0, 0, CAVE_EQUIPMENT_TORCH, 0);
    startingNode->prevNodeID = -1;
    queuePush(startingNode->id, &searchQueue);

    int costToTarget = -1;
    cave_search_node *finalNode = 0;
    while (queueItemCount(&searchQueue) > 0) {
        queueSort(&searchQueue);
        int nodeIndex = queuePop(&searchQueue);
        cave_search_node *currentNode = &searchNodes[nodeIndex];
        makeNodeSeen(currentNode, seenList, &seenListLength);
        //printf("current coords: x:%d, y:%d\n", currentNode->x, currentNode->y);

        if (currentNode->x == targetX && currentNode->y == targetY && currentNode->equipment == CAVE_EQUIPMENT_TORCH) { 
            costToTarget = currentNode->cost;
            finalNode = currentNode;
            //break; 
        }

        char tileType = cave[currentNode->y * caveWidth + currentNode->x];
        ASSERT(!(tileType == '.' && currentNode->equipment == CAVE_EQUIPMENT_NONE));
        ASSERT(!(tileType == '=' && currentNode->equipment == CAVE_EQUIPMENT_TORCH));
        ASSERT(!(tileType == '|' && currentNode->equipment == CAVE_EQUIPMENT_GEAR));

        // produce next nodes
        // change equipment
        if (currentNode->equipment != CAVE_EQUIPMENT_NONE) {
            tryChangeEquipment(currentNode->id, currentNode->x, currentNode->y, cave, caveWidth, caveHeight, 
                              CAVE_EQUIPMENT_NONE, currentNode->cost,
                              searchNodes, &numSearchNodes, seenList, &seenListLength, &searchQueue);
        }
        if (currentNode->equipment != CAVE_EQUIPMENT_TORCH) {
            tryChangeEquipment(currentNode->id, currentNode->x, currentNode->y, cave, caveWidth, caveHeight, 
                              CAVE_EQUIPMENT_TORCH, currentNode->cost,
                              searchNodes, &numSearchNodes, seenList, &seenListLength, &searchQueue);
        }
        if (currentNode->equipment != CAVE_EQUIPMENT_GEAR) {
            tryChangeEquipment(currentNode->id, currentNode->x, currentNode->y, cave, caveWidth, caveHeight, 
                              CAVE_EQUIPMENT_GEAR, currentNode->cost,
                              searchNodes, &numSearchNodes, seenList, &seenListLength, &searchQueue);
        }
        // move to adjacent coord
        tryAddNodeAtCoord(currentNode->id, currentNode->x-1, currentNode->y, cave, caveWidth, caveHeight, 
                          currentNode->equipment, currentNode->cost,
                          searchNodes, &numSearchNodes, seenList, &seenListLength, &searchQueue);
        tryAddNodeAtCoord(currentNode->id, currentNode->x+1, currentNode->y, cave, caveWidth, caveHeight, 
                          currentNode->equipment, currentNode->cost,
                          searchNodes, &numSearchNodes, seenList, &seenListLength, &searchQueue);
        tryAddNodeAtCoord(currentNode->id, currentNode->x, currentNode->y-1, cave, caveWidth, caveHeight, 
                          currentNode->equipment, currentNode->cost,
                          searchNodes, &numSearchNodes, seenList, &seenListLength, &searchQueue);
        tryAddNodeAtCoord(currentNode->id, currentNode->x, currentNode->y+1, cave, caveWidth, caveHeight, 
                          currentNode->equipment, currentNode->cost,
                          searchNodes, &numSearchNodes, seenList, &seenListLength, &searchQueue);

    }

    printPath(finalNode);
    printf("total minutes: %d\n", costToTarget);
    return 0;
}
