#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

enum combat_team {
    COMBAT_TEAM_IMMUNE_SYSTEM,
    COMBAT_TEAM_INFECTION
};

enum attack_type {
    ATTACK_TYPE_FIRE,
    ATTACK_TYPE_COLD,
    ATTACK_TYPE_SLASHING,
    ATTACK_TYPE_BLUDGEONING,
    ATTACK_TYPE_RADIATION,
    ATTACK_TYPE_COUNT
};

struct combat_group {
    int id;
    combat_team team;
    int numUnits;
    int totalUnits;
    int hitPointsPerUnit;
    attack_type weaknesses[10];
    int numWeaknesses;
    attack_type immunities[10];
    int numImmunities;
    int attackDamage;
    attack_type attackType;
    int initiative;

    bool dead;
    bool targeted;
    int targetID;
};

combat_group *globalCombatGroups;
int boost;

int effectivePowerSorter (const void *a, const void *b) {
    int *aIndex = (int *)a;
    int *bIndex = (int *)b;
    combat_group *groupA = &globalCombatGroups[*aIndex];
    combat_group *groupB = &globalCombatGroups[*bIndex];

    int aAttackDamage = groupA->attackDamage;
    if (groupA->team == COMBAT_TEAM_IMMUNE_SYSTEM) {
        aAttackDamage += boost;
    }
    int bAttackDamage = groupB->attackDamage;
    if (groupB->team == COMBAT_TEAM_IMMUNE_SYSTEM) {
        bAttackDamage += boost;
    }
    int aEffectivePower = groupA->numUnits * aAttackDamage;
    int bEffectivePower = groupB->numUnits * bAttackDamage;

    if (aEffectivePower > bEffectivePower) { return -1; }
    if (aEffectivePower < bEffectivePower) { return 1; }
    if (groupA->initiative > groupB->initiative) { return -1; }
    if (groupA->initiative < groupB->initiative) { return 1; }
    return 0;
}

