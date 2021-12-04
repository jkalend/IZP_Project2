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

/// struct to store universe contents
typedef struct
{
    char **items; ///< array of strings storing contents of a universe
    int universe_len; ///< number of elements in a universe
} universe_t;

/// struct to store a single set
typedef struct
{
    int *items; ///< array of indices referring to a universe
    int set_len; ///< length of a set
    long index; ///< index of a line to which a set belongs
} set_t;

/// struct to store all sets in the file
typedef struct
{
    set_t *sets; ///< array of all sets
    int setList_len; ///< count of all sets
} setList_t;

/// indices of the related strings from universe
typedef struct
{
    int x; ///< first element in a binary relation
    int y; ///< second element in a binary relation
} relationUnit_t;

///struct to store a relationship set
typedef struct
{
    relationUnit_t *items; ///< array of binary relations
    int relation_len; ///< count of binary relations
    long index; ///< index of a line to which a relation belongs
} relation_t;

/// struct to store all relationships in a file
typedef struct
{
    relation_t *relations; ///< array of all relations
    int relationList_len; ///< count of all relations
} relationList_t;

typedef struct
{
    int functionNameIdx;
    int *parameters; ///< array of parameters
    int argc;
    int idx;
    bool exec;
} command_t;

typedef struct
{
    command_t *commands; ///<array of commands>
    int commandList_len; ///< count of all commands
}commandList_t;


/// Prints error messages to stderr
/// \param msg string to be printed to stderr
/// \param status an integer to be returned by the function
/// \return a status variable
int errMsg(char *msg, int status)
{
    fprintf(stderr, "%s", msg);
    return status;
}

/// Safer and overall better realloc
/// \param ptr a dynamically allocated array
/// \param size indicates the size of the dynamic array after reallocation
/// \return NULL on failure, pointer to the new memory on success
void *bigBrainRealloc(void *ptr, size_t size)
{
    if (size <= 0)
    {
        if (ptr != NULL) free(ptr);
        return NULL;
    }

    void *tmp = realloc(ptr, size);
    if (tmp == NULL)
    {
        if (ptr != NULL) free(ptr);
        return NULL;
    }

    else
        return tmp;
}

int insertToSetList(set_t *set, setList_t *sets)
{
    sets->sets = bigBrainRealloc(sets->sets, ++sets->setList_len * sizeof(set_t));

    // check for memory error
    if (sets->sets == NULL)
    {
        return errMsg("Allocation failed\n", false);
    }

    sets->sets[sets->setList_len - 1].set_len = set->set_len;
    sets->sets[sets->setList_len - 1].items = set->items;
    sets->sets[sets->setList_len - 1].index = set->index;
    return true;
}

/// Decides whether the set is empty or not, and prints the result
/// \param A a set suspected of being empty
/// \return true when set is empty and false otherwise
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

/// Prints out the length of a set
/// \param A a set of which length is to be printed
void card(set_t *A)
{
    printf("%d\n", A->set_len);
}

/// Prints out a complement of a set to the universe
/// \param universe the universe over which the set is defined
/// \param A set of which we want the complement
/// \return false on failure of an allocation, true on success
int complement(universe_t *universe, setList_t *sets, set_t *A, int index)
{
    int *wholeSet = NULL;
    /*
    if (universe->universe_len)
        wholeSet = bigBrainRealloc(wholeSet, sizeof(int));
    else if (!universe->universe_len)
        wholeSet = bigBrainRealloc(wholeSet, sizeof(int));
    */

    printf("S ");
    int size = 0;
    for (int i = 0; i < universe->universe_len; i++) // U a b c //S a b
    {
        int o = 0;
        for (; o < A->set_len; o++)
        {
            if (A->items[o] == i) break;
        }
        if (o == A->set_len)
        {
            size++;
            wholeSet = bigBrainRealloc(wholeSet, size *sizeof(int));
            if (wholeSet == NULL)
                return errMsg("Allocation failed\n", false);
            wholeSet[size-1] = i;
        }
    }
    for (int o = 0; o < size; o++)
    {
        printf("%s ", universe->items[wholeSet[o]]);
    }
    printf("\n");
    set_t set = {.set_len = size, .items = NULL, .index = index};
    set.items = bigBrainRealloc(set.items, size*sizeof(int));
    if(set.items == NULL && size > 0) return errMsg("Allocation failed\n", false);

    memcpy(set.items, wholeSet, size*sizeof(int));
    insertToSetList(&set, sets);
    free(wholeSet);
    return true;
}

