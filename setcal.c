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

#define SET_FUNCTIONS_LASTINDEX 9
#define REL_FUNCTIONS_LASTINDEX 19
#define COMBINED_FUNCTIONS_LASTINDEX 22

char functions[][14] = {"empty",
                        "card",
                        "complement",
                        "union",
                        "intersect",
                        "minus",
                        "subseteq",
                        "subset",
                        "equals",
                        "reflexive",
                        "symmetric",
                        "antisymmetric",
                        "transitive",
                        "function",
                        "domain",
                        "codomain",
                        "closure_ref",
                        "closure_sym",
                        "closure_trans",
                        "injective",
                        "surjective",
                        "bijective"};

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

// big brain time
void *bigBrainRealloc(void *ptr, size_t size)
{
    if (size <= 0)
    {
        free(ptr);
        return NULL;
    }

    void *tmp = realloc(ptr, size);
    if (tmp == NULL)
    {
        free(ptr);
        return NULL;
    }

    else
        return tmp;
}

bool empty(set_t *A)
{

    if (!A->set_len)
    {
        printf("true\n");
        return true;
    }
    printf("false\n");

    return false;
}

void card(set_t *A)
{
    printf("%d\n", A->set_len);
}

int complement(universe_t *universe, set_t *A)
{
    int *wholeSet = NULL;
    if (universe->universe_len)
        wholeSet = bigBrainRealloc(wholeSet, universe->universe_len * sizeof(int));
    else if (!universe->universe_len)
        wholeSet = bigBrainRealloc(wholeSet, sizeof(int));
    if (wholeSet == NULL)
        return errMsg("Allocation failed\n", false);

    printf("S");
    for (int o = 0; o < universe->universe_len; o++)
    {
        wholeSet[o] = o;
    }
    for (int o = 0; o < A->set_len; o++)
    {
        wholeSet[A->items[o]] = -1;
    }
    for (int o = 0; o < universe->universe_len; o++)
    {
        if (wholeSet[o] != -1)
            printf(" %s", universe->items[o]);
    }
    printf("\n");
    free(wholeSet);
    return true;
}

int Union(universe_t *universe, set_t *A, set_t *B)
{
    int len;
    set_t *bigger;
    set_t *smaller;
    if (A->set_len > B->set_len) bigger = A, smaller = B;
    else bigger = B, smaller = A;

    int *uni = NULL;
    if (bigger->set_len)
        uni = bigBrainRealloc(uni, bigger->set_len * sizeof(int));
    else if (!bigger->set_len)
        uni = bigBrainRealloc(uni, sizeof(int));
    if (uni == NULL)
        return errMsg("Allocation failed\n", false);

    len = bigger->set_len;

    memcpy(uni, bigger->items, bigger->set_len * sizeof(int));

    int j;

    for (int i = 0; i < bigger->set_len; i++)
    {
        for (j = 0; j < smaller->set_len; j++)
        {
            if (smaller->items[j] == bigger->items[i])
            {
                j = 0;
                break;
            }
        }
        if (j != 0)
        {
            int o = 0;
            for (; o < len; o++)
            {
                if (uni[o] == smaller->items[j-1])
                {
                    o = -1;
                    break;
                }
            }
            if (o == -1) break;
            len++;
            uni = bigBrainRealloc(uni, len * sizeof(int));
            if (uni == NULL)
                return errMsg("Allocation failed\n", false);

            memcpy(&uni[len - 1], &smaller->items[j-1], sizeof(int));
        }
    }

    printf("S");
    for (int i = 0; i < len; i++)
    {
        printf(" %s", universe->items[uni[i]]);
    }
    printf("\n");
    free(uni);
    return true;
}

void intersect(universe_t *universe, set_t *A, set_t *B)
{

    printf("S");
    for (int i = 0; i < A->set_len; i++)
    {
        for (int j = 0; j < B->set_len; j++)
        {
            if (A->items[i] == B->items[j])
            {
                printf(" %s", universe->items[A->items[i]]);
                break;
            }
        }
    }
    printf("\n");
}

