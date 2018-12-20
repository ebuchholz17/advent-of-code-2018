#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

enum unit_type {
    UNIT_TYPE_ELF,
    UNIT_TYPE_GOBLIN
};

struct position {
    int x;
    int y;
};

int positionDistance (position a, position b) {
    int xDiff = b.x - a.x;
    if (xDiff < 0) { xDiff = -xDiff; }

    int yDiff = b.y - a.y;
    if (yDiff < 0) { yDiff = -yDiff; }

    return xDiff + yDiff;
}

struct map_unit {
    int id;
    unit_type type;
    position pos;
    int hitPoints;
    bool dead;
};

struct path_to_space {
    position spaces[100];
    int numSpaces;
};

struct path_node {
    int id;
    position pos;
    path_node *prev;
};

path_node *pathNodes;
position pathTarget;

struct path_node_queue {
    int *entries;
    int firstIndex;
    int lastIndex;
};

int pathNodeSorter (const void *a, const void *b) {
    int *pathNodeIndexA = (int *)a;
    int *pathNodeIndexB = (int *)b;
    path_node *nodeA = &pathNodes[*pathNodeIndexA];
    path_node *nodeB = &pathNodes[*pathNodeIndexB];

    int distToA = positionDistance(nodeA->pos, pathTarget);
    int distToB = positionDistance(nodeB->pos, pathTarget);

    if (distToA < distToB) { return -1; }
    if (distToA > distToB) { return 1; }
    if (nodeA->pos.y < nodeB->pos.y) { return -1; }
    if (nodeA->pos.y > nodeB->pos.y) { return 1; }
    if (nodeA->pos.x < nodeB->pos.x) { return -1; }
    if (nodeA->pos.x > nodeB->pos.x) { return 1; }
    return 0;
}

void queueInit (path_node_queue *queue, int *memory) {
    queue->entries = memory;
    queue->firstIndex = 0;
    queue->lastIndex = 0;
}

void queuePush (int nodeIndex, path_node_queue *queue) {
    queue->entries[queue->lastIndex] = nodeIndex;
    queue->lastIndex++;
}

int queuePop (path_node_queue *queue) {
    int result = queue->entries[queue->firstIndex];
    queue->firstIndex++;
    return result;
}

bool queueItemExists (int index, path_node_queue *queue) {
    for (int i = queue->firstIndex; i < queue->lastIndex; ++i) {
        if (queue->entries[i] == index) { return true; }
    }
    return false;
}

int queueItemCount (path_node_queue *queue) {
    return queue->lastIndex - queue->firstIndex;
}

void queueSort (path_node_queue *queue) {
    int numQueueItems = queueItemCount(queue);
    int *start = &queue->entries[queue->firstIndex];
    qsort(start, numQueueItems, sizeof(int), pathNodeSorter);
}

map_unit *units;

void printMap (char *map, int mapWidth, int mapHeight, int numUnits) {
    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            bool foundUnit = false;
            for (int unitIndex = 0; unitIndex < numUnits; ++unitIndex) {
                map_unit *unit = &units[unitIndex];
                if (unit->dead) { continue; }
                if (unit->pos.x == j && unit->pos.y == i) {
                    foundUnit = true;
                    switch (unit->type) {
                    case UNIT_TYPE_GOBLIN: 
                        printf("G");
                        break;
                    case UNIT_TYPE_ELF:
                        printf("E");
                        break;
                    }
                    break;
                }
            }
            if (!foundUnit) {
                printf("%c", map[i * mapWidth + j]);
            }
        }
        printf("\n");
    }
}

int unitSorter (const void *a, const void *b) {
    int *unitAID = (int *)a;
    int *unitBID = (int *)b;
    map_unit *unitA = &units[*unitAID];
    map_unit *unitB = &units[*unitBID];

    if (unitA->pos.y < unitB->pos.y) { return -1; }
    if (unitA->pos.y > unitB->pos.y) { return 1; }
    if (unitA->pos.x < unitB->pos.x) { return -1; }
    if (unitA->pos.x > unitB->pos.x) { return 1; }

    return 0;
}

int adjacentTargetSorter (const void *a, const void *b) {
    int *unitAID = (int *)a;
    int *unitBID = (int *)b;
    map_unit *unitA = &units[*unitAID];
    map_unit *unitB = &units[*unitBID];

    if (unitA->hitPoints < unitB->hitPoints) { return -1; }
    if (unitA->hitPoints > unitB->hitPoints) { return 1; }
    if (unitA->pos.y < unitB->pos.y) { return -1; }
    if (unitA->pos.y > unitB->pos.y) { return 1; }
    if (unitA->pos.x < unitB->pos.x) { return -1; }
    if (unitA->pos.x > unitB->pos.x) { return 1; }

    return 0;
}