/// Prints out the union of two sets
/// \param universe the universe over which the sets are defined
/// \param A the first set to be put in the union
/// \param B the second set to be put in the union
/// \return false on failure of an allocation, true on success
int Union(universe_t *universe, set_t *A, set_t *B, setList_t *sets, int index)
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
            if (uni == NULL && len > 0)
                return errMsg("Allocation failed\n", false);

            memcpy(&uni[len - 1], &smaller->items[j-1], sizeof(int));
        }
    }

    printf("S");
    for (int i = 0; i < len; i++)
    {
        printf(" %s", universe->items[uni[i]]);
    }
    set_t set = {.set_len = len, .items = NULL, .index = index};
    set.items = bigBrainRealloc(set.items, len*sizeof(int)); //FIXME when size is 0 -> leak of 0 bytes
    if(set.items == NULL && len > 0) return errMsg("Allocation failed\n", false);

    memcpy(set.items, uni, len*sizeof(int));
    insertToSetList(&set, sets);
    printf("\n");
    free(uni);
    return true;
}

/// Prints out the intersection of two sets
/// \param universe the universe over which the sets are defined
/// \param A the first set in the intersection
/// \param B the second set in the intersection
int intersect(universe_t *universe, set_t *A, set_t *B, setList_t *sets, int index)
{
    int *array = NULL;
    int size = 0;

    printf("S");
    for (int i = 0; i < A->set_len; i++)
    {
        for (int j = 0; j < B->set_len; j++)
        {
            if (A->items[i] == B->items[j])
            {
                printf(" %s", universe->items[A->items[i]]);
                array = bigBrainRealloc(array, ++size*sizeof(int));
                if(array == NULL && size > 0) return errMsg("Allocation failed\n", false);

                array[size-1] = A->items[i];
                break;
            }
        }
    }
    set_t set = {.set_len = size, .items = NULL, .index = index};
    set.items = bigBrainRealloc(set.items, size*sizeof(int));
    if(set.items == NULL && size > 0) return errMsg("Allocation failed\n", false);

    memcpy(set.items, array, size*sizeof(int));
    insertToSetList(&set, sets);
    printf("\n");
    free(array);
    return true;
}

/// Prints out the result of a subtraction of a set B from set A
/// \param universe the universe over which the sets are defined
/// \param A the set from which elements are subtracted
/// \param B the set containing the subtracted elements
/// \return false on failure of an allocation, true on success
int minus(universe_t *universe, set_t *A, set_t *B, setList_t *sets, int index)
{
    int *min = NULL;
    int size = 0;

    printf("S");
    for (int i = 0; i < A->set_len; i++)
    {
        int status = 0;
        for (int j = 0; j < B->set_len; j++)
        {
            if (A->items[i] == B->items[j])
            {
                status = EMPTY_INDEX;
                break;
            }
        }
        if (status != EMPTY_INDEX)
        {
            printf(" %s", universe->items[A->items[i]]);
            min = bigBrainRealloc(min, ++size*sizeof(int));
            if (min == NULL && size > 0)
                return errMsg("Allocation failed\n", false);

            min[size-1] = A->items[i];
        }
    }
    set_t set = {.set_len = size, .items = NULL, .index = index};
    set.items = bigBrainRealloc(set.items,size*sizeof(int));
    if(set.items == NULL && size > 0) return errMsg("Allocation failed\n", false);

    memcpy(set.items, min, size*sizeof(int));
    insertToSetList(&set, sets);

    printf("\n");
    free(min);
    return true;
}

/// Prints true when the set A is a subset of the set B, and false otherwise. Or returns the same boolean value when print == true
/// \param A a set that is a suspected subset of the set B
/// \param B a set that is a suspected superset of the set A
/// \param print decides whether the result is to be printed or returned, true to be printed and false to be returned
/// \return true when the set A is a subset of the set B, false otherwise
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

/// Prints true when the set A is a proper subset of the set B, and false otherwise
/// \param A a set that is a suspected proper subset of the set B
/// \param B a set that is a suspected superset of the set A
/// \return true when the set A is a proper subset of the set B, false otherwise
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

/// Prints true when the sets are equal and false otherwise
/// \param A the first set of the comparison
/// \param B the second set of the comparison
/// \return true when the sets are equal, false otherwise
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

