#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h> // for seeding random

#define DELIM ' '
#define MAX_STR_LEN 30
#define MAX_NUM_LINES 1000
#define END_OF_LINE -1
#define INVALID_INDEX -2
#define EMPTY_INDEX -3

// STRUCTURES

// struct to store universe contents
typedef struct
{
    char **items;
    int universe_len;
} universe_t;

// struct to store a single set
typedef struct
{
    int *items;
    int set_len;
    long index;
} set_t;

// struct to store all sets in the file
typedef struct
{
    set_t *sets;
    int setList_len;
} setList_t;

// indices of the related strings from universe
typedef struct
{
    int x;
    int y;
} relationUnit_t;

//struct to store a relationship set
typedef struct
{
    relationUnit_t *items;
    int relation_len;
    long index;
} relation_t;

// struct to store all relationships in a file
typedef struct
{
    relation_t *relations;
    int relationList_len;
} relationList_t;

// function for printing error messages to stderr
int errMsg(char *msg, int status)
{
    fprintf(stderr, "%s", msg);
    return status;
}

int readStringFromFile(FILE *file, char **string, char delimStart, char delimStop)
{
    char c;
    int strLen = 0;

    // allocate memory for the string
    *string = malloc((MAX_STR_LEN + 1) * sizeof(char));
    if (*string == NULL)
        return errMsg("Allocation failed\n", false);

    *string[0] = '\0';

    // read the string, character by character
    while (fscanf(file, "%c", &c) != EOF)
    {
        // check if the maximum length wasn't reached
        if (strLen > MAX_STR_LEN)
        {
            return errMsg("Items cannot be more than 30 characters long\n", false);
        }

        // reached the delimiting character or newline/EOF -> current string is complete
        else if ((c == delimStop && strLen != 0) || c == '\n')
        {
            // resize the string to it's true value
            *string = realloc(*string, (strLen + 1) * sizeof(char));

            if (*string == NULL)
                return errMsg("Reallocation failed\n", false);

            if (c == '\n')
                return END_OF_LINE;
            else
                return true;
        }

        // the start condition is met and all whitespace is gone -> start adding characters to the string
        else if (c != delimStart && c != DELIM)
        {
            (*string)[strLen] = c;
            (*string)[++strLen] = '\0';
        }
    }
    return END_OF_LINE;
}

int checkUniverse(char *str, universe_t *universe)
{
    for (int i = 0; i < universe->universe_len - 1; i++)
    {
        if (strcmp(universe->items[i], str) == 0)
        {
            return false;
        }
    }
    return true;
}

int readUniverse(universe_t *universe, FILE *file)
{
    universe->universe_len = 0;
    int status;
    char *str;
    do
    {
        // try reading a string from the specified file
        status = readStringFromFile(file, &str, DELIM, DELIM);

        // an error occured while reading the file
        if (!status)
            return false;

        else
        {
            if (universe->universe_len == 0)
            {
                universe->items = malloc(++universe->universe_len * sizeof(char *));
            }
            else
            {
                universe->items = realloc(universe->items, ++universe->universe_len * sizeof(char *));
            }

            // check for memory errors
            if (universe->items == NULL)
            {
                free(str);
                return errMsg("Reallocation failed.\n", false);
            }

            if (!checkUniverse(str, universe))
            {
                free(str);
                return errMsg("Duplicity in universe\n", false);
            }
            universe->items[universe->universe_len - 1] = str;
        }

    } while (status != END_OF_LINE);

    // if the loop finishes, we successfully read the universe
    return true;
}

// find the index of the specified string from the universe
int findUniverseIndex(char *str, universe_t *universe)
{
    for (int i = 0; i < universe->universe_len; i++)
    {
        if (strcmp(str, universe->items[i]) == 0)
        {
            return i;
        }
    }

    // the item wasn't found in the universe, so it's an invalid item
    return INVALID_INDEX;
}

int readIndexedItem(int *idx, FILE *file, universe_t *universe, char delimStart, char delimStop)
{
    char *str;
    int status = readStringFromFile(file, &str, delimStart, delimStop);

    if (!status)
    {
        free(str);
        return INVALID_INDEX; // an error occurred while reading the string
    }

    else
    {
        // when using this function with relations, the newline character gets saved separately
        if (strlen(str) == 0)
        {
            free(str);
            return END_OF_LINE;
        }

        int i = findUniverseIndex(str, universe);
        free(str);

        if (i != INVALID_INDEX)
        {
            *idx = i;
            return status;
        }
        else
            return errMsg("The set/relation contains items that do not belong in the universe\n", INVALID_INDEX);
    }
}

