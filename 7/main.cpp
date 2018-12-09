#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

struct instruction {
    char name;
    int index;

    instruction *nextSteps[30]; // TODO(ebuchholz): maybe use indexes instead
    int numNextSteps;
    instruction *prevSteps[30];
    int numPrevSteps;

    bool available;
    bool complete;
};

// global to use qsort
// TODO(ebuchholz): make my own sort
instruction *instructions;

instruction *getInstructionByName (char name, int numInstructions) {
    instruction *result = 0;
    for (int i = 0; i < numInstructions; ++i) {
        instruction *step = &instructions[i];
        if (step->name == name) {
            result = step;
            break;
        }
    }
    return result;
}

// gets instruction from global array by index, sorts by name
int instructionSorter (const void *a, const void *b) {
    int *stepIndexA = (int *)a;
    int *stepIndexB = (int *)b;
    instruction *stepA = &instructions[*stepIndexA];
    instruction *stepB = &instructions[*stepIndexB];

    if (stepA->name < stepB->name) { return -1; }
    if (stepA->name == stepB->name) { return 0; }
    return 1;
}


// TODO(ebuchholz): checks for running out of space, right now we assume memory will never run out for the inputs given
// also make it use a ring buffer
struct instruction_queue {
    int *entries;
    int firstIndex;
    int lastIndex;
};

void queueInit (instruction_queue *queue, int *memory) {
    queue->entries = memory;
    queue->firstIndex = 0;
    queue->lastIndex = 0;
}

void queuePush (int stepIndex, instruction_queue *queue) {
    queue->entries[queue->lastIndex] = stepIndex;
    queue->lastIndex++;
}

int queuePop (instruction_queue *queue) {
    int result = queue->entries[queue->firstIndex];
    queue->firstIndex++;
    return result;
}

bool queueItemExists (int index, instruction_queue *queue) {
    for (int i = queue->firstIndex; i < queue->lastIndex; ++i) {
        if (queue->entries[i] == index) { return true; }
    }
    return false;
}

int queueItemCount (instruction_queue *queue) {
    return queue->lastIndex - queue->firstIndex;
}

void queueSort (instruction_queue *queue) {
    int numQueueItems = queueItemCount(queue);
    int *start = &queue->entries[queue->firstIndex];
    qsort(start, numQueueItems, sizeof(int), instructionSorter);
}


bool listItemExists (int index, int *list, int listCount) {
    for (int i = 0; i < listCount; ++i) {
        if (list[i] == index) { return true; }
    }

    return false;
}

struct worker {
    int stepIndex;
    int workDone;
    int workRequired;
};

#define NUM_WORKERS 5

worker *getAvailableWorker (worker *workers) {
    worker *result = 0;
    for (int i = 0; i < NUM_WORKERS; ++i) {
        worker *worker = &workers[i];
        if (worker->stepIndex == -1) { return worker; }
    }
    return result;
}

void printStep (instruction *step) {
    printf("%c", step->name);
    for (int i = 0; i < step->numNextSteps; ++i) {
        printStep(step->nextSteps[i]);
    }
}

void printStepBackwards (instruction *step) {
    printf("%c", step->name);
    for (int i = 0; i < step->numPrevSteps; ++i) {
        printStepBackwards(step->prevSteps[i]);
    }
}

void printWorkProgress (int time, worker *workers, instruction_queue *doneQueue) {
    printf("%d ", time);
    for (int i = 0; i < NUM_WORKERS; ++i) {
        worker *worker = &workers[i];
        if (worker->stepIndex != -1) {
            instruction *step = &instructions[worker->stepIndex];
            printf("%c ", step->name);
        }
        else {
            printf(". ");
        }
    }
    for (int i = doneQueue->firstIndex; i < doneQueue->lastIndex; ++i) {
        int stepIndex = doneQueue->entries[i];
        instruction *step = &instructions[stepIndex];
        printf("%c", step->name);
    }
    printf("\n");
}