/// Function for reading strings seperated by whitespace from the specified file
/// \param file file to be read from
/// \param string container for the new string
/// \return false when an error occurs during reading
/// \return true when a string is successfully read
/// \return END_OF_LINE when a newline or EOF is reached at the end of the string
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
        if (c == '\n') return END_OF_LINE;
        else if (c == DELIM && strLen != 0) return true;

            // the start condition is met and all whitespace is gone -> start adding characters to the string
        else if (c != DELIM)
        {
            // check if the maximum length wasn't reached
            if (++strLen > MAX_STR_LEN)
            {
                free(*string);
                return errMsg("Items cannot be more than 30 characters long.\n", false);
            }

            (*string)[strLen - 1] = c;
            (*string)[strLen] = '\0';

        }
    }
    return END_OF_LINE;
}

/// Checks for space between U, R, S and it's contents
/// \param file file to be read from
/// \return DELIM when a space is present
/// \return true when a newline is present (special condition for empty sets, ...)
/// \return false when no space or newline is present
int testSpace(FILE *file)
{
    char c;
    fscanf(file, "%c", &c);
    if (c == '\n') return true;
    else if (c != DELIM) return errMsg("No space between identifier and it's definition.\n", false);
    else return DELIM;
}

/// Decides whether a string is a valid universe item or not
/// \param str new universe item to be validated
/// \param universe list of already existing universe items
/// \return true when the item meets all criteria (no duplicates, only alphabetical characters, no function names, true, false)
/// \return false when at least one criterium is not fulfilled
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

/// Reads and validates the universe contents
/// \param universe struct where the contents are saved
/// \param file file to be read from
/// \return true when no error occurs during reading, false otherwise
int readUniverse(universe_t *universe, FILE *file)
{
    char *str;
    int c, status;
    if ((c = testSpace(file)) != DELIM) return c;

    do
    {
        // try reading a string from the specified file
        status = readStringFromFile(file, &str);

        // an error occurred while reading the file
        if (!status) return false;

        // get rid of trailing whitespace
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

        // validate the newly added item
        if (!checkUniverse(universe->items[universe->universe_len - 1], universe))
        {
            return errMsg("Duplicity in universe\n", false);
        }
    } while (status != END_OF_LINE);

    // if the loop finishes, we successfully read the universe
    return true;
}


/// Read universe and append it in set structure to the setList, so it can be used in the same way as a regular set
/// \param universe the universe
/// \param sets structure containing all sets in the file
/// \param file file to be read from
/// \return true when no error occurs during reading, false otherwise
int appendUniverse(universe_t *universe, setList_t *sets, FILE *file)
{
    // read universe
    int status = readUniverse(universe, file);

    if (status)
    {
        set_t set = {.set_len = universe->universe_len, .items = NULL};

        if (status == EMPTY_INDEX && insertToSetList(&set, sets)) return true;

        set.items = bigBrainRealloc(set.items, sizeof(int) * set.set_len);
        if (set.items == NULL && set.set_len != 0) return errMsg("Allocation failed\n", false);

        for (int i = 0; i < set.set_len; i++)
        {
            set.items[i] = i;
        }

        if (insertToSetList(&set, sets)) return true;
        else
        {
            free(set.items);
            return false;
        }
    }
    else return false;
}

/// Find the index of the specified string from the universe
/// \param str the string whose index we are looking for
/// \param universe the universe in which we are finding the index
/// \return the index when the universe contains such item, otherwise returns INVALID_INDEX
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

/// Check whether a relation contains the specified relation pair
/// \param relation the relation in which we are searching
/// \param unit the relation pair we are looking for in the relation
/// \return the index when the relation contains such relation pair, otherwise returns INVALID_INDEX
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