int readSet(set_t *set, FILE *file, universe_t *universe, int index)
{
    set->set_len = 0;
    int idx, status;

    do
    {
        status = readIndexedItem(&idx, file, universe, DELIM, DELIM);

        if (status == INVALID_INDEX)
            return false;

        else
        {
            if (set->set_len == 0)
            {
                set->items = malloc(++set->set_len * sizeof(int));
            }
            else
            {
                set->items = realloc(set->items, ++set->set_len * sizeof(int));
            }

            if (set->items == NULL)
                return errMsg("Allocation failed\n", false);

            for (int i = 0; i < set->set_len - 1; i++)
            {
                if (set->items[i] == idx)
                    return errMsg("Duplicity in a set\n", false);
            }

            set->items[set->set_len - 1] = idx;
            set->index = index;
        }
    } while (status != END_OF_LINE);

    // if the loop finishes, we successfully read the set
    return true;
}

int readRelation(relation_t *relation, FILE *file, universe_t *universe, int index)
{
    relation->relation_len = 0;

    int statusX, statusY, idx, idy;
    do
    {
        statusX = readIndexedItem(&idx, file, universe, '(', DELIM);
        if (statusX == INVALID_INDEX)
            return false;
        else if (statusX == END_OF_LINE)
            break;

        statusY = readIndexedItem(&idy, file, universe, DELIM, ')');
        if (statusY == INVALID_INDEX)
            return false;
        else if (statusY == END_OF_LINE)
            break;

        else
        {
            if (relation->relation_len == 0)
            {
                relation->items = malloc(++relation->relation_len * sizeof(relationUnit_t));
            }
            else
            {
                relation->items = realloc(relation->items, ++relation->relation_len * sizeof(relationUnit_t));
            }

            if (relation->items == NULL)
            {
                return errMsg("Allocation failed\n", false);
            }

            /* for (int i = 0; i < relation->relation_len - 1; i++)
            {
                if (relation->items[i].x == idx || relation->items[i].y == idy)
                    return errMsg("Duplicity in a relation\n", false);
            } */

            relation->items[relation->relation_len - 1].x = idx;
            relation->items[relation->relation_len - 1].y = idy;
            relation->index = index;
        }
    } while (statusX != END_OF_LINE && statusY != END_OF_LINE);

    // if the loop finishes, we successfully read the relation
    return true;
}

// free the universe struct
void freeUniverse(universe_t *universe)
{
    for (int i = 0; i < universe->universe_len; i++)
    {
        free(universe->items[i]);
    }
    free(universe->items);
}

// free all malloced sets
void freeSets(setList_t *sets)
{
    if (sets->setList_len == 0)
        return; // the list is empty, nothing to free

    for (int i = 0; i < sets->setList_len; i++)
    {
        if (sets->sets[i].set_len != 0)
        {
            free(sets->sets[i].items);
        }
    }
    free(sets->sets);
}

// free all malloced relations
void freeRelations(relationList_t *relations)
{
    if (relations->relationList_len == 0)
        return; // the list is empty, nothing to free

    for (int i = 0; i < relations->relationList_len; i++)
    {
        if (relations->relations[i].relation_len != 0)
        {
            free(relations->relations[i].items);
        }
    }
    free(relations->relations);
}

// TODO ? delete later - only for validity checking
void printUniverse(universe_t *universe)
{
    printf("U ");
    for (int i = 0; i < universe->universe_len; i++)
    {
        printf("%s ", universe->items[i]);
    }
    printf("\n");
}

// TODO ? delete later - only for validity checking
void printSet(set_t *set, universe_t *universe)
{
    printf("S ");
    for (int i = 0; i < set->set_len; i++)
    {
        printf("%s ", universe->items[set->items[i]]);
    }
    printf("\n");
}

// TODO ? delete later - only for validity checking
void printRelation(relation_t *relation, universe_t *universe)
{
    printf("R ");
    for (int i = 0; i < relation->relation_len; i++)
    {
        printf("(%s %s) ", universe->items[relation->items[i].x], universe->items[relation->items[i].y]);
    }
    printf("\n");
}

// returns END_OF_LINE on \n, or false when trying to access negative indexes or the universe
// or contains other symbols than digits
// on success returns the index as a long int
long readIndex(FILE *file, int count)
{
    char c;
    char *ptr;
    long indexes[3], digit;
    int order = 0;
    while (fscanf(file, "%c", &c) != EOF)
    {
        if (isdigit(c))
        {
            digit = strtol(&c, &ptr, 10);
            if (digit < 1 || *ptr != '\0')
                return errMsg("Command taking wrong index", false);
            else if (digit > count)
                return EMPTY_INDEX; // TODO bonus
            else
                indexes[order] = digit;
        }
        else if (c == '\n')
            indexes[order] = '\n';

        order++;
    }
    return *indexes;
}

