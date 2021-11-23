///\file
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h> // for seeding random

//TODO rename set1, set2 -> A, B

#define DELIM           ' '
#define MAX_STR_LEN     30
#define MAX_NUM_LINES   1000
#define END_OF_LINE     -1
#define INVALID_INDEX   -2
#define EMPTY_INDEX     -3

// STRUCTURES

// struct to store universe contents
typedef struct{
    char **items;
    int universe_len;
}
universe_t;

// struct to store a single set
typedef struct{
    int *items;
    int set_len;
    long index;
}
set_t;

// struct to store all sets in the file
typedef struct{
    set_t *sets;
    int setList_len;
}
setList_t;

// indices of the related strings from universe
typedef struct {
    int x;  
    int y;
} 
relationUnit_t;

//struct to store a relationship set
typedef struct {
    relationUnit_t *items;
    int relation_len;
    long index;
}
relation_t;

// struct to store all relationships in a file
typedef struct{
    relation_t *relations;
    int relationList_len;
}
relationList_t;

// function for printing error messages to stderr
int errMsg(char *msg, int status)
{
    fprintf(stderr, "%s", msg);
    return status;
}

// big brain time
void *bigBrainRealloc(void *ptr, size_t size)
{
    void *tmp = realloc(ptr, size);
    if (tmp == NULL)
    {
        free(ptr);
        return NULL;
    }

    else return tmp;
}

//FIXME temporary
void empty (set_t *set)
{
    if (!set->set_len) {
        printf("true\n");
    }
    printf("false\n");
}

void card (set_t *set)
{
    printf("%d\n", set->set_len);
}

int complement (universe_t *universe, set_t *set) //TODO TEST
{
    int *wholeSet = NULL;
    wholeSet = bigBrainRealloc(wholeSet, universe->universe_len * sizeof(int));
    if (wholeSet == NULL)
        return errMsg("Allocation failed\n", false);

    printf("S ");
    for (int o = 0; o < universe->universe_len; o++)
    {
        wholeSet[o] = o;
    }
    for (int o = 0; o < set->set_len; o++)
    {
        wholeSet[set->items[o]] = -1;
    }
    for (int o = 0; o < universe->universe_len; o++)
    {
        if (wholeSet[o] != -1) printf("%s ", universe->items[o]);
    }
    printf("\n");
    free(wholeSet);
    return true;
}

int Union(universe_t *universe, set_t *set1, set_t *set2)
{
    int len;
    int *uni = NULL;
    uni = bigBrainRealloc(uni, set1->set_len * sizeof (int));
    if (uni == NULL)
        return errMsg("Allocation failed\n", false);

    len = set1->set_len;

    memcpy(uni, set1->items, set1->set_len * sizeof(int));

    int j;
    for (int i = 0; i < set2->set_len; i++)
    {
        for (j = 0; j < set1->set_len; j++)
        {
            if (set1->items[j] == set2->items[i])
            {
                j = 0;
                break;
            }
        }
        if(j != 0)
        {
            len++;
            uni = bigBrainRealloc(uni, len * sizeof(int));
            if (uni == NULL)
                return errMsg("Allocation failed\n", false);

            memcpy(&uni[len-1], &set2->items[i], sizeof(int));
            // uni[len-1] = set2->items[i];
        }

    }

    printf("S ");
    for(int i = 0; i < len; i++)
    {
        printf("%s ", universe->items[uni[i]]);
    }
    printf("\n");
    free(uni);
    return true;

}

void intersect (universe_t *universe ,set_t *A, set_t *B)
{ 
    printf("S ");
    for (int i = 0; i < A->set_len; i++)
    {
        for (int j = 0; j < B->set_len; j++)
        {
            if (A->items[i] == B->items[j])
            {
                printf("%s ", universe->items[A->items[i]]);
                break;
            }            
        }
    }
    printf("\n");
}

int minus (universe_t *universe, set_t *set1, set_t *set2)
{
    int *min = NULL;
    min = bigBrainRealloc(min, set1->set_len * sizeof(int));

    if (min == NULL)
        return errMsg("Allocation failed\n", false);

    memcpy(min, set1->items, set1->set_len * sizeof(int));

    printf("S ");
    for(int i = 0; i < set1->set_len; i++)
    {
        for(int j = 0; j < set2->set_len; j++)
        {
            if (set1->items[i] == set2->items[j])
            {
                min[i] = EMPTY_INDEX;
                break;
            }
        }
        if (min[i] != EMPTY_INDEX)
            printf("%s ", universe->items[min[i]]);
    }
    printf("\n");
    free(min);
    return true;
}

