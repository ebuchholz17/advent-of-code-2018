#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

struct navigation_node {
    int numChildren;
    int numMetadataEntries;
    navigation_node **children;
    int *metadata;
};

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

navigation_node *readNodeFromInput (const int stringLength, char *wholeInput, char **currentLetter, char nodeData[], 
                                    void *nodeMemory, int *memoryUsed, int memoryCapacity) 
{
    navigation_node *node = 
        (navigation_node *)allocateSize(sizeof(navigation_node), nodeMemory, memoryUsed, memoryCapacity);

    *currentLetter = readUntilCharacter(*currentLetter, nodeData, ' ');
    node->numChildren = atoi(nodeData);
    (*currentLetter)++;

    *currentLetter = readUntilCharacter(*currentLetter, nodeData, ' ');
    node->numMetadataEntries = atoi(nodeData);
    (*currentLetter)++;

    if (node->numChildren > 0) {
        node->children = (navigation_node **)allocateSize(node->numChildren * sizeof(navigation_node *), 
                                                         nodeMemory, memoryUsed, memoryCapacity);
        for (int i = 0; i < node->numChildren; ++i) {
            node->children[i] = 
                readNodeFromInput(stringLength, wholeInput, currentLetter, nodeData, nodeMemory, memoryUsed, memoryCapacity);
        }
    }

    if (node->numMetadataEntries > 0) {
        node->metadata = (int *)allocateSize(node->numMetadataEntries * sizeof(int), nodeMemory, memoryUsed, memoryCapacity);

        for (int i = 0; i < node->numMetadataEntries; ++i) {
            *currentLetter = readUntilCharacter(*currentLetter, nodeData, ' ');
            node->metadata[i] = atoi(nodeData);
            (*currentLetter)++;
        }
    }

    return node;
}

int sumMetadataEntriesForNode (navigation_node *node) {
    int sum = 0;
    if (node->numChildren == 0) {
        for (int i = 0; i < node->numMetadataEntries; ++i) {
            sum += node->metadata[i];
        }
    }
    else {
        for (int i = 0; i < node->numMetadataEntries; ++i) {
            int nodeIndex = node->metadata[i] - 1;
            if (nodeIndex < node->numChildren) {
                sum += sumMetadataEntriesForNode(node->children[nodeIndex]);
            }
        }
    }
    return sum;
}

int main (int argc, char **argv) {
    int memoryUsed = 0;
    int memoryCapacity = 1024 * 1024 * 10;
    void *nodeMemory = malloc(memoryCapacity);

    char *wholeInput = (char *)malloc(100000 * sizeof(char));
    fgets(wholeInput, 100000, stdin);
    char *currentLetter = wholeInput;

    const int stringLength = 10;
    char nodeData[stringLength];

    navigation_node *rootNode = 
        readNodeFromInput(stringLength, wholeInput, &currentLetter, nodeData, nodeMemory, &memoryUsed, memoryCapacity);

    int metadataSum = sumMetadataEntriesForNode(rootNode);
    printf("%d", metadataSum);

    return 0;
}