/// Parse a relation pair from the specified file
/// \param unit container for the relation pair to be read
/// \param file the file to be read from
/// \param universe the universe the members of the pair have to be part of
/// \return true if a relation pair is successfully read
/// \return EMPTY_INDEX if a trailing whitespace is read
/// \return END_OF_LINE if newline is the next character in the file after the relation pair
/// \return false if an error happens during reading
int readRelationUnit(relationUnit_t *unit, FILE *file, universe_t *universe)
{
    char *strX, *strY;

    int statusX = readStringFromFile(file, &strX);
    if (!statusX) return false;

    // trailing whitespace has been read
    if (statusX == END_OF_LINE && strlen(strX) == 0)
    {
        free(strX);
        return EMPTY_INDEX;
    }

    int statusY = readStringFromFile(file, &strY);
    if (!statusY) return false;

    // check if the pair is in valid format
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

/// Parse an entire relation from the specified file
/// \param relation container for the relation to be read
/// \param file the file to be read from
/// \param universe the universe the members of the relation have to be part of
/// \return true if a relation is successfully read
/// \return false if an error happens during reading
int readRelation(relation_t *relation, FILE *file, universe_t *universe)
{
    relation->relation_len = 0;
    relation->items = NULL;

    // test for opening space
    int c, status;
    if ((c = testSpace(file)) != DELIM) return c;

    // read relation pairs from the file until end of line
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
            if (relation->items == NULL && relation->relation_len != 0)
            {
                return errMsg("Allocation failed.\n", false);
            }
            break;
        }

        // check for duplicates in the current relation
        if (containsRelationUnit(relation, &relation->items[relation->relation_len - 1]) != relation->relation_len - 1)
        {
            return errMsg("Duplicity in a relation\n", false);
        }
    } while (status != END_OF_LINE);

    // if the loop finishes, we successfully read the relation
    return true;
}

int insertToRelatioList(relation_t *relation, relationList_t *relations)
{
    relations->relations = bigBrainRealloc(relations->relations, ++relations->relationList_len * sizeof(relation_t));

    // check for memory errors
    if (relations->relations == NULL)
    {
        return errMsg("Allocation failed\n", false);
    }

    relations->relations[relations->relationList_len - 1].relation_len = relation->relation_len;
    relations->relations[relations->relationList_len - 1].items = relation->items;
    return true;
}

/// Parse a set item from the specified file
/// \param idx container for the set item to be read
/// \param file the file to be read from
/// \param universe the universe the set item has to be part of
/// \return true if a set item is successfully read
/// \return EMPTY_INDEX if a trailing whitespace is read
/// \return END_OF_LINE if newline is the next character in the file after the set item
/// \return false if an error happens during reading
int readSetItem(int *idx, FILE *file, universe_t *universe)
{
    char *str;
    int status = readStringFromFile(file, &str);
    if (!status) return false;

    // trailing whitespace has been read
    if (strlen(str) == 0 && status == END_OF_LINE)
    {
        free(str);
        return EMPTY_INDEX;
    }

    *idx = findUniverseIndex(str, universe);
    free(str);

    // check for validity of the item read
    if (*idx == INVALID_INDEX)
    {
        return errMsg("The set contains items that are not part of the universe.\n", false);
    }
    return status;
}

/// Parse an entire set from the specified file
/// \param set container for the set to be read
/// \param file the file to be read from
/// \param universe the universe the members of the set have to be part of
/// \return true if a set is successfully read
/// \return false if an error happens during reading
int readSet(set_t *set, FILE *file, universe_t *universe)
{
    set->set_len = 0;
    set->items = NULL;

    // test for opening space or empty set
    int c, status;
    if ((c = testSpace(file)) != DELIM) return c;

    // read set items until newline is reached
    do
    {
        set->items = bigBrainRealloc(set->items, ++set->set_len * sizeof(int));
        if (set->items == NULL)
            return errMsg("Allocation failed.\n", false);

        status = readSetItem(&set->items[set->set_len - 1], file, universe);
        if (!status) return false;

            // trailing whitespace has been read
        else if (status == EMPTY_INDEX)
        {
            set->items = bigBrainRealloc(set->items, --set->set_len * sizeof(int));

            if (set->items == NULL && set->set_len != 0)
                return errMsg("Allocation failed.\n", false);

            break;
        }

        // check for duplicates in the current set
        for (int i = 0; i < set->set_len - 1; i++)
        {
            if (set->items[i] == set->items[set->set_len - 1])
                return errMsg("Duplicity in a set\n", false);
        }
    } while (status != END_OF_LINE);

    // if the loop finishes, we successfully read the set
    return true;
}


int insertIntoCommandList(commandList_t *commands, command_t *command)
{
    commands->commands = bigBrainRealloc(commands->commands, ++commands->commandList_len * sizeof(command_t));
    if (commands->commands == NULL) return false;

    commands->commands[commands->commandList_len - 1].functionNameIdx = command->functionNameIdx;
    commands->commands[commands->commandList_len - 1].argc = command->argc;
    commands->commands[commands->commandList_len - 1].parameters = command->parameters;

    return true;
}