bool unitsAreAdjacent (int unitAID, int unitBID) {
    map_unit *unitA = &units[unitAID];
    map_unit *unitB = &units[unitBID];

    return positionDistance(unitA->pos, unitB->pos) == 1;
}

bool unitInPosition (int *unitIDs, int numUnits, position pos) {
    for (int i = 0; i < numUnits; ++i) {
        int unitID = unitIDs[i];
        map_unit *unit = &units[unitID];
        if (unit->dead) { continue; }
        if (unit->pos.x == pos.x && unit->pos.y == pos.y) {
            return true;
        }
    }
    return false;
}

void findAdjacentTargets (int unitID, int *targetIDs, int numTargets, int *adjacentTargetIDs, int *numAdjacentTargets) {
    for (int targetIndex = 0; targetIndex < numTargets; ++targetIndex) {
        int targetID = targetIDs[targetIndex];
        if (unitsAreAdjacent(unitID, targetID)) {
            adjacentTargetIDs[*numAdjacentTargets] = targetID;
            ++(*numAdjacentTargets);
        }
    }
}

void addSquareIfOpen (position pos, int *unitIDs, int numUnits, char *map, int mapWidth, position *openSquares, int *numOpenSquares) {
    if (!unitInPosition(unitIDs, numUnits, pos) && map[pos.y * mapWidth + pos.x] == '.') {
        openSquares[*numOpenSquares] = pos;
        (*numOpenSquares)++;
    }
}