set_t subseteq (set_t *A, set_t *B, bool print)
{
  int count = 0;
  for (int i = 0; i <A->set_len; i++)
  {
    for (int j = 0; j < B->set_len; j++)
    {
      if (A->items[i] == B->items[j])
      {
        count++;
      }
    }
  }
  set_t terminus = {.set_len = 0}; // pro naznaceni ze A neni podmnozinou
  if (!print && count == A->set_len) return *A;
  else if (!print && count != A->set_len) return terminus;  
  if (count == A->set_len)
  {
    printf("true\n");
  }
  else
  {
    printf("false\n");
  }
  return terminus;
}
///subset() checks whether A is a proper subset of B
/// \param A is the suspected proper subset
/// \param B is the suspected proper superset
void subset (set_t *A, set_t *B)
{
    set_t temp;
    if (A->set_len == 0) printf("true\n");
    else
    {
        temp = subseteq(A, B, false);
    }

    if (!temp.set_len) printf("false\n");
    else if (temp.set_len < B->set_len) printf("true\n");
    else printf("false\n");

}

void equals(set_t *set1, set_t *set2)
{
    for (int i = 0; i < set1->set_len; i++)
    {
        if (set1->items[i] == set2->items[i])
        {
            printf("true\n");
            return;
        }
    }
    printf("false\n");
}