/// Free all dynamically allocated memory the universe
/// \param universe the universe to be freed
void freeUniverse(universe_t *universe)
{
    for (int i = 0; i < universe->universe_len; i++)
    {
        free(universe->items[i]);
    }
    free(universe->items);
}

/// Free all dynamically allocated memory for sets
/// \param sets the setList to be freed
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

/// Free all dynamically allocated memory for relations
/// \param relations the relationList to be freed
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

void freeCommands(commandList_t *commands)
{
    if (commands->commandList_len == 0) return;

    for (int i = 0; i < commands->commandList_len; i++)
    {
        if (commands->commands[i].argc != 0) free(commands->commands[i].parameters);
    }
    free(commands->commands);
}

/// Print the universe contents to stdout
/// \param universe the universe to be printed
void printUniverse(universe_t *universe)
{
    printf("U");
    for (int i = 0; i < universe->universe_len; i++)
    {
        printf(" %s", universe->items[i]);
    }
    printf("\n");
}

/// Print set contents to stdout
/// \param set the set to be printed
/// \param universe the universe the set members belong to
void printSet(set_t *set, universe_t *universe)
{
    printf("S");
    for (int i = 0; i < set->set_len; i++)
    {
        printf(" %s", universe->items[set->items[i]]);
    }
    printf("\n");
}

/// Print relation contents to stdout
/// \param relation the relation to be printed
/// \param universe the universe the relation members belong to
void printRelation(relation_t *relation, universe_t *universe)
{
    printf("R");
    for (int i = 0; i < relation->relation_len; i++)
    {
        printf(" (%s %s)", universe->items[relation->items[i].x], universe->items[relation->items[i].y]);
    }
    printf("\n");
}

void printCommands(commandList_t *cmds)
{
    for (int j = 0; j < cmds->commandList_len; j++)
    {
        printf("C %s", functions[cmds->commands[j].functionNameIdx]);
        for (int i = 0; i < cmds->commands[j].argc; i++)
        {
            printf(" %d", cmds->commands[j].parameters[i]);
        }
        printf("\n");
    }
}

void printFile(universe_t *universe, relationList_t *relations, setList_t *sets, commandList_t *commands)
{
    printUniverse(universe);
    int i = 0, j = 1;
    while (i < relations->relationList_len && j < sets->setList_len)
    {
        if (relations->relations[i].index < sets->sets[j].index)
        {
            printRelation(&relations->relations[i], universe);
            i++;
        }
        else
        {
            printSet(&sets->sets[j], universe);
            j++;
        }
    }
    while (i < relations->relationList_len) printRelation(&relations->relations[i++], universe);
    while (j < sets->setList_len) printSet(&sets->sets[j++], universe);

    //printCommands(commands);
}

bool areRelationMembersInSet(relation_t *R, set_t *S, int memberPosition)
{
    for (int i = 0; i < R->relation_len; i++)
    {
        bool status = false;
        if (memberPosition == 1)
        {

            for (int j = 0; j < S->set_len; j++)
            {
                if (R->items[i].x == S->items[j])
                {
                    status = true;
                    break;
                }
            }
        }
        else if (memberPosition == 2)
        {
            for (int j = 0; j < S->set_len; j++)
            {
                if (R->items[i].y == S->items[j])
                {
                    status = true;
                    break;
                }
            }
        }
        else
        {
            return false;
        }

        if (!status)
        {
            return false;
        }
    }
    return true;
}

bool injective(relation_t *R, set_t *S1, set_t *S2)
{
    if (areRelationMembersInSet(R, S1, 1) && areRelationMembersInSet(R, S2, 2))
    {
        return function(R);
    }
    else
    {
        printf("false\n");
        return false;
    }
}

bool surjective(relation_t *R, set_t *S1, set_t *S2)
{
    if (S1->set_len < S2->set_len)
    {
        printf("false\n");
        return false;
    }

    if (areRelationMembersInSet(R, S1, 1) && areRelationMembersInSet(R, S2, 2))
    {
        for (int i = 0; i < S2->set_len; i++)
        {
            bool status = false;
            for (int j = 0; j < R->relation_len; j++)
            {
                if (S2->items[i] == R->items[j].y)
                {
                    status = true;
                }
            }
            if (!status)
            {
                printf("false\n");
                return false;
            }
        }
        return function(R);
    }
    else
    {
        printf("false\n");
        return false;
    }
}

