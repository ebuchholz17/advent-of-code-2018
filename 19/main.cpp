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

void doInstruction (int registers[6], int opcode, int A, int B, int C, int instructionPointerRegisterIndex) {


    switch (opcode) {
        default: {
            ASSERT(false); // invalid opcode
        } break;
        case OPCODE_ADDR: {
            int aValue = registers[A];
            int bValue = registers[B];
            registers[C] = aValue + bValue;
        } break;
        case OPCODE_ADDI: {
            int aValue = registers[A];
            int bValue = B;
            registers[C] = aValue + bValue;
        } break;
        case OPCODE_MULR: {
            int aValue = registers[A];
            int bValue = registers[B];
            registers[C] = aValue * bValue;
        } break;
        case OPCODE_MULI: {
            int aValue = registers[A];
            int bValue = B;
            registers[C] = aValue * bValue;
        } break;
        case OPCODE_BANR: {
            int aValue = registers[A];
            int bValue = registers[B];
            registers[C] = aValue & bValue;
        } break;
        case OPCODE_BANI: {
            int aValue = registers[A];
            int bValue = B;
            registers[C] = aValue & bValue;
        } break;
        case OPCODE_BORR: {
            int aValue = registers[A];
            int bValue = registers[B];
            registers[C] = aValue | bValue;
        } break;
        case OPCODE_BORI: {
            int aValue = registers[A];
            int bValue = B;
            registers[C] = aValue | bValue;
        } break;
        case OPCODE_SETR: {
            registers[C] = registers[A];
        } break;
        case OPCODE_SETI: {
            registers[C] = A;
        } break;
        case OPCODE_GTIR: {
            int aValue = A;
            int bValue = registers[B];
            registers[C] = aValue > bValue ? 1 : 0;
        } break;
        case OPCODE_GTRI: {
            int aValue = registers[A];
            int bValue = B;
            registers[C] = aValue > bValue ? 1 : 0;
        } break;
        case OPCODE_GTRR: {
            int aValue = registers[A];
            int bValue = registers[B];
            registers[C] = aValue > bValue ? 1 : 0;
        } break;
        case OPCODE_EQIR: {
            int aValue = A;
            int bValue = registers[B];
            registers[C] = aValue == bValue ? 1 : 0;
        } break;
        case OPCODE_EQRI: {
            int aValue = registers[A];
            int bValue = B;
            registers[C] = aValue == bValue ? 1 : 0;
        } break;
        case OPCODE_EQRR: {
            int aValue = registers[A];
            int bValue = registers[B];
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
    int numInstructions = 0;

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

    int registers[6] = {};
    int instructionPointer = 0;

    //registers[0] = 1;
    // program sums up the factors of the number in register 3
    long long iterations = 0;
    while (true) {
        if (instructionPointer < 0 || instructionPointer >= numInstructions) { break; }

        instruction *inst = &instructions[instructionPointer];
        registers[instructionPointerRegisterIndex] = instructionPointer;

        printf("ip=%d [%d, %d, %d, %d, %d, %d] ", instructionPointer, registers[0], registers[1], registers[2], registers[3], registers[4], registers[5]);
        printf("%s %d %d %d ", opcodeNames[inst->opcode], inst->A, inst->B, inst->C);

        doInstruction(registers, inst->opcode, inst->A, inst->B, inst->C, instructionPointerRegisterIndex);

        printf("[%d, %d, %d, %d, %d, %d]\n", registers[0], registers[1], registers[2], registers[3], registers[4], registers[5]);

        instructionPointer = registers[instructionPointerRegisterIndex];
        ++instructionPointer;
        ++iterations;
    }

    printf("%d\n", registers[0]);
    printf("total iterations: %lld\n", iterations);

    return 0;
}
