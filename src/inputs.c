#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "inputs.h"
#include "types.h"


void get_file_inputs()
{
    add_flag();
    seed_rand_function();
    add_valid_walls();
    add_valid_poles();
    add_valid_stairs();
}

//opens a requested file in inputs folder, 
//also checks for errors
FILE* open_file(char* path)
{
    FILE* f = fopen(path, "r");
    
    if (f == NULL)
    {
        printf("%s not found\n", path);
        return NULL;
    }
    return f;
}


void seed_rand_function()
{
    FILE* f = open_file(SEED_TXT);
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


//checks if placing object at location violates any rules
/*
Rules for object placement:
    1. Object MUST be in map bounds
    2. Object MUST be on not empty cell
    3. For the types (-> can be placed on):
        Stair -> NOT WALL
        Pole -> NOT WALL
        Wall -> GAME 
        Flag -> GAME

NOTE: SPECIAL returns LINK_START or LINK_BAWANA if in those areas
*/
int can_place_object(int floor, int width, int length, CELL_TYPE type)
{
    if (!cell_in_maze_bounds(floor, width, length))
    {
        return 0;
    }
    //start cell special case
    if (cell_in_start_area(floor, width, length))
    {
        return LINK_START;
    }
    //puts("Cell in bounds");
    cell* current_cell = maze[floor][width][length];
    //printf("%p\n", current_cell);
    if (current_cell == NULL)
    {
        return 0;
    }
    //puts("Cell not NULL");
    CELL_TYPE maze_cell_type = current_cell->type;
    if (maze_cell_type == BAWANA)
    {
        return LINK_BAWANA;
    }
    switch (type)
    {
        case STAIR:
            return (maze_cell_type != WALL);
        case POLE:
            return (maze_cell_type != WALL);
        case WALL:
            return (maze_cell_type == GAME);
        case FLAG:
            return (maze_cell_type == GAME);
    }
}


//returns a malloced list of stairs, if no stairs returns NULL
void add_valid_stairs()
{
    FILE *f = open_file(STAIRS_TXT);
    if (f == NULL)
    {
        puts("No stairs will be added.");
        return;
    }

    char buff[100];
    stair stair_list[STAIR_CAP];
    while (fgets(buff, sizeof(buff), f) != NULL)
    {
        unsigned int start_floor, start_width, start_length;
        unsigned int end_floor, end_width, end_length;
        int args = sscanf(
            buff, 
            STAIR_FORMAT, 
            &start_floor,
            &start_width,
            &start_length,
            &end_floor,
            &end_width,
            &end_length
        );
        if (args != 6)
        {
            printf("Stair %s is formatted incorrectly.\n", buff);
        }
        // check for invalid stairs, by definition
        if (start_floor >= end_floor)
        {
            printf("Stair %s should be defined from lower to higher floor\n", buff);
            continue;
        }

        int start_return_value = can_place_object(start_floor, start_width, start_length, STAIR);
        int end_return_value = can_place_object(end_floor, end_width, end_length, STAIR);

        // check for invalid stairs, by out of bounds
        if (!start_return_value || !end_return_value)
        {
            printf("Invalid stair %s\n", buff);
            continue;        
        }
        cell* start_cell; //= maze[start_floor][start_width][start_length];
        cell* end_cell; //= maze[end_floor][end_width][end_length];

        //process links now
        //only need to check lower floor, as that's the only place with bawana and start
        if (start_return_value == LINK_START)
        {
            start_cell = start_link_cell;
        }
        else if (start_return_value == LINK_BAWANA)
        {
            start_cell = bawana_link_cell;
        }
        else
        {
            start_cell = maze[start_floor][start_width][start_length];
        }

        end_cell = maze[end_floor][end_width][end_length];


        // check for 2 staircases case
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


        // add the stair to starting cell
        if (start_cell->neighbours[FORCED] == NULL)
        {
            start_cell->neighbours[FORCED] = end_cell;
            start_cell->n1 = stair_count;
        }
        else
        {
            start_cell->neighbours[SECOND] = end_cell;
            start_cell->n2 = stair_count;
        }

        // add the stair to ending cell
        if (end_cell->neighbours[FORCED] == NULL)
        {
            end_cell->neighbours[FORCED] = start_cell;
            end_cell->n1 = stair_count;
        }
        else
        {
            end_cell->neighbours[FORCED] = start_cell;
            end_cell->n2 = stair_count;
        }

        // fix cell types
        //edge case 
        if (start_return_value == 1)
        {
            start_cell->type = STAIR;
        }
        end_cell->type = STAIR;

        // add stair to stair list
        stair current_stair = (stair){start_cell, end_cell, BIDIRECTIONAL};
        stair_list[stair_count++] = current_stair;
    }
    fclose(f);

    // handle no stairs edge case
    if (stair_count < 1)
    {
        puts("No stairs added to game.");
        return;
    }

    // malloc the stair list
    stairs = (stair*)malloc(stair_count * sizeof(stair));

    //handle malloc fail
    if (stairs == NULL)
    {
        puts("Failed to alloc stairs.");
        puts("Quitting game");
        free_map();
        exit(-1);
    }

    for (int stair_index = 0; stair_index < stair_count; stair_index++)
    {
        stairs[stair_index] = stair_list[stair_index];
    }
}


void add_valid_poles()
{
    FILE *f = open_file(POLES_TXT);
    if (f == NULL)
    {
        puts("No poles will be added.");
        return;
    }

    char buff[100];
    pole pole_list[GAME_CELL_CAP];
    while (fgets(buff, sizeof(buff), f) != NULL)
    {
        unsigned int start_floor, end_floor, width, length;
        int args = sscanf(
            buff,
            POLE_FORMAT,
            &start_floor,
            &end_floor,
            &width,
            &length
        );
        if (args != 4)
        {
            printf("Pole %s is formatted incorrectly.\n", buff);
            continue;
        }

        //check for pole validity
        if (start_floor == end_floor)
        {
            printf("Poles %s must be defined from/to different floors.\n", buff);
            continue;
        }

        int top_floor = MAX(start_floor, end_floor);
        int bottom_floor = MIN(start_floor, end_floor);

        int top_return_value = can_place_object(start_floor, width, length, POLE);
        int bottom_return_value = can_place_object(end_floor, width, length, POLE);

        if (!top_return_value || !bottom_return_value)
        {
            //handle edge case where pole defined from 
            //illegal location, to intersect a legal location
            if (top_floor - bottom_floor == 2 && can_place_object(1, width, length, POLE))
            {
                top_floor = 1;
            }
            else
            {            
                printf("%i\n", can_place_object(1, width, length, POLE));
                printf("Pole %s is invalid.\n", buff);
                continue;
            }
        }
        pole to_add;

        cell* top_cell; //= maze[top_floor][width][length];
        cell* bottom_cell; //= maze[bottom_floor][width][length];

        //process links
        //NOTE: only bottom cell can be bawana or start
        if (bottom_return_value == LINK_START)
        {
            bottom_cell = start_link_cell;
        }
        else if (bottom_return_value == LINK_BAWANA)
        {
            bottom_cell = start_link_cell;
        }
        else
        {
            bottom_cell = maze[bottom_floor][width][length];
        }
        top_cell = maze[top_floor][width][length];

        to_add.bottom_cell = bottom_cell;
        to_add.top_cell = top_cell;
        //if middle cell exists, add that cell as a separete pole
        if (top_floor - bottom_floor == 2 && can_place_object(1, width, length, POLE))
        {
            cell* middle_cell = maze[1][width][length];
            middle_cell->type = POLE;
            middle_cell->neighbours[FORCED] = bottom_cell;
            middle_cell->neighbours[SECOND] = NULL;
            to_add.middle_cell = middle_cell;
        }
        top_cell->type = POLE;
        top_cell->neighbours[FORCED] = bottom_cell;
        top_cell->neighbours[SECOND] = NULL;

        pole_list[pole_count++] = to_add;
    }
    fclose(f);

    // handle no poles edge case
    if (pole_count < 1)
    {
        puts("No poles added to game.");
        return;
    }

    // malloc the pole list
    poles = (pole*)malloc(pole_count * sizeof(pole));

    //handle malloc fail
    if (poles == NULL)
    {
        puts("Failed to alloc poles.");
        puts("Quitting game");
        free_map();
        free(stairs);
        exit(-1);
    }

    for (int pole_index = 0; pole_index < pole_count; pole_index++)
    {
        poles[pole_index] = pole_list[pole_index];
    }

}


void add_valid_walls()
{
    FILE *f = open_file(WALLS_TXT);
    if (f == NULL)
    {
        puts("No additional walls will be added.");
        return;
    }

    char buff[100];
    while (fgets(buff, sizeof(buff), f) != NULL)
    {
        unsigned int floor, start_width, start_length;
        unsigned int end_width, end_length;
        int args = sscanf(
            buff,
            WALL_FORMAT,
            &floor,
            &start_width,
            &start_length,
            &end_width,
            &end_length
        );
        if (args != 5)
        {
            printf("Wall %s is formatted incorrectly.\n", buff);
            continue;
        }

        //check for wall validity, by definition
        if (
            !(start_width == end_width ||
            start_length == end_length)
        )
        {
            printf("Wall %s is illegally defined.\n", buff);
            continue;
        }

        int valid = 1;
        if (start_width == end_width)
        {                
            int s_length = MIN(start_length, end_length);
            int l_length = MAX(start_length, end_length);
            //iterate over each wall cell, to ensure it is valid
            for (int i = s_length ; i <= l_length ; i++)
            {
                if (!can_place_object(floor, start_width, i, WALL))
                {
                    valid = 0;
                }
            }
            if (valid == 0)
            {
                printf("Wall %s is invalid.\n", buff);
                continue;
            }
            fill_section(floor, start_width, s_length, start_width, l_length, WALL);
        }
        else
        {
            int s_width = MIN(start_width, end_width);
            int l_width = MAX(start_width, end_width);
            //iterate over each wall cell, to ensure it is valid
            for (int i = s_width; i<= l_width; i++)
            {
                if (!can_place_object(floor, i, start_length, WALL))
                {
                    valid = 0;
                }
            }
            if (valid == 0)
            {
                printf("Wall %s is invalid.\n", buff);
                continue;
            }
            fill_section(floor, s_width, start_length, l_width, start_length, WALL);
        }
    }
    fclose(f); 
}


//if flag is illegally defined, the game cannot start
//NOTE: this function should be called first
void add_flag()
{
    FILE *f = open_file(FLAG_TXT);
    if (f == NULL)
    {
        printf("ERROR: Unable to locate flag.txt\n");
        puts("Quitting game.");
        free_map();
        exit(-1);
    }
    char buff[100];
    //puts("Trying to add flag");
    if (fgets(buff, sizeof(buff), f) != NULL)
    {
        unsigned int floor, width, length;
        int args = sscanf(
            buff,
            FLAG_FORMAT,
            &floor,
            &width,
            &length
        );
        //puts("Scanned flag");
        if (args != 3)
        {
            printf("ERROR: Flag %s is invalidly formatted.\n", buff);
            puts("Quitting game.");
            free_map();
            exit(-1);
        }

        if (!can_place_object(floor, width, length, FLAG))
        {
            printf("ERROR: Flag %s is illegally defined.\n", buff);
            puts("Quitting game.");
            free_map();
            exit(-1);
        }
        flag = maze[floor][width][length];
        flag->type = FLAG;
    }
    else
    {
        printf("ERROR: Flag must be defined.\n");
        puts("Quitting game.");
        free_map();
        exit(-1);
    }
    fclose(f);
    
}