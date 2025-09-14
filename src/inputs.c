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
    get_valid_stairs();
    get_valid_poles();
}


void seed_rand_function()
{
    FILE* f = fopen(SEED_TXT, "r");
    if (f == NULL)
    {
        log_issue(FILE_NOT_FOUND, GAME, 0, NULL, "Random seed was used.");
        srand(time(NULL));
        return;
    }

    unsigned int seed;
    if(fscanf(f, "%u", &seed) != 1)
    {
        log_issue(INVALID_FORMAT, GAME, 0, NULL, "Random seed was used.");
        fclose(f);
        srand(time(NULL));
        return;
    }
    fclose(f);
    srand(seed);
}


//adds a malloced list of stairs
void get_valid_stairs()
{
    FILE *f = fopen(STAIRS_TXT, "r");
    if (f == NULL)
    {
        log_issue(FILE_NOT_FOUND, STAIR, 0, NULL, "No stairs were added.");
        return;
    }

    char buff[100];
    stair stair_list[STAIR_CAP];
    while (fgets(buff, sizeof(buff), f) != NULL)
    {
        int start_floor, start_width, start_length;
        int end_floor, end_width, end_length;
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
            log_issue(INVALID_FORMAT, STAIR, 0, buff, "Stair was not added.");
            continue;
        }
        // check for invalid stairs, by definition
        if (start_floor >= end_floor)
        {
            log_issue(INCORRECT_FORMAT, STAIR, 0, buff, "A correct format was assumed for stair.");
            swap(&start_floor, &end_floor);
            swap(&start_width, &end_width);
            swap(&start_length, &end_length);
        }

        if (
            !cell_in_maze_bounds(start_floor, start_width, start_length) ||
            !cell_in_maze_bounds(end_floor, end_width, end_length)
        )
        {
            log_issue(OUT_OF_MAZE_BOUNDS, STAIR, 0, buff, "Stair was not added.");
            continue;
        }

        cell* start_cell = maze[start_floor][start_width][start_length];
        cell* end_cell = maze[end_floor][end_width][end_length];

        //start cell is the bottom cell and can be bawana or start
        if (cell_in_start_area(start_floor, start_width, start_length))
        {
            start_cell = start_link_cell;
        }

        if (start_cell == NULL || end_cell == NULL)
        {
            log_issue(DEFINED_IN_VOID, STAIR, 0, buff, "Stair was not added.");
            continue;
        }

        if (start_cell->type == BAWANA)
        {
            start_cell = bawana_link_cell;
        }

        if (
            (start_cell->type != GAME && start_cell->type != LINK_BAWANA && start_cell->type != STAIR && start_cell->type != LINK_START) ||
            (end_cell->type != GAME && end_cell->type != STAIR)
        )
        {
            log_issue(OBJECT_COLLISION, STAIR, 0, buff, "Stair was not added.");
            continue;
        }
        char ret_s = assign_to_forced_then_second(start_cell, end_cell, stair_count);
        if (ret_s == 0)
        {
            log_issue(TOO_MANY_OBJECTS, STAIR, 0, buff, "Stair was not added.");
            continue;
        }
        char ret_e = assign_to_forced_then_second(end_cell, start_cell, stair_count);
        if (ret_e == 0)
        {
            log_issue(TOO_MANY_OBJECTS, STAIR, 0, buff, "Stair was not added.");
            //undo previous assignment
            if (ret_s == 'F')
            {
                start_cell->neighbours[FORCED] = NULL;
            }
            else if (ret_s == 'S')
            {
                start_cell->neighbours[SECOND] = NULL;
            }
        }
        //just add the stair here as well
        if (start_cell->type == GAME)     
        {
            start_cell->type = STAIR;
        }    
        if (end_cell->type == GAME)
        {
            end_cell->type = STAIR;
        }

        stair current_stair = (stair){start_cell, end_cell, BIDIRECTIONAL};
        stair_list[stair_count++] = current_stair;
    }
    fclose(f);

    // handle no stairs edge case
    if (stair_count < 1)
    {
        log_issue(NO_OBJECTS_ADDED, STAIR, 0, NULL, NULL);
        return;
    }

    // malloc the stair list
    stairs = (stair*)malloc(stair_count * sizeof(stair));

    //handle malloc fail
    if (stairs == NULL)
    {
        log_issue(MEMORY_ALLOCATION_ERROR, STAIR, 1, NULL, NULL);
        return;
    }

    for (int stair_index = 0; stair_index < stair_count; stair_index++)
    {
        stairs[stair_index] = stair_list[stair_index];
    }
}


