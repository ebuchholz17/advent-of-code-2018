#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

struct room {
    int id;
    int northID;
    int southID;
    int eastID;
    int westID;
};

room *rooms;
int numRooms;
int maxDepth;
int atLeast1000DoorsDeepCounter = 0;

int addRoom () {
    room *newRoom = &rooms[numRooms];
    *newRoom = {};
    newRoom->id = numRooms;
    newRoom->northID = -1;
    newRoom->southID = -1;
    newRoom->eastID = -1;
    newRoom->westID = -1;
    numRooms++;

    return newRoom->id;
}

void parseDirectionsString (int currentRoomID, char *directionsStart, char *directionsEnd, int depth) {
    char *currentCharacter = directionsStart;
    room *currentRoom = &rooms[currentRoomID];
    do {
        switch (*currentCharacter) {
            case 'N':
            {
                if (currentRoom->northID == -1) {
                    int newRoomID = addRoom();
                    room *newRoom = &rooms[newRoomID];
                    currentRoom->northID = newRoomID;
                    newRoom->southID = currentRoomID;
                    ++depth;
                    if (depth > maxDepth) {
                        maxDepth = depth;
                    }
                    if (depth >= 1000) {
                        atLeast1000DoorsDeepCounter++;
                    }
                }
                else {
                    --depth; // assumes this can only happen for backtracking from dead-ends
                }
                currentRoomID = currentRoom->northID;
                currentRoom = &rooms[currentRoomID];
            } break;
            case 'S':
            {
                if (currentRoom->southID == -1) {
                    int newRoomID = addRoom();
                    room *newRoom = &rooms[newRoomID];
                    currentRoom->southID = newRoomID;
                    newRoom->northID = currentRoomID;
                    ++depth;
                    if (depth > maxDepth) {
                        maxDepth = depth;
                    }
                    if (depth >= 1000) {
                        atLeast1000DoorsDeepCounter++;
                    }
                }
                else {
                    --depth; // assumes this can only happen for backtracking from dead-ends
                }
                currentRoomID = currentRoom->southID;
                currentRoom = &rooms[currentRoomID];
            } break;
            case 'E':
            {
                if (currentRoom->eastID == -1) {
                    int newRoomID = addRoom();
                    room *newRoom = &rooms[newRoomID];
                    currentRoom->eastID = newRoomID;
                    newRoom->westID = currentRoomID;
                    ++depth;
                    if (depth > maxDepth) {
                        maxDepth = depth;
                    }
                    if (depth >= 1000) {
                        atLeast1000DoorsDeepCounter++;
                    }
                }
                else {
                    --depth; // assumes this can only happen for backtracking from dead-ends
                }
                currentRoomID = currentRoom->eastID;
                currentRoom = &rooms[currentRoomID];
            } break;
            case 'W':
            {
                if (currentRoom->westID == -1) {
                    int newRoomID = addRoom();
                    room *newRoom = &rooms[newRoomID];
                    currentRoom->westID = newRoomID;
                    newRoom->eastID = currentRoomID;
                    ++depth;
                    if (depth > maxDepth) {
                        maxDepth = depth;
                    }
                    if (depth >= 1000) {
                        atLeast1000DoorsDeepCounter++;
                    }
                }
                else {
                    --depth; // assumes this can only happen for backtracking from dead-ends
                }
                currentRoomID = currentRoom->westID;
                currentRoom = &rooms[currentRoomID];
            } break;
            case '(':
            {
                // scan until | is found
                char *parenthesisStart = currentCharacter + 1;
                char *parenthesisEnd = parenthesisStart;
                int leftParenCount = 0;

                char *subDirectionsStart = parenthesisStart;
                char *subDirectionsEnd = 0;
                while (true) {
                    char parenthesizedCharacter = *parenthesisEnd;
                    bool foundEnd = false;
                    switch (parenthesizedCharacter) {
                        case '(':
                        {
                            leftParenCount++;
                        } break;
                        case ')':
                        {
                            if (leftParenCount == 0) {
                                foundEnd = true;
                            }
                            else {
                                leftParenCount--;
                            }
                        } break;
                        case '|':
                        {
                            if (leftParenCount == 0) {
                                subDirectionsEnd = parenthesisEnd - 1;
                                parseDirectionsString(currentRoomID, subDirectionsStart, subDirectionsEnd, depth);
                                subDirectionsStart = parenthesisEnd + 1;
                                subDirectionsEnd = 0;
                            }
                        } break;
                    }
                    if (foundEnd) {
                        subDirectionsEnd = parenthesisEnd - 1;
                        currentCharacter = parenthesisEnd;
                        if (subDirectionsStart <= subDirectionsEnd) {
                            parseDirectionsString(currentRoomID, subDirectionsStart, subDirectionsEnd, depth);
                        }
                        break;
                    }
                    else {
                        ++parenthesisEnd;
                    }
                }
            } break;
        }
        ++currentCharacter;
    } while (currentCharacter <= directionsEnd);
}

int main (int argc, char **argv) {
    rooms = (room *)malloc(20000 * sizeof(room));
    numRooms = 0;
    maxDepth = 0;

    const int stringLength = 20000;
    char line[stringLength];

    fgets(line, stringLength, stdin);

    int startingRoomID = addRoom();

    char *directionsStart = line + 1;
    char *directionsEnd = directionsStart;
    while (*directionsEnd != '$') {
        ++directionsEnd;
    }
    parseDirectionsString(startingRoomID, directionsStart, directionsEnd, 0);
    printf("%d\n", maxDepth);
    printf("%d\n", atLeast1000DoorsDeepCounter);

    return 0;
}