int main (int argc, char **argv) {
    instructions = (instruction *)malloc(200 * sizeof(instruction));
    int numInstructions = 0;

    const int stringLength = 50;
    char word[stringLength];
    while (fgets(word, stringLength, stdin)) {
        char prereqName = word[5];
        char nextStepName = word[36];

        instruction *prereq = getInstructionByName(prereqName, numInstructions);
        if (!prereq) {
            prereq = &instructions[numInstructions];

            *prereq = {};
            prereq->name = prereqName;
            prereq->index = numInstructions;

            ++numInstructions;
        }

        instruction *nextStep = getInstructionByName(nextStepName, numInstructions);
        if (!nextStep) {
            nextStep = &instructions[numInstructions];

            *nextStep = {};
            nextStep->name = nextStepName;
            nextStep->index = numInstructions;

            ++numInstructions;
        }

        prereq->nextSteps[prereq->numNextSteps] = nextStep;
        prereq->numNextSteps++;

        nextStep->prevSteps[nextStep->numPrevSteps] = prereq;
        nextStep->numPrevSteps++;
    }

    // assumes only one first step
    instruction *firstStep = 0;
    for (int i = 0; i < numInstructions; ++i) {
        instruction *step = &instructions[i];
        if (step->numPrevSteps == 0) {
            firstStep = step;
            break;
        }
    }

    int *queueMemory = (int *)malloc(100000 * sizeof(int));
    instruction_queue knownQueue = {};
    queueInit(&knownQueue, queueMemory);
    queuePush(firstStep->index, &knownQueue);

    queueMemory = (int *)malloc(100000 * sizeof(int));
    instruction_queue availableQueue = {};
    queueInit(&availableQueue, queueMemory);

    queueMemory = (int *)malloc(1000 * sizeof(int));
    instruction_queue doneQueue = {};
    queueInit(&doneQueue, queueMemory);

    worker workers[NUM_WORKERS] = {};
    for (int i = 0; i < NUM_WORKERS; ++i) {
        worker *worker = &workers[i];
        worker->stepIndex = -1;
    }

    bool done = false;
    int time = 0;
    while (!done) {
        // figure out which steps are available
        queueSort(&knownQueue);

        while (queueItemCount(&knownQueue) > 0) {
            int stepIndex = queuePop(&knownQueue);
            instruction *step = &instructions[stepIndex];
            
            if (!step->available) {
                if (step->numPrevSteps == 0) {
                    step->available = true;
                }
                else {
                    bool allPrevStepsComplete = true;
                    for (int i = 0; i < step->numPrevSteps; ++i) {
                        instruction *prevStep = step->prevSteps[i];
                        if (!prevStep->complete) {
                            allPrevStepsComplete = false;
                            break;
                        }
                    }
                    if (allPrevStepsComplete) {
                        step->available = true;
                    }
                }
            }

            if (step->available) {
                queuePush(step->index, &availableQueue);
            }
        }

        // assign available steps to workers
        queueSort(&availableQueue);

        while (queueItemCount(&availableQueue) > 0) {
            int stepIndex = queuePop(&availableQueue);
            instruction *step = &instructions[stepIndex];

            worker *worker = getAvailableWorker(workers);
            if (worker) {
                worker->stepIndex = stepIndex;
                worker->workDone = 0;
                //worker->workRequired = (step->name - 'A') + 1;
                worker->workRequired = (step->name - 'A') + 61;
            }
            else {
                queuePush(stepIndex, &knownQueue);
            }
        }

        // do work
        for (int i = 0; i < NUM_WORKERS; ++i) {
            worker *worker = &workers[i];
            if (worker->stepIndex != -1) {
                worker->workDone += 1;
            }
        }

        // print progress
        printWorkProgress(time, workers, &doneQueue);

        // mark work as complete, free workers
        for (int i = 0; i < NUM_WORKERS; ++i) {
            worker *worker = &workers[i];
            if (worker->stepIndex != -1 && worker->workDone == worker->workRequired) {
                instruction *step = &instructions[worker->stepIndex];
                step->complete = true;

                worker->stepIndex = -1;
                queuePush(step->index, &doneQueue);

                for (int j = 0; j < step->numNextSteps; ++j) {
                    instruction *nextStep = step->nextSteps[j];
                    int nextIndex = nextStep->index;
                    if (!queueItemExists(nextIndex, &knownQueue)) {
                        queuePush(nextIndex, &knownQueue);
                    }
                }
            }
        }

        bool workersAllIdle = true;
        for (int i = 0; i < NUM_WORKERS; ++i) {
            worker *worker = &workers[i];
            if (worker->stepIndex != -1) {
                workersAllIdle = false;
                break;
            }
        }

        ++time;
        if (workersAllIdle && 
            queueItemCount(&knownQueue) == 0 && 
            queueItemCount(&availableQueue) == 0) 
        {
            done = true;
        }
    }

    printWorkProgress(time, workers, &doneQueue);

    // Test tree traversal
    // assume only one first step
    //instruction *firstStep = 0;
    //for (int i = 0; i < numInstructions; ++i) {
    //    instruction *step = &instructions[i];
    //    if (step->numPrevSteps == 0) {
    //        firstStep = step;
    //        break;
    //    }
    //}
    //if (firstStep) {
    //    printStep(firstStep);
    //}

    //printf("\n");

    //instruction *lastStep = 0;
    //for (int i = 0; i < numInstructions; ++i) {
    //    instruction *step = &instructions[i];
    //    if (step->numNextSteps == 0) {
    //        lastStep = step;
    //        break;
    //    }
    //}
    //if (lastStep) {
    //    printStepBackwards(lastStep);
    //}

    return 0;
}
