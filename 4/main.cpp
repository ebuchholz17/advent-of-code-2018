#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

struct guard_event {
    unsigned long long seconds;
    char event[100];
};

struct guard {
    int id;
    int minutesAsleep[60];
};

// usage
// char word[stringLength];
// char currentWord[10];
// char *currentLetter = word;
// ...
// currentLetter++;               // skip however many characters
// currentLetter = readUntilCharacter(currentLetter, currentWord, 'x');
// int width = atoi(currentWord); // read value
char *readUntilCharacter (char *currentLetter, char *currentWord, char character) {
    int letterIndex = 0;
    while (*currentLetter != character && currentLetter != 0) {
        currentWord[letterIndex] = *currentLetter;
        letterIndex++;
        currentLetter++;
    }
    currentWord[letterIndex] = 0;
    return currentLetter;
}

int guardSorter (const void *a, const void *b) {
    guard_event *guardEventA = (guard_event *)a;
    guard_event *guardEventB = (guard_event *)b;

    if (guardEventA->seconds < guardEventB->seconds) { return -1; };
    if (guardEventA->seconds == guardEventB->seconds) { return 0; };
    return 1;
}

guard *getGuardByID (int id, guard *guards, int numGuards) {
    for (int i = 0; i < numGuards; ++i) {
        guard *guard = &guards[i];
        if (guard->id == id) { return guard; }
    }
    return 0;
}

int main (int argc, char **argv) {
    guard_event *guardEvents = (guard_event *)malloc(2000 * sizeof(guard_event));
    int numGuardEvents = 0;

    // read events
    const int stringLength = 200;
    char word[stringLength];
    while (fgets(word, stringLength, stdin)) {
        guard_event *guardEvent = &guardEvents[numGuardEvents];
        numGuardEvents++;

        char *currentLetter = word;
        readUntilCharacter(currentLetter, guardEvent->event, '\n');
    }

    unsigned long long daysInMonth[] = {
        31,
        28,
        31,
        30,
        31,
        30,
        31,
        31,
        30,
        31,
        30,
        31
    };

    // convert timestamps to seconds
    for (int i = 0; i < numGuardEvents; ++i) {
        guard_event *guardEvent = &guardEvents[i];

        char *currentLetter = guardEvent->event;
        char currentWord[50];
        // skip [, year, and -
        currentLetter += 6;
        currentLetter = readUntilCharacter(currentLetter, currentWord, '-');
        int month = atoi(currentWord) - 1;

        currentLetter++;
        currentLetter = readUntilCharacter(currentLetter, currentWord, ' ');
        int day = atoi(currentWord) - 1;

        currentLetter++;
        currentLetter = readUntilCharacter(currentLetter, currentWord, ':');
        int minutes = atoi(currentWord);

        currentLetter++;
        currentLetter = readUntilCharacter(currentLetter, currentWord, ']');
        int seconds = atoi(currentWord);

        unsigned long long secondSeconds = (unsigned long long)seconds;
        unsigned long long minuteSeconds = ((unsigned long long)minutes * 60);
        unsigned long long daySeconds = ((unsigned long long)day * 60 * 60 * 24);
        unsigned long long totalDaysInMonth = 0;
        for (int m = 0; m < month; ++m) {
            totalDaysInMonth += daysInMonth[m];
        }
        unsigned long long monthSeconds = ((unsigned long long)month * 60 * 60 * 24 * totalDaysInMonth);

        guardEvent->seconds = secondSeconds + minuteSeconds + daySeconds + monthSeconds;
    }

    qsort(guardEvents, numGuardEvents, sizeof(guard_event), guardSorter);

    guard *guards = (guard *)malloc(1000 * sizeof(guard));
    int numGuards = 0;

    for (int i = 0; i < numGuardEvents; ++i) {
        guard_event *guardEvent = &guardEvents[i];
        char *currentLetter = guardEvent->event;
        char currentWord[50];

        // get the ID, see if guard exists, otherwise create him
        // # always starts on column 26, so id starts on 27
        currentLetter += 26;
        currentLetter = readUntilCharacter(currentLetter, currentWord, ' ');
        int id = atoi(currentWord);

        guard *guard = getGuardByID(id, guards, numGuards);
        if (!guard) {
            guard = &guards[numGuards];
            numGuards++;
            guard->id = id;
            for (int j = 0; j < 60; ++j) {
                guard->minutesAsleep[j] = 0;
            }
        }

        guard_event *nextEvent = guardEvent + 1; // should always exist with this input
        currentLetter = nextEvent->event;
        currentLetter += 19; //. find "falls asleep" or "wakes up" message
        while (*currentLetter != 'G') {
            ++i;
            // find start time of falling asleep
            currentLetter = nextEvent->event;
            currentLetter = readUntilCharacter(currentLetter, currentWord, ':');
            currentLetter++;
            currentLetter = readUntilCharacter(currentLetter, currentWord, ']');
            int startMinute = atoi(currentWord);

            // next one must be a wake up event
            ++i;
            nextEvent++;
            currentLetter = nextEvent->event;
            currentLetter = readUntilCharacter(currentLetter, currentWord, ':');
            currentLetter++;
            currentLetter = readUntilCharacter(currentLetter, currentWord, ']');
            int endMinute = atoi(currentWord);

            for (int minuteIndex = startMinute; minuteIndex < endMinute; ++minuteIndex) {
                guard->minutesAsleep[minuteIndex]++;
            }

            if (i + 1 == numGuardEvents) { break; }
            else {
                nextEvent++;
                currentLetter = nextEvent->event;
                currentLetter += 19;
            }
        }
    }

    int bestMinuteIndex = 0;
    int bestGuardID = 0;
    int mostTimesSleptOnMinute = 0;
    for (int i = 0; i < numGuards; ++i) {
        guard *guard = &guards[i];
        int bestMinuteIndexForGuard = 0;
        int minutesMostAsleep = 0;
        for (int j = 0; j < 60; ++j) {
            if (guard->minutesAsleep[j] > mostTimesSleptOnMinute) {
                mostTimesSleptOnMinute = guard->minutesAsleep[j];
                bestGuardID = guard->id;
                bestMinuteIndex = j;
            }
        }
    }
    printf("%d\n", bestGuardID);
    printf("%d\n", bestMinuteIndex);
    printf("%d\n", bestGuardID * bestMinuteIndex);
    return 0;
}
