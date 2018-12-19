#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

enum direction {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
};

enum turn {
    TURN_LEFT,
    TURN_STRAIGHT,
    TURN_RIGHT,
    TURN_COUNT
};

struct cart {
    int x;
    int y;
    direction dir;
    turn nextTurn;
    bool crashed;
};

void printMap (char *tracks, int trackWidth, int trackHeight, cart *carts, int numCarts, int crashX, int crashY) {
    for (int y = 0; y < trackHeight; ++y) {
        for (int x = 0; x < trackWidth; ++x) {
            if (crashX == x && crashY == y) {
                printf("X");
            }
            else {
                bool foundCart = false;
                for (int cartIndex = 0; cartIndex < numCarts; ++cartIndex) {
                    cart *cart = &carts[cartIndex];
                    if (cart->x == x && cart->y == y) {
                        switch (cart->dir) {
                            case DIRECTION_UP:
                                printf("%c", '^');
                                break;
                            case DIRECTION_DOWN:
                                printf("%c", 'v');
                                break;
                            case DIRECTION_LEFT:
                                printf("%c", '<');
                                break;
                            case DIRECTION_RIGHT:
                                printf("%c", '>');
                                break;
                        }
                        foundCart = true;
                        break;
                    }
                }
                if (!foundCart) {
                    char track = tracks[y * trackWidth + x];
                    switch (track) {
                        default:
                            printf(" ");
                            break;
                        case '-':
                        case '|':
                        case '/':
                        case '\\':
                        case '+':
                            printf("%c", track);
                            break;
                    }

                }
            }
        }
        printf("\n");
    }
}

int cartSorter (const void *a, const void *b) {
    cart *cartA = (cart *)a;
    cart *cartB = (cart *)b;

    if (cartA->y < cartB->y) { return -1; }
    if (cartA->y > cartB->y) { return 1; }
    if (cartA->x < cartB->x) { return -1; }
    if (cartA->x > cartB->x) { return 1; }

    return 0;
}