bool bijective(relation_t *R, set_t *S1, set_t *S2)
{
    if (areRelationMembersInSet(R, S1, 1) && areRelationMembersInSet(R, S2, 2))
    {
        if (S1->set_len == S2->set_len)
        {
            return function(R);
        }
        else
        {
            printf("false\n");
            return false;
        }
    }
    else
    {
        printf("false\n");
        return false;
    }
}

int matchStringToFunc(char *command)
{
    for (int i = 0; i < COMBINED_FUNCTIONS_LASTINDEX; i++)
    {
        if (strcmp(command, functions[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int findSet(setList_t *sets, int lineIdx)
{
    for (int i = 0; i < sets->setList_len; i++)
    {
        if (sets->sets[i].index == lineIdx) return i;
        else if (sets->sets[i].index > lineIdx) return INVALID_INDEX;
    }
    return INVALID_INDEX;
}

int findRel(relationList_t *relations, int lineIdx)
{
    for (int i = 0; i < relations->relationList_len; i++)
    {
        if (relations->relations[i].index == lineIdx) return i;
        else if (relations->relations[i].index > lineIdx) return INVALID_INDEX;
    }
    return INVALID_INDEX;
}


int checkArgs(command_t *cmd, setList_t *sets, relationList_t *relations, int *hasBonus)
{
    int arg_count[] = {1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 3, 3, 3};

    int arg_count_bonus[] = {2, 1, 1, 2, 2, 2, 3, 3, 3, 2, 2, 2, 2,
                             2, 2, 2, 2, 1, 1, 4, 4, 4};
    int argc = cmd->argc;

    if (argc != arg_count[cmd->functionNameIdx])
    {
        if (argc != arg_count_bonus[cmd->functionNameIdx]) return false;
        *hasBonus = 1;
        argc--;
    }

    if (cmd->functionNameIdx >= 0 && cmd->functionNameIdx < SET_FUNCTIONS_LASTINDEX && !(cmd->functionNameIdx < 2 || cmd->functionNameIdx > 5))
    {
        for (int j = 0; j < argc; j++)
        {
            if (findSet(sets, cmd->parameters[j]) == INVALID_INDEX)
            {
                return false;
            }
        }
    }
    else if (cmd->functionNameIdx >= SET_FUNCTIONS_LASTINDEX && cmd->functionNameIdx < REL_FUNCTIONS_LASTINDEX)
    {
        for (int j = 0; j < argc; j++)
        {
            if (findRel(relations, cmd->parameters[j]) == INVALID_INDEX)
            {
                return false;
            }
        }
    }
    else if (cmd->functionNameIdx >= REL_FUNCTIONS_LASTINDEX && !(cmd->functionNameIdx < 2 || cmd->functionNameIdx > 5))
    {
        if (findRel(relations, cmd->parameters[0]) == INVALID_INDEX ||
            findSet(sets, cmd->parameters[1]) == INVALID_INDEX ||
            findSet(sets, cmd->parameters[2]) == INVALID_INDEX)
            return false;
    }
    else   return true;

    return true;
}

int readArgs(FILE *file, command_t *command)
{
    char *str, *ptr;
    int status, argc = 0;
    do
    {
        status = readStringFromFile(file, &str);
        if (!status) return false;

        if (status == END_OF_LINE && strlen(str) == 0)
        {
            free(str);
            return true;
        }

        long digit = strtol(str, &ptr, 10);
        free(str);
        /*if (ptr[0] != '\0')
        {
            return errMsg("Command taking wrong index\n", false);
        }*/

        command->parameters = bigBrainRealloc(command->parameters, ++command->argc * sizeof(int));
        if (command->parameters == NULL) return errMsg("Allocation failed.\n", false);

        command->parameters[command->argc - 1] = (int) digit;

    } while (status != END_OF_LINE);

    return true;
}

int readCommands(FILE *file, commandList_t *commands, relationList_t *relations, setList_t *sets, int *bonus)
{
    char c;
    if (testSpace(file) != DELIM) return false;

    char *command;
    int status = readStringFromFile(file, &command);
    if (!status) return false;

    int funcIdx = matchStringToFunc(command);

    if (status == END_OF_LINE || funcIdx == -1)
    {
        free(command);
        return errMsg("Invalid arguments passed to command\n", false);
    }

    command_t cmd = {.functionNameIdx = funcIdx, .argc = 0, .parameters = NULL, .exec = false, .idx = 0};
    free(command);

    if (readArgs(file, &cmd))
    {
        if (!checkArgs(&cmd, sets, relations, bonus))
        {
            if (cmd.argc > 0) free(cmd.parameters);
            return errMsg("Invalid arguments passed to function.\n", false);
        }

        if (!insertIntoCommandList(commands, &cmd))
        {
            if (cmd.argc > 0) free(cmd.parameters);
        }
    }
    else
    {
        if (cmd.argc > 0) free(cmd.parameters);
        return false;
    }

    return true;
}


//TODO
int transitiveClosure(relation_t *relation, universe_t *universe)
{
    if (relation->relation_len == 0)
    {
        printf("R\n");
        return true;
    }
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

/// Free all dynamically allocated memory in the program
/// \param universe the universe to be freed
/// \param relations the relationList to be freed
/// \param sets the setList to be freed
/// \param file the file to be closed
void destructor(universe_t *universe, relationList_t *relations, setList_t *sets, FILE *file, commandList_t *commands)
{
    freeUniverse(universe);
    freeRelations(relations);
    freeSets(sets);
    freeCommands(commands);
    fclose(file);
}

int symmetricClosure(universe_t *universe, relation_t *relation)
{

}

int reflexiveClosure(universe_t *universe, relation_t *relation)
{

}

int execute(commandList_t *cmds, setList_t *sets, relationList_t *relations, universe_t *universe, int hasBonus, int initSize)
{
    int cmd, fileSize = initSize + cmds->commandList_len;
    for (int i = initSize; i < fileSize; i++)
    {
        cmds->commands[i- initSize].idx = i+2; //initsize is 2 off from the real number
        cmds->commands[i- initSize].exec = false; //hello valgrind?
    }

    for (int i = initSize; i < fileSize; i++)
    {
        if (i < initSize)
        {
            bool printed = false;
            for(int o = 0; o < sets->setList_len && !printed; o++)
            {
                if (i == sets->sets[o].index)
                {
                    printSet(&sets->sets[o], universe);
                    printed = true;
                }
            }
            for(int o = 0; o < relations->relationList_len && !printed; o++)
            {
                if (i == relations->relations[o].index)
                {
                    printRelation(&relations->relations[o], universe);
                    printed = true;
                }
            }
        }
        cmd = cmds->commands[i - initSize].functionNameIdx;
        if (i >= initSize)
        {
            if (cmds->commands[i-initSize].exec && cmd < 9)
            {
                set_t *set = {0};
                for (int j = 0; j < sets->setList_len; j++)
                {
                    if (cmds->commands[i- initSize].idx == sets->sets[j].index)
                        set = &sets->sets[j];
                }
                printSet(set, universe);
                continue;
            }
            else if (cmds->commands[i-initSize].exec && cmd >= 9)
            {
                relation_t  *relation = {0};
                for (int j = 0; j < relations->relationList_len; j++)
                {
                    if(cmds->commands[i-initSize].idx == relations->relations[j].index)
                        relation = &relations->relations[j];
                }
                printRelation(relation, universe);
                continue;
            }

            //TODO add default
            switch (cmd) {
                case 0: {
                    if (!empty(&sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[0])]))
                    {
                        if (cmds->commands[i-initSize].argc == 2 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[1] - initSize - 1;
                    }
                    break;
                }
                case 1: {
                    card(&sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[0])]);
                    break;
                }
                case 2: {
                    complement(universe, sets, &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[0])], cmds->commands[i-initSize].idx);
                    cmds->commands[i-initSize].exec = true;
                    break;
                }
                case 3: {
                    Union(universe, &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[0])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[1])], sets, cmds->commands[i-initSize].idx);
                    cmds->commands[i-initSize].exec = true;
                    break;
                }
                case 4: {
                    intersect(universe, &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[0])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[1])], sets, cmds->commands[i-initSize].idx);
                    cmds->commands[i-initSize].exec = true;
                    break;
                }
                case 5: {
                    minus(universe, &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[0])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[1])], sets, cmds->commands[i-initSize].idx);
                    cmds->commands[i-initSize].exec = true;
                    break;
                }
                case 6: {
                    if (!subseteq(&sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[0])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[1])], true))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;
                }
                case 7: {
                    if (!subset(&sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[0])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[1])]))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;
                }
                case 8: {
                    if (!equals(&sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[0])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[1])]))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;
                }
                case 9:
                    if(!reflexive(universe, &relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])]))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;
                case 10:
                    if(!symmetric(&relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])]))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;
                case 11:
                    if(!antisymmetric(&relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])]))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;
                case 12:
                    if(!transitive(&relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])]))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;
                case 13:
                    if(!function(&relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])]))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;
                case 14:
                    domain(universe, &relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])]);
                    break;
                case 15:
                    codomain(universe, &relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])]);
                    break;
                case 16:
                    reflexiveClosure(universe, &relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])]);
                    break;
                case 17:
                    symmetricClosure(universe, &relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])]);
                    break;
                case 18:
                    transitiveClosure(&relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])], universe);
                    break;
                case 19:
                    if(!injective(&relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[1])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[2])]))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;
                case 20:
                    if(!surjective(&relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[1])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[2])]))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;
                case 21:
                    if(!bijective(&relations->relations[findRel(relations, cmds->commands[i-initSize].parameters[0])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[1])], &sets->sets[findSet(sets, cmds->commands[i-initSize].parameters[2])]))
                    {
                        if (cmds->commands[i-initSize].argc == 3 && hasBonus)
                            i = cmds->commands[i-initSize].parameters[2] - initSize - 1;
                    }
                    break;

            }
        }
    }
}