void get_valid_poles()
{
    pole_count = 0;
    FILE *f = fopen(POLES_TXT, "r");
    if (f == NULL)
    {
        log_issue(FILE_NOT_FOUND, POLE, 0, NULL, "No poles were added.");
        return;
    }

    char buff[100];
    pole pole_list[GAME_CELL_CAP];
    while (fgets(buff, sizeof(buff), f) != NULL)
    {
        //printf("Trying pole %s.\n", buff);
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
            log_issue(INVALID_FORMAT, POLE, 0, buff, "Pole was not added.");
            continue;
        }

        //check for pole validity
        if (start_floor == end_floor)
        {
            log_issue(INVALID_DEFINITION, POLE, 0, buff, "Pole was not added.");
            continue;
        }

        int top_floor = MAX(start_floor, end_floor);
        int bottom_floor = MIN(start_floor, end_floor);
        if (
            !cell_in_maze_bounds(start_floor, width, length) ||
            !cell_in_maze_bounds(end_floor, width, length)
        )
        {
            log_issue(OUT_OF_MAZE_BOUNDS, STAIR, 0, buff, "Pole was not added.");
            continue;
        }

        cell* top_cell = maze[top_floor][width][length];
        cell* bottom_cell = maze[bottom_floor][width][length];
        //bottom cell edge case first
        if (bottom_cell == NULL)
        {
            if (cell_in_start_area(bottom_floor, width, length))
            {
                bottom_cell = start_link_cell;
            }
            else
            {
                log_issue(DEFINED_IN_VOID, POLE, 0, buff, "Pole was not added.");
                continue;
            }
        }
        else if (bottom_cell->type == BAWANA)
        {
            bottom_cell = bawana_link_cell;
        }

        if (top_cell == NULL)
        {
            if (top_floor - bottom_floor == 2)
            {
                top_cell = maze[1][width][length];
                if (top_cell == NULL)
                {
                    log_issue(DEFINED_IN_VOID, POLE, 0, buff, "Pole was not added.");
                    continue;
                }
                log_issue(DEFINED_IN_VOID, POLE, 0, buff, "Attempting to add pole from floor 1.");
            }
            else
            {
                log_issue(DEFINED_IN_VOID, POLE, 0, buff, "Pole was not added.");
                continue;
            }
        }

        if (
            (top_cell->type != GAME && top_cell->type != STAIR && top_cell->type != POLE) ||
            (bottom_cell->type != GAME && bottom_cell->type != STAIR && bottom_cell->type != LINK_BAWANA && bottom_cell->type != LINK_START)
        )
        {
            log_issue(OBJECT_COLLISION, POLE, 0, buff, "Pole was not added.");
            continue;
        }

        pole to_add;

        to_add.bottom_cell = bottom_cell;
        to_add.top_cell = top_cell;

        //if middle cell exists, add that cell as a separete pole
        if (top_floor - bottom_floor == 2)
        {
            cell* middle_cell = maze[1][width][length];
            if (middle_cell != NULL && middle_cell->type != WALL)
            {
                to_add.middle_cell = middle_cell;
            }
        }

        pole_list[pole_count++] = to_add;
    }
    fclose(f);

    // handle no poles edge case
    if (pole_count < 1)
    {
        log_issue(NO_OBJECTS_ADDED, POLE, 0, NULL, "No poles were added.");
        return;
    }

    // malloc the pole list
    poles = (pole*)malloc(pole_count * sizeof(pole));

    //handle malloc fail
    if (poles == NULL)
    {
        log_issue(MEMORY_ALLOCATION_ERROR, POLE, 1, NULL, NULL);
        return;
    }

    for (int pole_index = 0; pole_index < pole_count; pole_index++)
    {
        poles[pole_index] = pole_list[pole_index];
    }

}