bool posIsOpenAndUnseen (position pos, int *unitIDs, int numUnits, char *map, int mapWidth, position *seenList, int numSeen) {
    if (!unitInPosition(unitIDs, numUnits, pos) && map[pos.y * mapWidth + pos.x] == '.') {
        for (int i = 0; i < numSeen; ++i) {
            position seenPos = seenList[i];
            if (seenPos.x == pos.x && seenPos.y == pos.y) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void addPathNodeToPath (path_to_space *path, path_node *pathNode) {
    if (pathNode->prev) {
        addPathNodeToPath(path, pathNode->prev);
    }
    path->spaces[path->numSpaces] = pathNode->pos;
    path->numSpaces++;
}

void processPathPosition (int *numPathNodes, 
                          position pos, path_node *prev,
                          position *seenList, int *numSeen, 
                          path_node_queue *pathfindingQueue) 
{
    path_node *node = &pathNodes[*numPathNodes];
    *node = {};
    node->id = *numPathNodes;
    node->pos = pos;
    node->prev = prev;
    ++(*numPathNodes);

    seenList[*numSeen] = node->pos;
    ++(*numSeen);

    queuePush(node->id, pathfindingQueue);
}

void findShortestPathToOpenSquare (position startPos, position endPos, 
                                   path_to_space *possiblePaths, int *numPossiblePaths,
                                   int *numPathNodes,
                                   position *seenList, int *numSeen,
                                   path_node_queue *pathfindingQueue,
                                   char *map, int mapWidth, int *unitIDs, int numUnits) 
{
    pathTarget = endPos;

    processPathPosition(numPathNodes, startPos, 0, seenList, numSeen, pathfindingQueue);

    path_node *endNode = 0;
    while (queueItemCount(pathfindingQueue) > 0) {
        //queueSort(pathfindingQueue);
        int pathNodeID = queuePop(pathfindingQueue);

        path_node *pathNode = &pathNodes[pathNodeID];
        position nodePos = pathNode->pos;

        if (pathNode->pos.x == pathTarget.x && pathNode->pos.y == pathTarget.y) {
            endNode = pathNode;
            break;
        }

        position abovePos = nodePos;
        abovePos.y -= 1;
        if (posIsOpenAndUnseen(abovePos, unitIDs, numUnits, map, mapWidth, seenList, *numSeen)) {
            processPathPosition(numPathNodes, abovePos, pathNode, seenList, numSeen, pathfindingQueue);
        }

        position leftPos = nodePos;
        leftPos.x -= 1;
        if (posIsOpenAndUnseen(leftPos, unitIDs, numUnits, map, mapWidth, seenList, *numSeen)) {
            processPathPosition(numPathNodes, leftPos, pathNode, seenList, numSeen, pathfindingQueue);
        }

        position rightPos = nodePos;
        rightPos.x += 1;
        if (posIsOpenAndUnseen(rightPos, unitIDs, numUnits, map, mapWidth, seenList, *numSeen)) {
            processPathPosition(numPathNodes, rightPos, pathNode, seenList, numSeen, pathfindingQueue);
        }

        position belowPos = nodePos;
        belowPos.y += 1;
        if (posIsOpenAndUnseen(belowPos, unitIDs, numUnits, map, mapWidth, seenList, *numSeen)) {
            processPathPosition(numPathNodes, belowPos, pathNode, seenList, numSeen, pathfindingQueue);
        }
    }

    if (endNode) {
        path_to_space *pathToSpace = &possiblePaths[*numPossiblePaths];
        ++(*numPossiblePaths);
        addPathNodeToPath(pathToSpace, endNode);
    }
}


int main (int argc, char **argv) {
    char *map = (char *)malloc(2000 * sizeof(char));
    memset(map, 0, 2000 * sizeof(char));
    int mapWidth = 0;
    int mapHeight = 0;
    int numMapTiles = 0;

    const int maxUnits = 100;
    units = (map_unit *)malloc(maxUnits * sizeof(map_unit));
    int numUnits = 0;

    int *unitIDs = (int *)malloc(maxUnits * sizeof(int));

    const int stringLength = 40;
    char line[stringLength];
    while (fgets(line, stringLength, stdin)) {
        int width = 0;
        char *currentLetter = line;
        while (*currentLetter != '\n' && *currentLetter != 0) {
            switch (*currentLetter) {
                default: 
                {
                    map[numMapTiles] = *currentLetter;
                } break;
                case 'G': 
                {
                    ASSERT(numUnits < maxUnits);
                    map_unit *goblin = &units[numUnits];
                    *goblin = {};
                    goblin->id = numUnits;
                    unitIDs[numUnits] = goblin->id;
                    ++numUnits;

                    goblin->type = UNIT_TYPE_GOBLIN;
                    goblin->pos.x = width;
                    goblin->pos.y = mapHeight;
                    goblin->hitPoints = 200;
                    map[numMapTiles] = '.';
                }
                break;
                case 'E':
                {
                    ASSERT(numUnits < maxUnits);
                    map_unit *elf = &units[numUnits];
                    *elf = {};
                    elf->id = numUnits;
                    unitIDs[numUnits] = elf->id;
                    ++numUnits;

                    elf->type = UNIT_TYPE_ELF;
                    elf->pos.x = width;
                    elf->pos.y = mapHeight;
                    elf->hitPoints = 200;
                    map[numMapTiles] = '.';
                }
                break;
            }
            ++numMapTiles;
            ++width;
            ++currentLetter;
        }
        mapWidth = width;
        ++mapHeight;
    }

    // working memory for unit logic
    int *targetIDs = (int *)malloc(maxUnits * sizeof(int));
    int numTargets = 0;

    int *adjacentTargetIDs = (int *)malloc(maxUnits * sizeof(int));
    int numAdjacentTargets = 0;

    position *openSquares = (position *)malloc(maxUnits * 4 * sizeof(position));
    int numOpenSquares = 0;

    path_to_space *possiblePaths = (path_to_space *)malloc(100 * sizeof(path_to_space));
    int numPossiblePaths = 0;

    pathNodes = (path_node *)malloc(10000 * sizeof(path_node));
    int numPathNodes = 0;

    position *seenList = (position *)malloc(10000 * sizeof(position));
    int numSeen = 0;

    int *queueMemory = (int *)malloc(100000 * sizeof(int));
    path_node_queue pathfindingQueue = {};

    int round = 0;
    while (true) {
        printf("round %d\n", round);
        printMap(map, mapWidth, mapHeight, numUnits);
        printf("\n");

        if (round == 24) {
            int t = 903874;
        }

        qsort(unitIDs, numUnits, sizeof(int), unitSorter);

        for (int unitIndex = 0; unitIndex < numUnits; ++unitIndex) {
            int unitID = unitIDs[unitIndex];
            map_unit *unit = &units[unitID];
            if (unit->dead) { continue; }
            unit_type unitType = unit->type;

            numTargets = 0;
            if (unitType == UNIT_TYPE_GOBLIN) {
                for (int targetIndex = 0; targetIndex < numUnits; ++targetIndex) {
                    int targetID = unitIDs[targetIndex];
                    map_unit *target = &units[targetID];
                    if (target->dead) { continue; }
                    if (target->type == UNIT_TYPE_ELF) {
                        ASSERT(numTargets < maxUnits);
                        targetIDs[numTargets] = targetID;
                        numTargets++;
                    }
                }
            }
            else if (unitType == UNIT_TYPE_ELF) {
                for (int targetIndex = 0; targetIndex < numUnits; ++targetIndex) {
                    int targetID = unitIDs[targetIndex];
                    map_unit *target = &units[targetID];
                    if (target->dead) { continue; }
                    if (target->type == UNIT_TYPE_GOBLIN) {
                        ASSERT(numTargets < maxUnits);
                        targetIDs[numTargets] = targetID;
                        numTargets++;
                    }
                }
            }

            if (numTargets == 0) {
                // combat over
                goto combatOver;
            }

            numAdjacentTargets = 0;
            numOpenSquares = 0;
            numPossiblePaths = 0;
            memset(possiblePaths, 0, 100 * sizeof(path_to_space));
            findAdjacentTargets(unitID, targetIDs, numTargets, adjacentTargetIDs, &numAdjacentTargets);

            if (numAdjacentTargets == 0) {
                for (int targetIndex = 0; targetIndex < numTargets; ++targetIndex) {
                    int targetID = targetIDs[targetIndex];
                    map_unit *target = &units[targetID];
                    position targetPos = target->pos;

                    position abovePos = targetPos;
                    abovePos.y -= 1;
                    addSquareIfOpen(abovePos, unitIDs, numUnits, map, mapWidth, openSquares, &numOpenSquares);

                    position leftPos = targetPos;
                    leftPos.x -= 1;
                    addSquareIfOpen(leftPos, unitIDs, numUnits, map, mapWidth, openSquares, &numOpenSquares);

                    position rightPos = targetPos;
                    rightPos.x += 1;
                    addSquareIfOpen(rightPos, unitIDs, numUnits, map, mapWidth, openSquares, &numOpenSquares);

                    position belowPos = targetPos;
                    belowPos.y += 1;
                    addSquareIfOpen(belowPos, unitIDs, numUnits, map, mapWidth, openSquares, &numOpenSquares);
                }

                if (numOpenSquares > 0) {
                    for (int openSquareIndex = 0; openSquareIndex < numOpenSquares; ++openSquareIndex) {
                        memset(queueMemory, 0, 100000 * sizeof(int));
                        pathfindingQueue = {};
                        queueInit(&pathfindingQueue, queueMemory);

                        numPathNodes = 0;
                        numSeen = 0;
                        position openSquare = openSquares[openSquareIndex];
                        findShortestPathToOpenSquare(unit->pos, openSquare, 
                                                     possiblePaths, &numPossiblePaths, 
                                                     &numPathNodes,
                                                     seenList, &numSeen,
                                                     &pathfindingQueue,
                                                     map, mapWidth, unitIDs, numUnits);
                    }

                    if (numPossiblePaths > 0) {
                        int bestPathIndex = 0;
                        int leastNumSpaces = possiblePaths[bestPathIndex].numSpaces;
                        for (int pathIndex = 1; pathIndex < numPossiblePaths; ++pathIndex) {
                            path_to_space *possiblePath = &possiblePaths[pathIndex];
                            if (possiblePath->numSpaces < leastNumSpaces) {
                                bestPathIndex = pathIndex;
                                leastNumSpaces = possiblePath->numSpaces;
                            }
                        }

                        path_to_space *bestPath = &possiblePaths[bestPathIndex];
                        // move along path, first position is the currentposition
                        unit->pos = bestPath->spaces[1];
                        findAdjacentTargets(unitID, targetIDs, numTargets, adjacentTargetIDs, &numAdjacentTargets);
                    }
                }
            }

            // if additional targets were found after moving
            if (numAdjacentTargets != 0) {
                qsort(adjacentTargetIDs, numAdjacentTargets, sizeof(int), adjacentTargetSorter);
                map_unit *adjacentTarget = &units[adjacentTargetIDs[0]];
                adjacentTarget->hitPoints -= 3;
                if (adjacentTarget->hitPoints <= 0) {
                    adjacentTarget->dead = true;
                }
            }
        }
        ++round;
    }
combatOver:

    int outcome = 0;
    for (int i = 0; i < numUnits; ++i) {
        int unitID = unitIDs[i];
        map_unit *unit = &units[unitID];
        if (unit->dead) { continue; }
        outcome += unit->hitPoints;
    }
    printf("%d\n", outcome * round);

    return 0;
}
