#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "inputs.h"
#include "types.h"


//opens a requested file in inputs folder, 
//also checks for errors
FILE* open_file(char* filename)
{
    char to_open[100];
    sprintf(to_open, "./inputs/%s", filename);
    FILE* f = fopen(to_open, "r");
    
    if (f == NULL)
    {
        printf("%s not found in inputs folder\n", filename);
        return NULL;
    }
    return f;
}


void seed_rand_function()
{
    FILE* f = open_file("seed.txt");
    if (f == NULL)
    {
        puts("Using random seed.");
        srand(time(NULL));
        return;
    }

    unsigned int seed;
    if(fscanf(f, "%u", &seed) != 1)
    {
        puts("seed.txt must contain an unsigned int on the first line.");
        puts("using random seed.");
        srand(time(NULL));
        return;
    }
    fclose(f);
    srand(seed);
}


//returns a malloced list of stairs, if no stairs returns NULL
stair* add_valid_stairs()
{
    FILE *f = open_file("stairs.txt");
    if (f == NULL)
    {
        puts("No stairs will be added.");
        return NULL;
    }

    char buff[100];
    stair stair_list[500];
    while (fscanf(f, STAIR_FORMAT, &buff) == 6)
    {
        unsigned int start_floor, start_width, start_length;
        unsigned int end_floor, end_width, end_length;
        sscanf(
            buff, 
            STAIR_FORMAT, 
            start_floor,
            start_width,
            start_length,
            end_floor,
            end_width,
            end_length
        );
        //check for invalid stairs, by definition
        if (start_floor >= end_floor)
        {
            printf("Stair %s should be defined from lower to higher floor\n", buff);
            continue;
        }

        cell* start_cell = maze[start_floor][start_width][start_length];
        cell* end_cell = maze[end_floor][end_width][end_length];
        
        if (
            start_cell == NULL ||
            end_cell == NULL
        )
        {
            printf("Invalid stair %s\n", buff);
            continue;
        }

        //check for cell validity by type
        if (start_cell->type != GAME || end_cell->type != GAME)
        {
            printf("Stair %s is defined from/to invalid blocks.\n", buff);
            continue;
        }


        //check for 2 staircases case
        if (
            start_cell->neighbours[FORCED] != NULL &&
            start_cell->neighbours[SECOND] != NULL 
            ||
            end_cell->neighbours[FORCED] != NULL &&
            end_cell->neighbours[SECOND] != NULL
        )
        {
            printf("Only two stairs may be defined for a cell.\n");
            printf("Stair %s has been discarded", buff);
            continue;
        }


        //add the stair to starting cell
        if (start_cell->neighbours[FORCED] == NULL)
        {
            start_cell->neighbours[FORCED] = end_cell;
        }
        else
        {
            start_cell->neighbours[SECOND] = end_cell;
        }

        //add the stair to ending cell
        if (end_cell->neighbours[FORCED] == NULL)
        {
            end_cell->neighbours[FORCED] = start_cell;
        }
        else
        {
            end_cell->neighbours[FORCED] = start_cell;
        }

        //fix cell types
        start_cell->type = STAIR;
        end_cell->type = STAIR;

        //add stair to stair list
        stair current_stair = (stair){start_cell, end_cell, BIDIRECTIONAL};
        stair_list[stair_count++] = current_stair;
    }
    //handle no stairs edge case
    if (stair_count < 1)
    {
        puts("No stairs added to game.");
        return NULL;
    }

    //malloc the stair list
    stair* stairs = malloc(stair_count * sizeof(stair));

    for (int stair_index = 0; stair_index < stair_count; stair_index++)
    {
        stairs[stair_index] = stair_list[stair_index];
    }
    return stairs;
}