int minus(universe_t *universe, set_t *A, set_t *B)
{
    int *min = NULL;
    if (A->set_len)
        min = bigBrainRealloc(min, A->set_len * sizeof(int));
    else if (!A->set_len)
        min = bigBrainRealloc(min, sizeof(int));
    if (min == NULL)
        return errMsg("Allocation failed\n", false);

    memcpy(min, A->items, A->set_len * sizeof(int));

    printf("S");
    for (int i = 0; i < A->set_len; i++)
    {
        for (int j = 0; j < B->set_len; j++)
        {
            if (A->items[i] == B->items[j])
            {
                min[i] = EMPTY_INDEX;
                break;
            }
        }
        if (min[i] != EMPTY_INDEX)
            printf(" %s", universe->items[min[i]]);
    }
    printf("\n");
    free(min);
    return true;
}

bool subseteq(set_t *A, set_t *B, bool print)
{
    int count = 0;
    for (int i = 0; i < A->set_len; i++)
    {
        for (int j = 0; j < B->set_len; j++)
        {
            if (A->items[i] == B->items[j])
            {
                count++;
            }
        }
    }

    if(!print && count == A->set_len)
    {
        return true;
    }
    else if (print && count == A->set_len)
    {
        printf("true\n");
        return true;
    }
    else if (print && count != A->set_len)
    {
        printf("false\n");
        return false;
    }
    else
    {
        return false;
    }
}

bool subset(set_t *A, set_t *B) {
    int control = subseteq(A, B, false);
    if (control == 1 && A->set_len < B->set_len)
    {
        printf("true\n");
        return true;
    }
    else
    {
        printf("false\n");
        return false;
    }
}

bool equals(set_t *A, set_t *B)
{
    if (!A->set_len && !B->set_len)
    {
        printf("true\n");
        return true;
    }
    else if (A->set_len != B->set_len)
    {
        printf("false\n");
        return false;
    }

    for (int i = 0; i < A->set_len; i++)
    {
        bool status = false;
        for (int j = 0; j < A->set_len; j++)
        {
            if (A->items[i] == B->items[j])
            {
                status = true;
                break;
            }
        }
        if (!status)
        {
            printf("false\n");
            return false;
        }
    }
    printf("true\n");
    return true;
}

bool reflexive(universe_t *uni, relation_t *R)
{
    int reflexiveUnitsCount = 0;
    for (int i = 0; i < R->relation_len; i++)
    {
        if (R->items[i].x == R->items[i].y)
        {
            reflexiveUnitsCount++;
        }
    }
    if (reflexiveUnitsCount == uni->universe_len)
    {
        printf("true\n");
        return true;
    }
    printf("false\n");
    return false;
}

bool symmetric(relation_t *R)
{
    for (int i = 0; i < R->relation_len; i++)
    {
        bool hasSymmetricUnit = false;
        for (int j = 0; j < R->relation_len; j++)
        {
            if (R->items[i].x == R->items[j].y && R->items[j].x == R->items[i].y)
            {
                hasSymmetricUnit = true;
            }
        }
        if (!hasSymmetricUnit)
        {
            printf("false\n");
            return false;
        }
    }
    printf("true\n");
    return true;
}

bool antisymmetric(relation_t *R)
{
    for (int i = 0; i < R->relation_len; i++)
    {
        for (int j = 0; j < R->relation_len; j++)
        {
            if (R->items[i].x == R->items[j].y && R->items[j].x == R->items[i].y && R->items[i].x != R->items[i].y)
            {
                printf("false\n");
                return false;
            }
        }
    }
    printf("true\n");
    return true;
}