int initiativeSorter (const void *a, const void *b) {
    int *aIndex = (int *)a;
    int *bIndex = (int *)b;
    combat_group *groupA = &globalCombatGroups[*aIndex];
    combat_group *groupB = &globalCombatGroups[*bIndex];

    if (groupA->initiative > groupB->initiative) { return -1; }
    if (groupA->initiative < groupB->initiative) { return 1; }
    return 0;
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

char *readUntilAnyOfCharacters (char *currentLetter, char *currentWord, char character1, char character2, char character3) {
    int letterIndex = 0;
    while (*currentLetter != character1 && *currentLetter != character2 && *currentLetter != character3 &&
           *currentLetter != '\n' && currentLetter != 0) 
    {
        currentWord[letterIndex] = *currentLetter;
        letterIndex++;
        currentLetter++;
    }
    currentWord[letterIndex] = 0;
    return currentLetter;
}

bool stringsAreEqual (char *a, char *b) {
    int aIndex = 0;
    int bIndex = 0;

    while (a[aIndex] && b[bIndex]) {
        if (a[aIndex] != b[bIndex]) { return false; }
        ++aIndex;
        ++bIndex;
    }
    return a[aIndex] == b[bIndex];
}

int main (int argc, char **argv) {
    combat_group combatGroups[40];
    int numCombatGroups = 0;
    globalCombatGroups = combatGroups;

    const int stringLength = 200;
    char line[stringLength];

    char word[stringLength] = {};

    char *attackTypeNames[] = {
        "fire",
        "cold",
        "slashing",
        "bludgeoning",
        "radiation"
    };

    // skip header
    fgets(line, stringLength, stdin);
    bool loadingInfectionGroups = false;
    while (fgets(line, stringLength, stdin)) {
        char *currentLetter = line;

        if (*currentLetter == '\n') {
            loadingInfectionGroups = true;
            fgets(line, stringLength, stdin);
            continue; 
        }

        combat_group *group;
        group = &combatGroups[numCombatGroups];
        *group = {};
        group->id = numCombatGroups;
        ++numCombatGroups;
        if (!loadingInfectionGroups) {
            group->team = COMBAT_TEAM_IMMUNE_SYSTEM;
        }
        else {
            group->team = COMBAT_TEAM_INFECTION;
        }

        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        group->numUnits = atoi(word);
        group->totalUnits = group->numUnits;

        currentLetter += 17;
        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        group->hitPointsPerUnit = atoi(word);

        currentLetter += 12;
        bool parsingWeaknesses = false;
        if (*currentLetter == '(') {
            ++currentLetter;
            if (*currentLetter == 'w') { // weakness
                parsingWeaknesses = true;
                currentLetter += 8;
            }
            else { // immunity
                parsingWeaknesses = false;
                currentLetter += 10;
            }
            while (true) {
                currentLetter = readUntilAnyOfCharacters(currentLetter, word, ',', ';', ')');
                int attackType = -1;
                for (int i = 0; i < ATTACK_TYPE_COUNT; ++i) {
                    if (stringsAreEqual(attackTypeNames[i], word)) {
                        attackType = i;
                        break;
                    }
                }
                ASSERT(attackType != -1);
                if (parsingWeaknesses) {
                    group->weaknesses[group->numWeaknesses] = (attack_type)attackType;
                    ++(group->numWeaknesses);
                }
                else {
                    group->immunities[group->numImmunities] = (attack_type)attackType;
                    ++(group->numImmunities);
                }
                if (*currentLetter == ')') { 
                    currentLetter += 2;
                    break; 
                }
                else if (*currentLetter == ',') {
                    currentLetter += 2;
                }
                else if (*currentLetter == ';') {
                    if (parsingWeaknesses) {
                        currentLetter += 12;
                    }
                    else {
                        currentLetter += 10;
                    }
                    parsingWeaknesses = !parsingWeaknesses; // switch to immunities/weaknesses
                }
            }

        }
        currentLetter += 25;
        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        group->attackDamage = atoi(word);

        ++currentLetter;
        currentLetter = readUntilCharacter(currentLetter, word, ' ');
        int attackType = -1;
        for (int i = 0; i < ATTACK_TYPE_COUNT; ++i) {
            if (stringsAreEqual(attackTypeNames[i], word)) {
                attackType = i;
                break;
            }
        }
        ASSERT(attackType != -1);
        group->attackType = (attack_type)attackType;

        currentLetter += 22;
        currentLetter = readUntilCharacter(currentLetter, word, '\n');
        group->initiative = atoi(word);
    }

    int *groupsByEffectivePower = (int *)malloc(numCombatGroups * sizeof(combat_group));
    int *groupsByInitiative = (int *)malloc(numCombatGroups * sizeof(combat_group));
    for (int i = 0; i < numCombatGroups; ++i) {
        groupsByEffectivePower[i] = combatGroups[i].id;
        groupsByInitiative[i] = combatGroups[i].id;
    }

    boost = 0;
beginCombat:
    printf("current boost: %d\n", boost);
    int result = 0;
    bool immuneSystemWins = false;
    while (true) {

        for (int i = 0; i < numCombatGroups; ++i) {
            combat_group *group = &combatGroups[i];
            group->targeted = false;
            group->targetID = -1;
        }

        bool immuneSystemAllDead = true;
        for (int i = 0; i < numCombatGroups; ++i) {
            combat_group *group = &combatGroups[i];
            if (group->team == COMBAT_TEAM_IMMUNE_SYSTEM && !group->dead) {
                immuneSystemAllDead = false;
                break;
            }
        }
        if (immuneSystemAllDead) {
            for (int i = 0; i < numCombatGroups; ++i) {
                combat_group *group = &combatGroups[i];
                if (group->team == COMBAT_TEAM_INFECTION && !group->dead) {
                    result += group->numUnits;
                }
            }
            break;
        }

        bool infectionAllDead = true;
        for (int i = 0; i < numCombatGroups; ++i) {
            combat_group *group = &combatGroups[i];
            if (group->team == COMBAT_TEAM_INFECTION && !group->dead) {
                infectionAllDead = false;
                break;
            }
        }
        if (infectionAllDead) {
            for (int i = 0; i < numCombatGroups; ++i) {
                combat_group *group = &combatGroups[i];
                if (group->team == COMBAT_TEAM_IMMUNE_SYSTEM && !group->dead) {
                    result += group->numUnits;
                }
            }
            immuneSystemWins = true;
            break;
        }

        qsort(groupsByEffectivePower, numCombatGroups, sizeof(int), effectivePowerSorter);

        for (int i = 0; i < numCombatGroups; ++i) {
            combat_group *group = &combatGroups[groupsByEffectivePower[i]];
            if (group->dead) { continue; }

            int mostPossibleDamage = 0;
            int mostPossibleDamageIndex = -1;
            for (int j = 0; j < numCombatGroups; ++j) {
                if (i == j) { continue; }

                combat_group *target = &combatGroups[groupsByEffectivePower[j]];
                if (group->team == target->team) { continue; }
                if (target->dead || target->targeted) { continue; }

                int attackDamage = group->attackDamage;
                if (group->team == COMBAT_TEAM_IMMUNE_SYSTEM) {
                    attackDamage += boost;
                }
                int possibleDamage = group->numUnits * attackDamage;
                attack_type attackerAttackType = group->attackType;

                for (int weaknessIndex = 0; weaknessIndex < target->numWeaknesses; ++weaknessIndex) {
                    attack_type weakness = target->weaknesses[weaknessIndex];
                    if (attackerAttackType == weakness) {
                        possibleDamage *= 2;
                        break;
                    }
                }

                for (int immunityIndex = 0; immunityIndex < target->numImmunities; ++immunityIndex) {
                    attack_type immunity = target->immunities[immunityIndex];
                    if (attackerAttackType == immunity) {
                        possibleDamage = 0;
                        break;
                    }
                }

                if (possibleDamage > mostPossibleDamage) {
                    mostPossibleDamage = possibleDamage;
                    mostPossibleDamageIndex = target->id;
                }
            }

            if (mostPossibleDamageIndex != -1) {
                group->targetID = mostPossibleDamageIndex;
                combat_group *target = &combatGroups[mostPossibleDamageIndex];
                target->targeted = true;
            }
        }

        qsort(groupsByInitiative, numCombatGroups, sizeof(int), initiativeSorter);


        int totalUnitsKilled = 0;
        for (int i = 0; i < numCombatGroups; ++i) {
            combat_group *group = &combatGroups[groupsByInitiative[i]];
            if (group->dead) { continue; }
            if (group->targetID == -1) { continue; }

            combat_group *target = &combatGroups[group->targetID];

            int attackDamage = group->attackDamage;
            if (group->team == COMBAT_TEAM_IMMUNE_SYSTEM) {
                attackDamage += boost;
            }
            int damage = group->numUnits * attackDamage;
            attack_type attackerAttackType = group->attackType;

            for (int weaknessIndex = 0; weaknessIndex < target->numWeaknesses; ++weaknessIndex) {
                attack_type weakness = target->weaknesses[weaknessIndex];
                if (attackerAttackType == weakness) {
                    damage *= 2;
                    break;
                }
            }

            for (int immunityIndex = 0; immunityIndex < target->numImmunities; ++immunityIndex) {
                attack_type immunity = target->immunities[immunityIndex];
                if (attackerAttackType == immunity) {
                    damage = 0;
                    break;
                }
            }

            int unitsLost = damage / target->hitPointsPerUnit;
            target->numUnits -= unitsLost;
            totalUnitsKilled += unitsLost;
            if (target->numUnits <= 0) {
                target->dead = true;
            }

        }
        if (totalUnitsKilled == 0) {
            break;

        }
    }

    if (!immuneSystemWins) {
        for (int i = 0; i < numCombatGroups; ++i) {
            combat_group *group = &combatGroups[i];
            group->dead = false;
            group->numUnits = group->totalUnits;
        }
        ++boost;

        goto beginCombat;
    }

    printf("%d\n", result);
    printf("%d\n", boost);

    return 0;
}