void add_valid_walls()
{
    FILE *f = fopen(WALLS_TXT, "r");
    if (f == NULL)
    {
        log_issue(FILE_NOT_FOUND, WALL, 0, NULL, "No walls were added.");
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
            log_issue(INVALID_FORMAT, WALL, 0, buff, "Wall was not added.");
            continue;
        }

        //check for wall validity, by definition
        if (
            !(start_width == end_width ||
            start_length == end_length)
        )
        {
            log_issue(INVALID_DEFINITION, WALL, 0, buff, "Wall was not added.");
        }

        int to_add = 1;
        if (start_width == end_width)
        {                
            int s_length = MIN(start_length, end_length);
            int l_length = MAX(start_length, end_length);
            //iterate over each wall cell, to ensure it is valid
            for (int i = s_length ; i <= l_length ; i++)
            {
                if (!cell_in_maze_bounds(floor, start_width, i))
                {
                    to_add = 0;
                    break;
                }
                else if (maze[floor][start_width][i]->type != GAME)
                {
                    to_add = 0;
                    break;
                }
            }
            if (!to_add)
            {
                log_issue(OUT_OF_MAZE_BOUNDS, WALL, 0, buff, "Wall was not added.");
                continue;
            }
            for (int i = s_length ; i <= l_length ; i++)
            {
                maze[floor][start_width][i]->type == WALL;
            }
        }
        else
        {
            int s_width = MIN(start_width, end_width);
            int l_width = MAX(start_width, end_width);
            //iterate over each wall cell, to ensure it is valid
            for (int i = s_width; i<= l_width; i++)
            {
                if (!cell_in_maze_bounds(floor, i, start_length))
                {
                    to_add = 0;
                    break;
                }
                else if (maze[floor][i][start_length]->type != GAME)
                {
                    to_add = 0;
                    break;
                }
            }
            if (!to_add)
            {
                log_issue(OBJECT_COLLISION, WALL, 0, buff, "Wall was not added.");
                continue;
            }
            for (int i = s_width; i<= l_width; i++)
            {
                maze[floor][i][start_length]->type = GAME;
            }
        }
    }
    fclose(f); 
}


//if flag is illegally defined, the game cannot start
//NOTE: this function should be called first
void add_flag()
{
    FILE *f = fopen(FLAG_TXT, "r");
    if (f == NULL)
    {
        log_issue(FILE_NOT_FOUND, FLAG, 1, NULL, "Quitting game.");
        return;
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
            log_issue(INVALID_FORMAT, FLAG, 1, buff, "Quitting game.");
            return;
        }

        if (!cell_in_maze_bounds(floor, width, length))
        {
            log_issue(OUT_OF_MAZE_BOUNDS, FLAG, 1, buff, "Quitting game.");
            return;
        }
        flag = maze[floor][width][length];
        if (flag == NULL)
        {
            log_issue(DEFINED_IN_VOID, FLAG, 1, buff, "Quitting game.");
            return;
        }
        else if (flag->type != GAME)
        {
            log_issue(OBJECT_COLLISION, FLAG, 1, buff, "Quitting game.");
            return;
        }
        flag->type = FLAG;
    }
    else
    {
        log_issue(INVALID_DEFINITION, FLAG, 1, NULL, "Quitting game.");
    }
    fclose(f);
    
}