bool transitive(relation_t *R)
{

    for (int i = 0; i < R->relation_len; i++)
    {
        for (int j = 0; j < R->relation_len; j++)
        {
            if (R->items[i].y == R->items[j].x)
            {
                bool status = false;
                for (int k = 0; k < R->relation_len; k++)
                {
                    if (R->items[k].x == R->items[i].x && R->items[k].y == R->items[j].y)
                    {
                        status = true;
                        break;
                    }
                }
                if (!status)
                {
                    printf("false\n");
                    return false;
                }
            }
        }
    }

    /* for (int i = 0; i < R->relation_len; i++)
    {
        for (int j = 0; j < R->relation_len; j++)
        {
            if (R->items[i].y == R->items[j].x)
            {
                bool hasTransitiveUnit = false;
                for (int k = 0; k < R->relation_len; k++)
                {
                    if (R->items[i].x == R->items[k].x == 0 && R->items[j].y == R->items[k].y)
                    {
                        hasTransitiveUnit = true;
                        break;
                    }
                }
                if (!hasTransitiveUnit)
                {
                    printf("\nfalse");
                    return false;
                }
            }
        }
    } */
    printf("true\n");
    return true;
}

bool function(relation_t *R)
{
    for (int i = 0; i < R->relation_len; i++)
    {
        for (int j = 0; j < R->relation_len; j++)
        {
            if ((R->items[i].x == R->items[j].x) && (R->items[i].y != R->items[j].y))
            {
                printf("false\n");
                return false;
            }
        }
    }

    printf("true\n");
    return true;
}

bool domain(universe_t *uni, relation_t *R)
{
    int *domain = malloc(R->relation_len * sizeof(int));
    int domainCount = 0;
    if (domain == NULL)
        return false;

    printf("S");
    for (int i = 0; i < R->relation_len; i++)
    {
        bool status = false;
        for (int j = 0; j < domainCount; j++)
        {
            if (R->items[i].x == domain[j])
            {
                status = true;
                break;
            }
        }
        if (!status)
        {
            printf(" %s", uni->items[R->items[i].x]);
            domain[domainCount] = R->items[i].x;
            domainCount++;
        }
    }
    printf("\n");
    free(domain);
    return true;
}

bool codomain(universe_t *uni, relation_t *R)
{
    int *domain = malloc(R->relation_len * sizeof(int));
    int domainCount = 0;
    if (domain == NULL)
        return false;
    printf("S");
    for (int i = 0; i < R->relation_len; i++)
    {
        bool status = false;
        for (int j = 0; j < domainCount; j++)
        {
            if (R->items[i].y == domain[j])
            {
                status = true;
                break;
            }
        }
        if (!status)
        {
            printf(" %s", uni->items[R->items[i].y]);
            domain[domainCount] = R->items[i].y;
            domainCount++;
        }
    }
    printf("\n");
    free(domain);
    return true;
}

int readStringFromFile(FILE *file, char **string)
{
    char c;
    int strLen = 0;

    // allocate memory for the string
    *string = malloc((MAX_STR_LEN + 1) * sizeof(char));
    if (*string == NULL)
        return errMsg("Allocation failed.\n", false);

    *string[0] = '\0';

    // read the string, character by character
    while (fscanf(file, "%c", &c) != EOF)
    {
        // check if the maximum length wasn't reached
        if (strLen > MAX_STR_LEN)
        {
            return errMsg("Items cannot be more than 30 characters long.\n", false);
        }

            // reached the delimiting character or newline/EOF -> current string is complete
        else if ((c == DELIM && strLen != 0) || c == '\n')
        {
            // resize the string to it's true value
            *string = bigBrainRealloc(*string, (strLen + 1) * sizeof(char));

            if (*string == NULL)
                return errMsg("Reallocation failed\n", false);

            if (c == '\n')
                return END_OF_LINE;
            else
                return true;
        }

            // the start condition is met and all whitespace is gone -> start adding characters to the string
        else if (c != DELIM)
        {
            (*string)[strLen] = c;
            (*string)[++strLen] = '\0';
        }
    }
    return END_OF_LINE;
}

int testSpace(FILE *file)
{
    char c;
    fscanf(file, "%c", &c);
    if (c == '\n') return true;
    else if (c != DELIM) return errMsg("No space between identifier and it's definition.\n", false);
    else return DELIM;
}

