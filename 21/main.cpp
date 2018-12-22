#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

enum opcode {
    OPCODE_ADDR,
    OPCODE_ADDI,
    OPCODE_MULR,
    OPCODE_MULI,
    OPCODE_BANR,
    OPCODE_BANI,
    OPCODE_BORR,
    OPCODE_BORI,
    OPCODE_SETR,
    OPCODE_SETI,
    OPCODE_GTIR,
    OPCODE_GTRI,
    OPCODE_GTRR,
    OPCODE_EQIR,
    OPCODE_EQRI,
    OPCODE_EQRR,
    OPCODE_COUNT
};

struct instruction {
    int opcode;
    int A;
    int B;
    int C;
};

void doInstruction (long long registers[6], int opcode, long long A, long long B, long long C, int instructionPointerRegisterIndex) {
    switch (opcode) {
        default: {
            ASSERT(false); // invalid opcode
        } break;
        case OPCODE_ADDR: {
            long long aValue = registers[A];
            long long bValue = registers[B];
            registers[C] = aValue + bValue;
        } break;
        case OPCODE_ADDI: {
            long long aValue = registers[A];
            long long bValue = B;
            registers[C] = aValue + bValue;
        } break;
        case OPCODE_MULR: {
            long long aValue = registers[A];
            long long bValue = registers[B];
            registers[C] = aValue * bValue;
        } break;
        case OPCODE_MULI: {
            long long aValue = registers[A];
            long long bValue = B;
            registers[C] = aValue * bValue;
        } break;
        case OPCODE_BANR: {
            long long aValue = registers[A];
            long long bValue = registers[B];
            registers[C] = aValue & bValue;
        } break;
        case OPCODE_BANI: {
            long long aValue = registers[A];
            long long bValue = B;
            registers[C] = aValue & bValue;
        } break;
        case OPCODE_BORR: {
            long long aValue = registers[A];
            long long bValue = registers[B];
            registers[C] = aValue | bValue;
        } break;
        case OPCODE_BORI: {
            long long aValue = registers[A];
            long long bValue = B;
            registers[C] = aValue | bValue;
        } break;
        case OPCODE_SETR: {
            registers[C] = registers[A];
        } break;
        case OPCODE_SETI: {
            registers[C] = A;
        } break;
        case OPCODE_GTIR: {
            long long aValue = A;
            long long bValue = registers[B];
            registers[C] = aValue > bValue ? 1 : 0;
        } break;
        case OPCODE_GTRI: {
            long long aValue = registers[A];
            long long bValue = B;
            registers[C] = aValue > bValue ? 1 : 0;
        } break;
        case OPCODE_GTRR: {
            long long aValue = registers[A];
            long long bValue = registers[B];
            registers[C] = aValue > bValue ? 1 : 0;
        } break;
        case OPCODE_EQIR: {
            long long aValue = A;
            long long bValue = registers[B];
            registers[C] = aValue == bValue ? 1 : 0;
        } break;
        case OPCODE_EQRI: {
            long long aValue = registers[A];
            long long bValue = B;
            registers[C] = aValue == bValue ? 1 : 0;
        } break;
        case OPCODE_EQRR: {
            long long aValue = registers[A];
            long long bValue = registers[B];
            registers[C] = aValue == bValue ? 1 : 0;
        } break;
    }
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

int main (int argc, char **argv) {
    const int stringLength = 40;
    char line[stringLength];

    instruction *instructions = (instruction *)malloc(100 * sizeof(instruction));
    long long numInstructions = 0;

    fgets(line, stringLength, stdin);
    char word[stringLength] = {};
    char *currentLetter = line;

    currentLetter += 4;
    int instructionPointerRegisterIndex = *currentLetter - '0';

    char *opcodeNames[] = {
        "addr",
        "addi",
        "mulr",
        "muli",
        "banr",
        "bani",
        "borr",
        "bori",
        "setr",
        "seti",
        "gtir",
        "gtri",
        "gtrr",
        "eqir",
        "eqri",
        "eqrr"
    };

    while (fgets(line, stringLength, stdin)) {
        currentLetter = line;

        instruction *inst = &instructions[numInstructions];
        ++numInstructions;
        inst->opcode = -1;

        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        ++currentLetter;
        char *opcodeName = word;
        for (int i = 0; i < 16; ++i) {
            char *opcode = opcodeNames[i];
            bool matching = true;
            for (int j = 0; j < 4; ++j) {
                if (opcode[j] != opcodeName[j]) { 
                    matching = false;
                    break;
                }
            }
            if (matching) {
                inst->opcode = i;
                break;
            }
        }
        ASSERT(inst->opcode != -1);

        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        ++currentLetter;
        inst->A = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        ++currentLetter;
        inst->B = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, '\n');
        inst->C = atoi(word);
    }

    //printf("#ip %d\n", instructionPointerRegisterIndex);
    //for (int i = 0; i < numInstructions; ++i) {
    //    instruction *inst = &instructions[i];
    //    printf("%s %d %d %d\n", opcodeNames[inst->opcode], inst->A, inst->B, inst->C);
    //}

    long long registers[6] = {};
    long long instructionPointer = 0;

    //registers[0] = 1;
    // program sums up the factors of the number in register 3
    long long iterations = 0;
    while (true) {
        if (instructionPointer < 0 || instructionPointer >= numInstructions) { break; }

        instruction *inst = &instructions[instructionPointer];
        registers[instructionPointerRegisterIndex] = instructionPointer;

        // the program exits when this instruction runs
        // print out the registers, find when it first happens, and the last time it happens before the register values repeat (determined by examining output)
        if (inst->opcode == OPCODE_EQRR) { 
            printf("ip=%lld [%lld, %lld, %lld, %lld, %lld, %lld] ", instructionPointer, registers[0], registers[1], registers[2], registers[3], registers[4], registers[5]);
            printf("%s %d %d %d ", opcodeNames[inst->opcode], inst->A, inst->B, inst->C);
        }

        doInstruction(registers, inst->opcode, inst->A, inst->B, inst->C, instructionPointerRegisterIndex);

        if (inst->opcode == OPCODE_EQRR) { 
            printf("[%lld, %lld, %lld, %lld, %lld, %lld]\n", registers[0], registers[1], registers[2], registers[3], registers[4], registers[5]);
        }

        instructionPointer = registers[instructionPointerRegisterIndex];
        ++instructionPointer;
        ++iterations;
    }

    printf("%lld\n", registers[0]);
    printf("total iterations: %lld\n", iterations);

    return 0;
}