// FIXME if this DOESNT work let me know as soon as possible
/* int readCommands(universe_t *universe, relationList_t *relations, setList_t *sets, FILE *file, int count)
{
char *command = malloc(sizeof(char));
char c;

if (command == NULL)
    return errMsg("Allocation failed\n", false);

command[0] = '\0';

int length = 0;
while (fscanf(file, "%c", &c))
{
    if (length != 0 && command[length] != DELIM)
    {
        command = realloc(command, (length + 1) * sizeof(char));
        if (command == NULL)
            return errMsg("Reallocation failed\n", false);

        command[length - 1] = c;
        command[length] = '\0';
    }
    else if (length != 0 && command[length] == DELIM)
    {
        break;
    }
    length++;
}

    *
    * selection of the wanted command
    * use char *command to decide which command is required
    * within these functions use the readIndex() to get indexes of the required lines
    * enter their data through setList->sets[index].items[i]
    * or relationsList->relations[index].items[i]
    *
    */
/* free(command);
return true;
} */

//TODO looping throught relation could be made in a separate function

int getUniqueRelMembers(relation_t rel, int members[], int param)
{
    bool shouldFree = false;
    if (members == NULL)
    {
        shouldFree = true;
        members = malloc(rel.relation_len * 2 * sizeof(int));
    }

    int memberCount = 0;
    if (param == 1 || param == 0)
    {
        for (int i = 0; i < rel.relation_len; i++)
        {
            bool isDuplicit = false;
            for (int j = 0; j < memberCount; j++)
            {
                if (rel.items[i].x == members[j])
                {
                    isDuplicit = true;
                    break;
                }
            }

            if (!isDuplicit)
            {
                members[memberCount] = rel.items[i].x;
                memberCount++;
            }
        }
    }
    if (param == 2 || param == 0)
    {
        for (int i = 0; i < rel.relation_len; i++)
        {

            bool isDuplicit = false;
            for (int j = 0; j < memberCount; j++)
            {
                if (rel.items[i].y == members[j])
                {
                    isDuplicit = true;
                    break;
                }
            }
            if (!isDuplicit)
            {
                members[memberCount] = rel.items[i].y;
                memberCount++;
            }
        }
    }

    if (shouldFree)
    {
        free(members);
    }

    return memberCount;
}

bool r_isReflexive(universe_t uni, relation_t rel)
{
    int reflexiveUnitsCount = 0;
    for (int i = 0; i < rel.relation_len; i++)
    {
        if (strcmp(uni.items[rel.items[i].x], uni.items[rel.items[i].y]) == 0)
        {
            reflexiveUnitsCount++;
        }
    }
    if (reflexiveUnitsCount == getUniqueRelMembers(rel, NULL, 0))
    {
        return true;
    }
    return false;
}

bool r_isSymmetric(universe_t uni, relation_t rel)
{
    for (int i = 0; i < rel.relation_len; i++)
    {
        bool hasSymmetricUnit = false;
        for (int j = 0; j < rel.relation_len; j++)
        {
            if (strcmp(uni.items[rel.items[i].x], uni.items[rel.items[j].y]) == 0 && strcmp(uni.items[rel.items[j].x], uni.items[rel.items[i].y]) == 0)
            {
                hasSymmetricUnit = true;
            }
        }
        if (!hasSymmetricUnit)
        {
            return false;
        }
    }
    return true;
}

bool r_isAntiSymmetric(universe_t uni, relation_t rel)
{
    for (int i = 0; i < rel.relation_len; i++)
    {

        for (int j = 0; j < rel.relation_len; j++)
        {
            if (strcmp(uni.items[rel.items[i].x], uni.items[rel.items[j].y]) == 0 && strcmp(uni.items[rel.items[j].x], uni.items[rel.items[i].y]) == 0)
            {
                return false;
            }
        }
    }
    return true;
}