int checkUniverse(char *str, universe_t *universe)
{
    for (int i = 0, n = strlen(str); i < n; i++)
    {
        if (!isalpha(str[i])) return false;
    }

    if (strcmp(str, "false") == 0 || strcmp(str, "true") == 0) return false;

    for (int i = 0; i < COMBINED_FUNCTIONS_LASTINDEX; i++)
    {
        if (strcmp(str, functions[i]) == 0) return false;
    }

    for (int i = 0; i < universe->universe_len - 1; i++)
    {
        if (strcmp(universe->items[i], str) == 0) return false;
    }

    return true;
}

int readUniverse(universe_t *universe, FILE *file)
{
    universe->universe_len = 0;
    universe->items = NULL;
    char *str;

    int c, status;
    if ((c = testSpace(file)) != DELIM) return c;

    do
    {
        // try reading a string from the specified file
        status = readStringFromFile(file, &str);

        // an error occured while reading the file
        if (!status)
        {
            free(str);
            return false;
        }

        // empty universe
        if (status == END_OF_LINE && strlen(str) == 0)
        {
            free(str);
            return EMPTY_INDEX;
        }

        universe->items = bigBrainRealloc(universe->items, ++universe->universe_len * sizeof(char *));

        // check for memory errors
        if (universe->items == NULL)
        {
            free(str);
            return errMsg("Reallocation failed.\n", false);
        }

        universe->items[universe->universe_len - 1] = str;

        if (!checkUniverse(universe->items[universe->universe_len - 1], universe))
        {
            return errMsg("Duplicity in universe\n", false);
        }
    } while (status != END_OF_LINE);

    // if the loop finishes, we successfully read the universe
    return true;
}

