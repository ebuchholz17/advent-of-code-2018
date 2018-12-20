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

struct sample_operation {
    int registersBefore[4];
    instruction instruction;
    int registersAfter[4];
};

void doInstruction (int registers[4], int opcode, int A, int B, int C) {
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
    sample_operation *samples = (sample_operation *)malloc(1000 * sizeof(sample_operation));
    int numSamples = 0;

    const int stringLength = 40;
    char line[stringLength];
    while (fgets(line, stringLength, stdin)) {
        char *currentLetter = line;
        char word[stringLength] = {};

        if (*currentLetter == '\n') { break; }

        sample_operation *sample = &samples[numSamples];
        ++numSamples;
        *sample = {};

        currentLetter = readUntilCharacter(currentLetter, word, '[');
        ++currentLetter;
        currentLetter = readUntilCharacter(currentLetter, word, ',');
        currentLetter += 2;
        sample->registersBefore[0] = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ',');
        currentLetter += 2;
        sample->registersBefore[1] = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ',');
        currentLetter += 2;
        sample->registersBefore[2] = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ']');
        sample->registersBefore[3] = atoi(word);

        fgets(line, stringLength, stdin);
        currentLetter = line;

        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        ++currentLetter;
        sample->instruction.opcode = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        ++currentLetter;
        sample->instruction.A = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        ++currentLetter;
        sample->instruction.B = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, '\n');
        sample->instruction.C = atoi(word);

        fgets(line, stringLength, stdin);
        currentLetter = line;

        currentLetter = readUntilCharacter(currentLetter, word, '[');
        ++currentLetter;
        currentLetter = readUntilCharacter(currentLetter, word, ',');
        currentLetter += 2;
        sample->registersAfter[0] = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ',');
        currentLetter += 2;
        sample->registersAfter[1] = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ',');
        currentLetter += 2;
        sample->registersAfter[2] = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ']');
        sample->registersAfter[3] = atoi(word);

        fgets(line, stringLength, stdin);
    }

    fgets(line, stringLength, stdin);

    instruction *instructions = (instruction *)malloc(2000 * sizeof(instruction));
    int numInstructions = 0;

    while (fgets(line, stringLength, stdin)) {
        char *currentLetter = line;
        char word[stringLength] = {};

        instruction *inst = &instructions[numInstructions];
        ++numInstructions;

        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        ++currentLetter;
        inst->opcode = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        ++currentLetter;
        inst->A = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        ++currentLetter;
        inst->B = atoi(word);

        currentLetter = readUntilCharacter(currentLetter, word, '\n');
        inst->C = atoi(word);
    }

    //for (int i = 0; i < numSamples; ++i) {
    //    sample_operation *sample = &samples[i];
    //    printf("Before: [%d, %d, %d, %d]\n", sample->registersBefore[0], sample->registersBefore[1], sample->registersBefore[2], sample->registersBefore[3]);
    //    printf("%d %d %d %d\n", sample->instruction.opcode, sample->instruction.A, sample->instruction.B, sample->instruction.C);
    //    printf("After: [%d, %d, %d, %d]\n", sample->registersAfter[0], sample->registersAfter[1], sample->registersAfter[2], samples->registersAfter[3]);
    //}

    int registers[4] = {};
    int matchingOpcodeCounts[OPCODE_COUNT][OPCODE_COUNT] = {};
    for (int sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex) {
        sample_operation *sample = &samples[sampleIndex];
        instruction inst = sample->instruction;

        int matchingOpcodes = 0;
        for (int opcodeIndex = 0; opcodeIndex < OPCODE_COUNT; ++opcodeIndex) {
            for (int regIndex = 0; regIndex < 4; ++regIndex) {
                registers[regIndex] = sample->registersBefore[regIndex];
            }

            doInstruction(registers, opcodeIndex, inst.A, inst.B, inst.C);

            bool matching = true;
            for (int regIndex = 0; regIndex < 4; ++regIndex) {
                if (registers[regIndex] != sample->registersAfter[regIndex]) {
                    matching = false;
                    break;
                }
            }

            if (matching) {
                matchingOpcodeCounts[inst.opcode][opcodeIndex]++;
            }
        }
    }

    for (int i = 0; i < OPCODE_COUNT; ++i) {
        printf("opcode %2d: ", i);
        for (int j = 0; j < OPCODE_COUNT; ++j) {
            printf("%2d ", matchingOpcodeCounts[i][j]);
        }
        printf("\n");
    }

    int opcodeMapping[] = {
        7,
        3,
        4,
        5,
        10,
        8,
        0,
        13,
        9,
        1,
        15,
        14,
        6,
        12,
        2,
        11
    };

    registers[0] = 0;
    registers[1] = 0;
    registers[2] = 0;
    registers[3] = 0;
    for (int instIndex = 0; instIndex < numInstructions; ++instIndex) {
        instruction *inst = &instructions[instIndex];
        doInstruction(registers, opcodeMapping[inst->opcode], inst->A, inst->B, inst->C);
    }
    printf("%d\n", registers[0]);

    return 0;
}