bool r_isTransitive(universe_t uni, relation_t rel)
{
    for (int i = 0; i < rel.relation_len; i++)
    {
        for (int j = 0; j < rel.relation_len; j++)
        {
            if (strcmp(uni.items[rel.items[i].y], uni.items[rel.items[j].x]) == 0)
            {
                bool hasTransitiveUnit = false;
                for (int k = 0; k < rel.relation_len; k++)
                {
                    if (strcmp(uni.items[rel.items[i].x], uni.items[rel.items[k].x]) == 0 && strcmp(uni.items[rel.items[j].y], uni.items[rel.items[k].y]) == 0)
                    {
                        hasTransitiveUnit = true;
                        break;
                    }
                }
                if (!hasTransitiveUnit)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool r_isFunction(universe_t uni, relation_t rel)
{
    for (int i = 0; i < rel.relation_len; i++)
    {
        for (int j = i; j < rel.relation_len; j++)
        {
            if (strcmp(uni.items[rel.items[i].x], uni.items[rel.items[j].x]) == 0)
            {
                if (strcmp(uni.items[rel.items[i].y], uni.items[rel.items[j].y]) != 0)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void r_domain(universe_t uni, relation_t rel)
{
    int *members = malloc(rel.relation_len * 2 * sizeof(int));
    if (members == NULL)
        errMsg("Allocation failed", false);

    int len = getUniqueRelMembers(rel, members, 1);
    for (int i = 0; i < len; i++)
    {
        printf("%s ", uni.items[members[i]]);
    }
    free(members);
}

void r_codomain(universe_t uni, relation_t rel)
{
    int *members = malloc(rel.relation_len * 2 * sizeof(int));
    if (members == NULL)
        errMsg("Allocation failed", false);

    int len = getUniqueRelMembers(rel, members, 2);
    for (int i = 0; i < len; i++)
    {
        printf("%s ", uni.items[members[i]]);
    }
    free(members);
}

bool r_isInjective(universe_t uni, relation_t rel)
{
    for (int i = 0; i < rel.relation_len; i++)
    {
        for (int j = i; j < rel.relation_len; j++)
        {
            if (strcmp(uni.items[rel.items[i].y], uni.items[rel.items[j].y]) == 0)
            {
                if (strcmp(uni.items[rel.items[i].x], uni.items[rel.items[j].x]) != 0)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

int readFile(FILE *file)
{
    char c;
    int count = 0;
    universe_t universe;
    relationList_t relations;
    setList_t sets;

    relations.relationList_len = 0;
    sets.setList_len = 0;

    while (fscanf(file, "%c", &c) != EOF)
    {
        count++;
        switch (c)
        {
        case 'U':
        {
            if (readUniverse(&universe, file))
            {
                printUniverse(&universe);
            }
            else
            {
                return EXIT_FAILURE;
            }
            break;
        }
        case 'S':
        {
            if (sets.setList_len == 0)
            {
                sets.sets = malloc(++sets.setList_len * sizeof(set_t));
            }
            else
            {
                sets.sets = realloc(sets.sets, ++sets.setList_len * sizeof(set_t));
            }

            // check for memory error
            if (sets.sets == NULL)
            {
                return errMsg("Allocation failed\n", EXIT_FAILURE);
            }

            if (readSet(&sets.sets[sets.setList_len - 1], file, &universe, count))
            {
                printSet(&sets.sets[sets.setList_len - 1], &universe);
            }
            else
            {
                return EXIT_FAILURE;
            }

            break;
        }
        case 'R':
        {
            if (relations.relationList_len == 0)
            {
                relations.relations = malloc(++relations.relationList_len * sizeof(relation_t));
            }
            else
            {
                relations.relations = realloc(relations.relations, ++relations.relationList_len * sizeof(relation_t));
            }

            // check for memory errors
            if (relations.relations == NULL)
            {
                return errMsg("Allocation failed\n", EXIT_FAILURE);
            }

            if (readRelation(&relations.relations[relations.relationList_len - 1], file, &universe, count))
            {
                printRelation(&relations.relations[relations.relationList_len - 1], &universe);
            }
            else
            {
                return EXIT_FAILURE;
            }
            break;
        }
        case 'C':
        {

            printf("%d", r_isReflexive(universe, relations.relations[0]));
            printf("%d", r_isSymmetric(universe, relations.relations[0]));
            printf("%d", r_isAntiSymmetric(universe, relations.relations[0]));
            printf("%d", r_isTransitive(universe, relations.relations[0]));
            printf("%d", r_isFunction(universe, relations.relations[0]));
            r_domain(universe, relations.relations[0]);
            r_codomain(universe, relations.relations[0]);
            printf("%d", r_isInjective(universe, relations.relations[0]));
        }

        default:
        {
            freeUniverse(&universe);
            freeRelations(&relations);
            freeSets(&sets);
            fclose(file);
            return 0;
        }
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    FILE *f;

    if (argc == 2)
    {
        if ((f = fopen(argv[1], "r")) != NULL)
        {
            return readFile(f);
        }
        else
        {
            fprintf(stderr, "Error opening file %s.\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        fprintf(stderr, "Invalid arguments supplied.\n");
        return EXIT_FAILURE;
    }
}