int readStringFromFile(FILE *file, char **string)
{
    char c; int strLen = 0;

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

    
            if (c == '\n') return END_OF_LINE;
            else return true;
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

int checkUniverse(char *str, universe_t *universe)
{
    for(int i = 0; i < universe->universe_len - 1; i++)
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
    universe->items = NULL;
    int status; char *str;
    do
    {
        // try reading a string from the specified file
        status = readStringFromFile(file, &str);

         // an error occurred while reading the file
        if (!status)
        {
            free(str);
            return false;
        }
            

        // empty universe
        if (status == END_OF_LINE && strlen(str) == 0) 
        {
            free(str);
            return true;
        }

        else
        {
            universe->items = bigBrainRealloc(universe->items, ++universe->universe_len * sizeof(char *));
            
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

int readSet(set_t *set, FILE *file, universe_t *universe)
{
    set->set_len = 0;
    set->items = NULL;

    int idx, status;    char *str;

    do
    {
        status = readStringFromFile(file, &str);

        // empty set
        if (strlen(str) == 0 && status == END_OF_LINE) return true;

        idx = findUniverseIndex(str, universe);
        free(str);

        if (!status)    return false;
        else
        {
            if (idx == INVALID_INDEX)
            {
                return errMsg("The set contains items that are not part of the universe.\n", false);
            }
            else
            {
                set->items = bigBrainRealloc(set->items, ++set->set_len * sizeof(int));
                if (set->items == NULL)
                    return errMsg("Allocation failed.\n", false);

                for (int i = 0; i < set->set_len - 1; i++)
                {
                    if (set->items[i] == idx)
                        return errMsg("Duplicity in a set\n", false);
                }

                set->items[set->set_len - 1] = idx;
            }
        }
    } while (status != END_OF_LINE);

    // if the loop finishes, we successfully read the set
    return true;
}

int readRelation(relation_t *relation, FILE *file, universe_t *universe)
{
    relation->relation_len = 0;
    relation->items = NULL;

    int statusX, statusY, idx, idy;
    char *str;
    do
    {
        statusX = readStringFromFile(file, &str);

        if (!statusX) return false;
        else if (statusX == END_OF_LINE && strlen(str) == 0) return true;
        else if (str[0] != '(') return errMsg("Invalid relation.\n", false);

        // compare without opening brace
        idx = findUniverseIndex(&str[1], universe);
        free(str);

        statusY = readStringFromFile(file, &str);

        if (!statusY) return false;
        else if (str[strlen(str) - 1] != ')')
            return errMsg("Invalid relation.\n", false);

        // get rid of closing brace before the comparison
        str[strlen(str) - 1] = '\0';
        idy = findUniverseIndex(str, universe);
        free(str);

        if (idx == INVALID_INDEX || idy == INVALID_INDEX)
            return errMsg("The relation contains items that are not part of the universe.\n", false);

        relation->items = bigBrainRealloc(relation->items, ++relation->relation_len * sizeof(relationUnit_t));
        if (relation->items == NULL)
        {
            return errMsg("Allocation failed.\n", false);
        }

        for (int i = 0; i < relation->relation_len - 1; i++)
        {
            if (relation->items[i].x == idx && relation->items[i].y == idy)
                return errMsg("Duplicity in a relation\n", false);
        }
        relation->items[relation->relation_len - 1].x = idx;
        relation->items[relation->relation_len - 1].y = idy;

    } while (statusX != END_OF_LINE && statusY != END_OF_LINE);

    // if the loop finishes, we successfully read the relation
    return true;
}

int appendUniverse(universe_t *universe, setList_t *sets, FILE *file)
{
    if (readUniverse(universe, file))
    {
        sets->sets = bigBrainRealloc(sets->sets, ++sets->setList_len * sizeof(set_t));
        if (sets->sets == NULL)
            return false;

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
    else return false;
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
    else
    {
        return false;
    }
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
    if (sets->setList_len == 0) return; // the list is empty, nothing to free

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
    if (relations->relationList_len == 0) return; // the list is empty, nothing to free

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
    printf("U ");
    for (int i = 0; i < universe->universe_len; i++)
    {
        printf("%s ", universe->items[i]);
    }
    printf("\n");
}

// print set contents
void printSet(set_t *set, universe_t *universe)
{
    printf("S ");
    for (int i = 0; i < set->set_len; i++)
    {
        printf("%s ", universe->items[set->items[i]]);
    }
    printf("\n");
}

// print relation contents
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
    /*
    char c;
    char *ptr;
    long indexes[2], digit;
    int order = 0;
    while (fscanf(file, "%c", &c) != EOF)
    {
        if (isdigit(c))
        {
            digit = strtol(&c, &ptr, 10);
            if (digit < 1 || *ptr != '\0') return errMsg("Command taking wrong index", false);
            else if (digit > count) return EMPTY_INDEX; // TODO bonus
            else indexes[order] = digit;
        }
        else if (c == '\n')
            break;

        order++;
    }
    return *indexes;
     */

    char *idx;
    char *ptr;
    long indexes[2] = {0}, digit;
    int order = 0;
    while (readStringFromFile(file, &idx) != END_OF_LINE)
    {
        digit = strtol(idx, &ptr, 10);
        if (digit < 1 || *ptr != '\0') return errMsg("Command taking wrong index", false);
        else if (digit > count) return EMPTY_INDEX; // TODO bonus
        else indexes[order] = digit;

        order++;
    }
    return *indexes;
}

// FIXME if this DOESNT work let me know as soon as possible
int readCommands(universe_t *universe, relationList_t *relations, setList_t *sets, FILE *file, int count)
{
    char *command;
    int status;
    status = readStringFromFile(file, &command);
    if (status == END_OF_LINE) return true; //TODO not needed?
    else if (status == INVALID_INDEX) return false;
    /*
     *
     * selection of the wanted command
     * use char *command to decide which command is required
     * within these functions use the readIndex() to get indexes of the required lines
     * enter their data through setList->sets[index].items[i]
     * or relationsList->relations[index].items[i]
     *
     */
    free(command);
    return true;

}

bool containsRelationUnit(relation_t *relation, relationUnit_t *unit)
{
  for (int i = 0; i < relation->relation_len; i++)
  {
    if (relation->items[i].x == unit->x && relation->items[i].y == unit->y)
    {
      return true;
    }
  }
  return false;
}

int transitiveClosure(relation_t *relation, universe_t *universe)
{
    relation_t tmp = {.items = NULL, .relation_len = relation->relation_len};
    tmp.items = malloc(sizeof(relationUnit_t) * relation->relation_len);
    if (tmp.items == NULL) return errMsg("Allocation failed.\n", false);

    memcpy(tmp.items, relation->items, sizeof(relationUnit_t) * relation->relation_len);
    for (int i = 0; i < tmp.relation_len; i++)
    {
        for (int j = 0; j < tmp.relation_len; j++)
        {
            if (tmp.items[i].y == tmp.items[j].x)
            {
                relationUnit_t unit = {.x = tmp.items[i].x, .y = tmp.items[j].y};
                if (!containsRelationUnit(&tmp, &unit))
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
    int count = 0;
    universe_t universe;
    relationList_t relations = {.relationList_len = 0, .relations = NULL};
    setList_t sets = {.setList_len = 0, .sets = NULL};

    while (fscanf(file, "%c", &c) != EOF)
    {
        count++;
        switch (c)
        {
            case 'U': 
            {
                if (appendUniverse(&universe, &sets, file))
                {
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
                if (appendSet(&sets, &universe, file))
                {
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
                if (appendRelation(&relations, &universe, file))
                {
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
                /*
                //I suppose prints will happen in each function
                if (readCommands(&universe, &relations, &sets, file, count))
                {
                    break;
                }
                else
                {
                    return EXIT_FAILURE;
                }
                */
               break;
            }

            default:
            {
                destructor(&universe, &relations, &sets, file);
                return errMsg("Invalid file structure.\n", EXIT_FAILURE);
            }
        }
    }
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