int appendUniverse(universe_t *universe, setList_t *sets, FILE *file)
{
    int status = readUniverse(universe, file);

    if (status)
    {
        sets->sets = bigBrainRealloc(sets->sets, ++sets->setList_len * sizeof(set_t));
        if (sets->sets == NULL)
            return false;

        if (status == EMPTY_INDEX)
        {
            sets->sets[sets->setList_len - 1].items = NULL;
            sets->sets[sets->setList_len - 1].set_len = 0;
            return true;
        }

        sets->sets[sets->setList_len - 1].items = malloc(universe->universe_len * sizeof(int));
        if (sets->sets[sets->setList_len - 1].items == NULL)
            return false;

        for (int i = 0; i < universe->universe_len; i++)
        {
            sets->sets[sets->setList_len - 1].items[i] = i;
        }
        sets->sets[sets->setList_len - 1].set_len = universe->universe_len;

        return true;
    }
    else
        return false;
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

int containsRelationUnit(relation_t *relation, relationUnit_t *unit)
{
    for (int i = 0; i < relation->relation_len; i++)
    {
        if (relation->items[i].x == unit->x && relation->items[i].y == unit->y)
        {
            return i;
        }
    }
    return INVALID_INDEX;
}

int readRelationUnit(relationUnit_t *unit, FILE *file, universe_t *universe)
{
    char *strX, *strY;

    int statusX = readStringFromFile(file, &strX);
    if (!statusX) return false;
    if (statusX == END_OF_LINE && strlen(strX) == 0)
    {
        free(strX);
        return EMPTY_INDEX;
    }

    int statusY = readStringFromFile(file, &strY);
    if (!statusY) return false;

    if (strX[0] != '(' || strY[strlen(strY) - 1] != ')')
    {
        free(strX); free(strY);
        return errMsg("Invalid relation.\n", false);
    }

    strY[strlen(strY) - 1] = '\0';

    // compare without opening and closing brace
    int idx = findUniverseIndex(&strX[1], universe);
    int idy = findUniverseIndex(strY, universe);

    free(strX);
    free(strY);

    if (idx == INVALID_INDEX || idy == INVALID_INDEX)
        return errMsg("The relation contains items that are not part of the universe.\n", false);

    unit->x = idx;
    unit->y = idy;

    return statusY;
}

int readRelation(relation_t *relation, FILE *file, universe_t *universe)
{
    relation->relation_len = 0;
    relation->items = NULL;

    int c, status;
    if ((c = testSpace(file)) != DELIM) return c;

    do
    {
        relation->items = bigBrainRealloc(relation->items, ++relation->relation_len * sizeof(relationUnit_t));
        if (relation->items == NULL)
        {
            return errMsg("Allocation failed.\n", false);
        }

        status = readRelationUnit(&relation->items[relation->relation_len - 1], file, universe);
        if (!status)
        {
            return false;
        }
        else if (status == EMPTY_INDEX)
        {
            relation->items = bigBrainRealloc(relation->items, --relation->relation_len * sizeof(relationUnit_t));
            if (relation->items == NULL && relation->relation_len != 0) // if the new size is 0, bigBrainRealloc is equivalent to free
            {
                return errMsg("Allocation failed.\n", false);
            }
            break;
        }

        if (containsRelationUnit(relation, &relation->items[relation->relation_len - 1]) != relation->relation_len - 1)
        {
            return errMsg("Duplicity in a relation\n", false);
        }
    } while (status != END_OF_LINE);

    // if the loop finishes, we sucessfully read the relation
    return true;
}

int appendRelation(relationList_t *relations, universe_t *universe, FILE *file)
{
    relations->relations = bigBrainRealloc(relations->relations, ++relations->relationList_len * sizeof(relation_t));

    // check for memory errors
    if (relations->relations == NULL)
    {
        return errMsg("Allocation failed\n", false);
    }

    if (readRelation(&relations->relations[relations->relationList_len - 1], file, universe))
    {
        return true;
    }
    else return false;
}

int readSetItem(int *idx, FILE *file, universe_t *universe)
{
    char *str;
    int status = readStringFromFile(file, &str);
    if (!status)
    {
        free(str);
        return false;
    }
    // empty set or trailing whitespace
    if (strlen(str) == 0 && status == END_OF_LINE)
    {
        free(str);
        return EMPTY_INDEX;
    }

    *idx = findUniverseIndex(str, universe);
    free(str);

    if (*idx == INVALID_INDEX)
    {
        return errMsg("The set contains items that are not part of the universe.\n", false);
    }
    return status;
}

int readSet(set_t *set, FILE *file, universe_t *universe)
{
    set->set_len = 0;
    set->items = NULL;

    int c, status;
    if ((c = testSpace(file)) != DELIM) return c;

    do
    {
        set->items = bigBrainRealloc(set->items, ++set->set_len * sizeof(int));
        if (set->items == NULL)
            return errMsg("Allocation failed.\n", false);

        status = readSetItem(&set->items[set->set_len - 1], file, universe);

        if (!status) return false;
        else if (status == EMPTY_INDEX)
        {
            set->items = bigBrainRealloc(set->items, --set->set_len * sizeof(int));

            if (set->items == NULL && set->set_len != 0)
                return errMsg("Allocation failed.\n", false);

            break;
        }

        for (int i = 0; i < set->set_len - 1; i++)
        {
            if (set->items[i] == set->items[set->set_len - 1])
                return errMsg("Duplicity in a set\n", false);
        }
    } while (status != END_OF_LINE);

    // if the loop finishes, we sucessfully read the set
    return true;
}

int appendSet(setList_t *sets, universe_t *universe, FILE *file)
{
    sets->sets = bigBrainRealloc(sets->sets, ++sets->setList_len * sizeof(set_t));

    // check for memory error
    if (sets->sets == NULL)
    {
        return errMsg("Allocation failed\n", false);
    }

    if (readSet(&sets->sets[sets->setList_len - 1], file, universe))
    {
        return true;
    }
    else
    {
        return false;
    }
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

// print universe contents
void printUniverse(universe_t *universe)
{
    printf("U");
    for (int i = 0; i < universe->universe_len; i++)
    {
        printf(" %s", universe->items[i]);
    }
    printf("\n");
}

// print set contents
void printSet(set_t *set, universe_t *universe)
{
    printf("S");
    for (int i = 0; i < set->set_len; i++)
    {
        printf(" %s", universe->items[set->items[i]]);
    }
    printf("\n");
}

// print relation contents
void printRelation(relation_t *relation, universe_t *universe)
{
    printf("R");
    for (int i = 0; i < relation->relation_len; i++)
    {
        printf(" (%s %s)", universe->items[relation->items[i].x], universe->items[relation->items[i].y]);
    }
    printf("\n");
}

// returns END_OF_LINE on \n, or false when trying to access negative indexes or the universe
// or contains other symbols than digits
// on success returns the index as a long int
long *readIndex(FILE *file, int count, int *numberOfIndices)
{
    char *idx;
    char *ptr;
    long *indexes = NULL, digit;
    int order = 0;
    int status;
    do
    {
        if (!(status = readStringFromFile(file, &idx)))
            return NULL;

        indexes = bigBrainRealloc(indexes, sizeof(long));
        if (indexes == NULL)
            return NULL;

        digit = strtol(idx, &ptr, 10);
        if (digit < 1 || *ptr != '\0')
        {
            errMsg("Command taking wrong index", false);
            return NULL;
        }
        else if (digit > count)
        {
            indexes[0] = 0;
            return indexes;
        }
        else
            indexes[order] = digit;

        order++;
    } while (status != END_OF_LINE);
    *numberOfIndices = order;
    return indexes;
}

int callSetFunction(universe_t *universe, setList_t *sets, char *command, long *indices, int numberOfIndices, int funcNumber)
{

    int status = -1;
    int indicesUsed = 1;
    set_t *set1 = NULL;
    set_t *set2 = NULL;
    if (numberOfIndices > 0 && indices[0] < sets->setList_len)
    {
        set1 = &sets->sets[indices[0]];
    }
    else
    {
        return -1;
    }

    switch (funcNumber)
    {
        case 0:
            status = empty(set1);
            break;
        case 1:
            card(set1);
            break;
        case 2:
            complement(universe, set1);
            break;

        default:
            if (numberOfIndices > 1 && indices[1] < sets->setList_len)
            {
                set2 = &sets->sets[indices[1]];
            }
            else
            {
                return -1;
            }
            indicesUsed = 2;
            switch (funcNumber)
            {
                case 3:
                    Union(universe, set1, set2);
                    break;
                case 4:
                    intersect(universe, set1, set2);
                    break;
                case 5:
                    minus(universe, set1, set2);
                    break;
                case 6:
                    /* status =  */ subseteq(set1, set2, true); // TODO upravit subsetq tak aby vracel bool
                    break;
                case 7:
                    status = subset(set1, set2);
                    break;
                case 8:
                    status = equals(set1, set2);
                    break;
                default:
                    return -1;
            }
    }

    if (!(numberOfIndices - indicesUsed))
        return 0;
    else if (status == -1)
    {
        return -1;
    }
    else if (status == 1 && numberOfIndices - indicesUsed == 1)
    {
        return 0;
    }
    else if (numberOfIndices - indicesUsed == 1)
    {
        return 1;
    }
    return -1;
}

int callRelFunction(universe_t *universe, relationList_t *relations, char *command, long *indices, int numberOfIndices, int funcNumber)
{
    int status = -1;
    int indicesUsed = 1;
    relation_t *rel = NULL;
    if (numberOfIndices > 2 || !numberOfIndices)
    {
        return -1;
    }
    else
    {
        rel = &relations->relations[indices[0]];
    }

    switch (funcNumber)
    {
        case 9:
            status = reflexive(universe, rel);
            break;
        case 10:
            status = symmetric(rel);
            break;
        case 11:
            status = antisymmetric(rel);
            break;
        case 12:
            status = transitive(rel);
            break;
        case 13:
            status = function(rel);
            break;
        case 14:
            if (!domain(universe, rel))
            {
                return -1;
            }
            break;
        case 15:
            if (!codomain(universe, rel))
            {
                return -1;
            }
            break;
            /* case 16:
            closure_ref(...);
            break;
        case 17:
            closure_sym(...);
            break;
        case 18:
            closure_trans(...);
            break; */

        default:
            break;
    }
    if (!(numberOfIndices - indicesUsed))
        return 0;
    else if (status == -1)
    {
        return -1;
    }
    else if (status == 1 && numberOfIndices - indicesUsed == 1)
    {
        return 0;
    }
    else if (numberOfIndices - indicesUsed == 1)
    {
        return 1;
    }
    return -1;
}

int pickAndCallFunction(universe_t *universe, setList_t *sets, relationList_t *relations, char *command, long *indices, int *indexType, int numberOfIndices, int funcNumber)
{
    int status = 0;
    if (funcNumber < SET_FUNCTIONS_LASTINDEX)
    {
        for (int i = 0; i < numberOfIndices; i++)
        {
            if (indexType[i] < 0)
            {
                return -1;
            }
        }

        status = callSetFunction(universe, sets, command, indices, numberOfIndices, funcNumber);
    }
    else if (funcNumber < REL_FUNCTIONS_LASTINDEX)
    {
        if (indexType[0] > 0)
        {

            return -1;
        }

        status = callRelFunction(universe, relations, command, indices, numberOfIndices, funcNumber);
    }
    /* else if (status = COMBINED_FUNCTIONS_LASTINDEX)
    {
        status = callCombinedFunction(...);
    } */
    return status;
}



int matchStringToFunc(char *command)
{
    for (int i = 0; i < 19; i++)
    {
        if (strcmp(command, functions[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int findByLineIndex(int index, setList_t *sets, relationList_t *relations, int *isSet)
{
    for (int i = 0; i < sets->setList_len; i++)
    {
        if (index == sets->sets[i].index)
        {
            *isSet = 1;
            return i;
        }
    }

    for (int i = 0; i < relations->relationList_len; i++)
    {
        if (index == relations->relations[i].index)
        {
            *isSet = -1;
            return i;
        }
    }

    *isSet = 0;
    return 0;
}

// FIXME if this DOESNT work let me know as soon as possible
int readCommands(universe_t *universe, relationList_t *relations, setList_t *sets, FILE *file, int count, bool shouldExecuteCommand)
{
    if (shouldExecuteCommand)
    {
        char *command;
        int status;
        status = readStringFromFile(file, &command);

        if (status == INVALID_INDEX)
            return false;
        int numberOfIndices = 0;
        long *lineIndices = readIndex(file, count, &numberOfIndices);
        if (!numberOfIndices)
            return false;
        long indices[3];
        int indexType[3];
        for (int i = 0; i < numberOfIndices; i++)
        {

            indices[i] = findByLineIndex(lineIndices[i], sets, relations, &indexType[i]);
            if (!indexType[i])
            {
                free(command);
                free(lineIndices);
                return false;
            }
        }
        int funcNumber = matchStringToFunc(command);
        if (funcNumber == -1)
        {
            free(command);
            free(lineIndices);
            return false;
        }

        if (shouldExecuteCommand)
        {
            status = pickAndCallFunction(universe, sets, relations, command, indices, indexType, numberOfIndices, funcNumber);
            if (status == -1)
            {
                return false;
            }
            /* else if(status == 1)
        {
            int line = numberOfIndices - status;
            jumpToLine(line);
        } */
        }

        free(command);
        free(lineIndices);
    }
    /* else
    {
        saveLine();
    } */

    return true;
}


//TODO
int transitiveClosure(relation_t *relation, universe_t *universe)
{
    relation_t tmp = {.items = NULL, .relation_len = relation->relation_len};
    tmp.items = malloc(sizeof(relationUnit_t) * relation->relation_len);
    if (tmp.items == NULL)
        return errMsg("Allocation failed.\n", false);

    memcpy(tmp.items, relation->items, sizeof(relationUnit_t) * relation->relation_len);
    for (int i = 0; i < tmp.relation_len; i++)
    {
        for (int j = 0; j < tmp.relation_len; j++)
        {
            if (tmp.items[i].y == tmp.items[j].x)
            {
                relationUnit_t unit = {.x = tmp.items[i].x, .y = tmp.items[j].y};
                if (containsRelationUnit(&tmp, &unit) < 0)
                {
                    tmp.items = bigBrainRealloc(tmp.items, ++tmp.relation_len * sizeof(relationUnit_t));
                    if (tmp.items == NULL)
                    {
                        return errMsg("Allocation failed.\n", false);
                    }
                    tmp.items[tmp.relation_len - 1].x = unit.x;
                    tmp.items[tmp.relation_len - 1].y = unit.y;
                }
            }
        }
    }
    printRelation(&tmp, universe);
    free(tmp.items);
    return true;
}

// free all dynamic memory
void destructor(universe_t *universe, relationList_t *relations, setList_t *sets, FILE *file)
{
    freeUniverse(universe);
    freeRelations(relations);
    freeSets(sets);
    fclose(file);
}

int readFile(FILE *file)
{
    char c;
    int count = 0, hasU = 0, hasRorS = 0, hasC = 0;
    universe_t universe;
    relationList_t relations = {.relationList_len = 0, .relations = NULL};
    setList_t sets = {.setList_len = 0, .sets = NULL};

    while (fscanf(file, "%c", &c) != EOF)
    {
        if (++count > MAX_NUM_LINES) return errMsg("The file cannot be more than 1000 lines long.\n", EXIT_FAILURE);

        switch (c)
        {
            case 'U':
            {
                if (appendUniverse(&universe, &sets, file))
                {
                    if (++hasU > 1) return errMsg("Invalid file structure.\n", EXIT_FAILURE);

                    sets.sets[sets.setList_len - 1].index = count;
                    printUniverse(&universe);
                }
                else
                {
                    destructor(&universe, &relations, &sets, file);
                    return EXIT_FAILURE;
                }

                break;
            }
            case 'S':
            {
                if (!hasU || hasC) return errMsg("Invalid file structure.\n", EXIT_FAILURE);

                if (appendSet(&sets, &universe, file))
                {
                    hasRorS = 1;
                    sets.sets[sets.setList_len - 1].index = count;
                    printSet(&sets.sets[sets.setList_len - 1], &universe);
                }
                else
                {
                    destructor(&universe, &relations, &sets, file);
                    return EXIT_FAILURE;
                }

                break;
            }
            case 'R':
            {
                if (!hasU || hasC) return errMsg("Invalid file structure.\n", EXIT_FAILURE);

                if (appendRelation(&relations, &universe, file))
                {
                    hasRorS = 1;
                    relations.relations[relations.relationList_len - 1].index = count;
                    printRelation(&relations.relations[relations.relationList_len - 1], &universe);
                }
                else
                {
                    destructor(&universe, &relations, &sets, file);
                    return EXIT_FAILURE;
                }

                break;
            }
            case 'C':
            {
                hasC = 1;
                //I suppose prints will happen in each function
                if (!hasU || !hasRorS) return errMsg("Invalid file structure.\n", EXIT_FAILURE);

                if (!readCommands(&universe, &relations, &sets, file, count, true))
                {
                    destructor(&universe, &relations, &sets, file);
                    return errMsg("In readCommand.\n", EXIT_FAILURE);
                }
                break;
            }

            default:
            {
                destructor(&universe, &relations, &sets, file);
                return errMsg("Invalid file structure.\n", EXIT_FAILURE);
            }
        }
    }
    if (!hasC) return errMsg("Invalid file structure.\n", EXIT_FAILURE);
    destructor(&universe, &relations, &sets, file);
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