int main (int argc, char **argv) {
    cart *carts = (cart *)malloc(100 * sizeof(cart));
    int numCarts = 0;

    char *tracks = (char *)malloc(30000 * sizeof(char));
    memset(tracks, 0, 30000 * sizeof(char));
    int trackWidth = 0;
    int trackHeight = 0;
    int numTracks = 0;

    const int stringLength = 200;
    char line[stringLength];
    int height = 0;
    while (fgets(line, stringLength, stdin)) {
        int width = 0;
        char *currentLetter = line;
        while (*currentLetter != '\n' && *currentLetter != 0) {
            char track = *currentLetter;
            switch (track) {
                default:
                    // no track or cart here
                    break;
                case '-':
                case '|':
                case '/':
                case '\\':
                case '+':
                    tracks[numTracks] = track;
                    break;
                case '>':
                {
                    cart *cart = &carts[numCarts];
                    numCarts++;
                    cart->x = width;
                    cart->y = height;
                    cart->dir = DIRECTION_RIGHT;
                    cart->nextTurn = TURN_LEFT;
                    cart->crashed = false;

                    tracks[numTracks] = '-';
                } break;
                case '<':
                {
                    cart *cart = &carts[numCarts];
                    numCarts++;
                    cart->x = width;
                    cart->y = height;
                    cart->dir = DIRECTION_LEFT;
                    cart->nextTurn = TURN_LEFT;
                    cart->crashed = false;

                    tracks[numTracks] = '-';
                } break;
                case '^':
                {
                    cart *cart = &carts[numCarts];
                    numCarts++;
                    cart->x = width;
                    cart->y = height;
                    cart->dir = DIRECTION_UP;
                    cart->nextTurn = TURN_LEFT;
                    cart->crashed = false;

                    tracks[numTracks] = '|';
                } break;
                case 'v':
                {
                    cart *cart = &carts[numCarts];
                    numCarts++;
                    cart->x = width;
                    cart->y = height;
                    cart->dir = DIRECTION_DOWN;
                    cart->nextTurn = TURN_LEFT;
                    cart->crashed = false;

                    tracks[numTracks] = '|';
                } break;
            }
            ++numTracks;
            ++width;
            if (width > 150) {
                int test = 0;
            }
            ++currentLetter;
        }
        if (width > trackWidth) {
            trackWidth = width;
        }
        ++height;
    }
    trackHeight = height;

    int time = 0;
    int crashX = -1;
    int crashY = -1;
    int lastCartX = -1;
    int lastCartY = -1;
    printMap(tracks, trackWidth, trackHeight, carts, numCarts, crashX, crashY);
    while (true) {
        qsort(carts, numCarts, sizeof(cart), cartSorter);

        for (int i = 0; i < numCarts; ++i) {
            cart *firstCart = &carts[i];
            if (firstCart->crashed) { continue; }
            switch (firstCart->dir) {
                case DIRECTION_UP:
                    firstCart->y -= 1;
                    break;
                case DIRECTION_DOWN:
                    firstCart->y += 1;
                    break;
                case DIRECTION_LEFT:
                    firstCart->x -= 1;
                    break;
                case DIRECTION_RIGHT:
                    firstCart->x += 1;
                    break;
            }

            for (int j = 0; j < numCarts; ++j) {
                if (i == j) { continue; }

                cart *secondCart = &carts[j];
                if (secondCart->crashed) { continue; }
                if (firstCart->x == secondCart->x && firstCart->y == secondCart->y) {
                    crashX = firstCart->x;
                    crashY = firstCart->y;
                    firstCart->crashed = true;
                    secondCart->crashed = true;
                    break;
                    //goto cartUpdateDone;
                }
            }

            char track = tracks[firstCart->y * trackWidth + firstCart->x];
            switch (track) {
                case '/':
                    switch (firstCart->dir) {
                        case DIRECTION_UP:
                            firstCart->dir = DIRECTION_RIGHT;
                            break;
                        case DIRECTION_DOWN:
                            firstCart->dir = DIRECTION_LEFT;
                            break;
                        case DIRECTION_LEFT:
                            firstCart->dir = DIRECTION_DOWN;
                            break;
                        case DIRECTION_RIGHT:
                            firstCart->dir = DIRECTION_UP;
                            break;
                    }
                    break;
                case '\\':
                    switch (firstCart->dir) {
                        case DIRECTION_UP:
                            firstCart->dir = DIRECTION_LEFT;
                            break;
                        case DIRECTION_DOWN:
                            firstCart->dir = DIRECTION_RIGHT;
                            break;
                        case DIRECTION_LEFT:
                            firstCart->dir = DIRECTION_UP;
                            break;
                        case DIRECTION_RIGHT:
                            firstCart->dir = DIRECTION_DOWN;
                            break;
                    }
                    break;
                case '+':
                    direction leftDirection = DIRECTION_LEFT;
                    direction straightDirection = DIRECTION_LEFT;
                    direction rightDirection = DIRECTION_LEFT;
                    switch (firstCart->dir) {
                        case DIRECTION_UP:
                            leftDirection = DIRECTION_LEFT;
                            straightDirection = DIRECTION_UP;
                            rightDirection = DIRECTION_RIGHT;
                            break;
                        case DIRECTION_DOWN:
                            leftDirection = DIRECTION_RIGHT;
                            straightDirection = DIRECTION_DOWN;
                            rightDirection = DIRECTION_LEFT;
                            break;
                        case DIRECTION_LEFT:
                            leftDirection = DIRECTION_DOWN;
                            straightDirection = DIRECTION_LEFT;
                            rightDirection = DIRECTION_UP;
                            break;
                        case DIRECTION_RIGHT:
                            leftDirection = DIRECTION_UP;
                            straightDirection = DIRECTION_RIGHT;
                            rightDirection = DIRECTION_DOWN;
                            break;
                    }
                    switch (firstCart->nextTurn) {
                        case TURN_LEFT:
                            firstCart->dir = leftDirection;
                            break;
                        case TURN_STRAIGHT:
                            firstCart->dir = straightDirection;
                            break;
                        case TURN_RIGHT:
                            firstCart->dir = rightDirection;
                            break;
                    }
                    firstCart->nextTurn = (turn)(((int)firstCart->nextTurn + 1) % TURN_COUNT);
                    break;
            }
        }
        ++time;
        //printMap(tracks, trackWidth, trackHeight, carts, numCarts, crashX, crashY);

        lastCartX = -1;
        lastCartY = -1;
        int numCartsLeft = 0;
        for (int i = 0; i < numCarts; ++i) {
            cart *cart = &carts[i];
            if (!cart->crashed) {
                lastCartX = cart->x;
                lastCartY = cart->y;
                numCartsLeft++;
            }
        }

        if (numCartsLeft == 1 || numCartsLeft == 0) {
            break;
        }
    }
    printMap(tracks, trackWidth, trackHeight, carts, numCarts, crashX, crashY);
    printf("%d,%d\n", lastCartX, lastCartY);
    return 0;
}
