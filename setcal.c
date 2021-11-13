#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> // for seeding random

#define DELIM           ' '
#define MAX_STR_LEN     30
#define MAX_NUM_LINES   1000
#define END_OF_LINE     -1
#define INVALID_INDEX   -2

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
    /*

    add line number

    */
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
    /*

    add line number

    */
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


int readStringFromFile(FILE *file, char **string, char delimStart, char delimStop)
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
        else if ((c == delimStop && strLen != 0) || c == '\n')
        {
            // resize the string to it's true value
            *string = realloc(*string, (strLen + 1) * sizeof(char));

            if (*string == NULL)
                return errMsg("Reallocation failed\n", false);

    
            if (c == '\n') return END_OF_LINE;
            else return true;
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

int readUniverse(universe_t *universe, FILE *file)
{
    universe->universe_len = 0;
    int status; char *str;
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
            universe->items[universe->universe_len - 1] = str;
        }
       
    } while (status != END_OF_LINE);

    // if the loop finishes, we sucessfully read the universe
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

    // the item wasn't found in the universe so it's an invalid item
    return INVALID_INDEX;
}

int readIndexedItem(int *idx, FILE *file, universe_t *universe, char delimStart, char delimStop)
{
    char *str;
    int status = readStringFromFile(file, &str, delimStart, delimStop);

    if (!status) 
    {
        free(str);
        return INVALID_INDEX; // an error occured while reading the string
    }

    else
    {
        // when using this function with relations, the newline character gets saved seperately
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
        else return errMsg("The set/relation contains items that do not belong in the universe\n", INVALID_INDEX);
    }
}

int readSet(set_t *set, FILE *file, universe_t *universe)
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
                return errMsg("Allocation failed.\n", false);

            set->items[set->set_len - 1] = idx;
        }
    } while (status != END_OF_LINE);

    // if the loop finishes, we sucessfully read the set
    return true;
}

int readRelation(relation_t *relation, FILE *file, universe_t *universe)
{
    relation->relation_len = 0;
 
    int statusX, statusY, idx, idy;
    do
    {
        statusX = readIndexedItem(&idx, file, universe, '(', DELIM);
        if (statusX == INVALID_INDEX) return false;
        else if (statusX == END_OF_LINE) break;

        statusY = readIndexedItem(&idy, file, universe, DELIM, ')');
        if (statusY == INVALID_INDEX) return false;
        else if (statusY == END_OF_LINE) break;

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
                return errMsg("Allocation failed.\n", false);
            }

            relation->items[relation->relation_len - 1].x = idx;
            relation->items[relation->relation_len - 1].y = idy;
        }
    } while (statusX != END_OF_LINE && statusY != END_OF_LINE);

    // if the loop finishes, we sucessfully read the relation
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

// free all mallocated sets
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

// free all mallocated relations
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

// delete later - only for validity checking
void printUniverse(universe_t *universe)
{
    printf("U ");
    for (int i = 0; i < universe->universe_len; i++)
    {
        printf("%s ", universe->items[i]);
    }
    printf("\n");
}

// delete later - only for validity checking
void printSet(set_t *set, universe_t *universe)
{
    printf("S ");
    for (int i = 0; i < set->set_len; i++)
    {
        printf("%s ", universe->items[set->items[i]]);
    }
    printf("\n");
}

// delete later - only for validity checking
void printRelation(relation_t *relation, universe_t *universe)
{
    printf("R ");
    for (int i = 0; i < relation->relation_len; i++)
    {
        printf("( %s %s ) ", universe->items[relation->items[i].x], universe->items[relation->items[i].y]);
    }
    printf("\n");
}



int readFile(FILE *file)
{
    char c;
    universe_t universe;
    relationList_t relations;
    setList_t sets;

    relations.relationList_len = 0;
    sets.setList_len = 0;
    
    while (fscanf(file, "%c", &c) != EOF)
    {
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

                if (readSet(&sets.sets[sets.setList_len - 1], file, &universe)) 
                { 
                    printSet(&sets.sets[sets.setList_len - 1], &universe);
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

                if (readRelation(&relations.relations[relations.relationList_len - 1], file, &universe)) 
                { 
                    printRelation(&relations.relations[relations.relationList_len - 1], &universe);
                } 
                break;
            }
            case 'C':
            {
                /*

                    READ COMMANDS

                */
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