/// Function encapsulating everything that happens with the file
/// \param file the file to be read
/// \return 0 if the entire file is successfully read
/// \return EXIT_FAILURE if an error happens during the file execution
int readFile(FILE *file, universe_t *universe, relationList_t *relations, setList_t *sets, commandList_t *commands)
{
    char c;
    int count = 0, hasU = 0, hasRorS = 0, hasC = 0, hasBonus = 0;

    while (fscanf(file, "%c", &c) != EOF)
    {
        if (++count > MAX_NUM_LINES) return errMsg("The file cannot be more than 1000 lines long.\n", EXIT_FAILURE);

        switch (c)
        {
            case 'U':
            {
                if (++hasU > 1) return errMsg("Invalid file structure.\n", EXIT_FAILURE);

                if (appendUniverse(universe, sets, file))
                {
                    sets->sets[sets->setList_len - 1].index = count;
                }
                else return EXIT_FAILURE;

                break;
            }
            case 'S':
            {
                if (!hasU || hasC) return errMsg("Invalid file structure.\n", EXIT_FAILURE);

                set_t set;
                if (readSet(&set, file, universe) && insertToSetList(&set, sets))
                {
                    hasRorS++;
                    sets->sets[sets->setList_len - 1].index = count;
                }
                else
                {
                    free(set.items);
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'R':
            {
                if (!hasU || hasC) return errMsg("Invalid file structure.\n", EXIT_FAILURE);

                relation_t relation;
                if (readRelation(&relation, file, universe) && insertToRelatioList(&relation, relations))
                {
                    hasRorS++;
                    relations->relations[relations->relationList_len - 1].index = count;
                }
                else
                {
                    free(relation.items);
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'C':
            {
                hasC = 1;
                if (!hasU || hasRorS < 1) return errMsg("Invalid file structure.\n", EXIT_FAILURE);

                if (!readCommands(file, commands, relations, sets, &hasBonus))
                {
                    return EXIT_FAILURE;
                }
                break;
            }

            default:
            {
                return errMsg("Invalid file structure.\n", EXIT_FAILURE);
            }
        }
    }

    if (!hasC) return errMsg("Invalid file structure.\n", EXIT_FAILURE);
    printFile(universe, relations, sets, commands);
    execute(commands, sets, relations, universe, hasBonus, hasRorS);
    return 0;
}

/// Parse input arguments and open file for reading
/// \param argc number of command line arguments passed to the program
/// \param argv list of the arguments passed
/// \return 0 if the file is successfully opened and read
/// \return EXIT_FAILURE when an error happens during reading or the program is run with invalid args
int main(int argc, char **argv)
{
    FILE *f;
    universe_t universe = {.universe_len = 0, .items = NULL};
    relationList_t relations = {.relationList_len = 0, .relations = NULL};
    setList_t sets = {.setList_len = 0, .sets = NULL};
    commandList_t commands = {.commandList_len = 0, .commands = NULL};

    if (argc == 2)
    {
        if ((f = fopen(argv[1], "r")) != NULL)
        {
            int status = readFile(f, &universe, &relations, &sets, &commands);
            destructor(&universe, &relations, &sets, f, &commands);
            return status;
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
