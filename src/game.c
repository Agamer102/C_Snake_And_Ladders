#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "calc.h"
#include "game.h"

const cell empty_cell =
{
    255,
    255,
    255,
    GAME,
    ADD,
    0,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    }
};

cell* maze[FLOORS][WIDTH][LENGTH] = {NULL};
player players[NUMBER_OF_PLAYERS];
int game_ticks = 0;

stair* stairs = NULL;
int stair_count = 0;


void game()
{
    //seed_rand();
    generate_map();
    initialize_players();
    get_file_inputs();
    assign_movement_points();
    printf("Mov: %i\n", maze[0][4][7]->movement_point_operand);
    do
    {
        turn(players + game_ticks % 3);
    } while(++game_ticks);
    free_map();
}


void turn(player* current_player)
{
    unsigned char dice = roll_dice();

    //handle starting area case
    if (current_player->location == current_player->start)
    {
        if (dice != 6)
        {
            printf(
                "%c is at the starting area and rolls %u on the movement dice cannot enter the maze.\n",
                current_player->name, dice
            );
        }
        else
        {
            current_player->location = current_player->start->neighbours[FORCED];
            current_player->current_direction = current_player->start_direction;
            printf(
                "%c is at the starting area and rolls 6 on the movement dice and is placed on ",
                current_player->name
            );
            print_cell(current_player->location);
            printf(" of the maze.\n");
        }
        return;
    }
    //BLOCKED TEMPORARILY
    return;

    //standard movement section

    //standard dice message
    printf("%c rolls and %u on the movement dice", current_player->name, dice);

    //check if direction dice must be rolled, once every 4 player ticks
    if (current_player->direction_dice == 3)
    {
        DIRECTION movement_dice = roll_dice();
        if (movement_dice != FORCED || movement_dice != DIRECTION_COUNT)
        {
            current_player->current_direction = movement_dice;
            printf(" and ");
            print_direction(movement_dice);
            printf(" on the direction dice, changes direction to ");
            print_direction(movement_dice);
        }
    }

    //set movement dice ticks
    //modulos to prevent value from exploding
    current_player->direction_dice = (current_player->direction_dice + 1) % 4;


}


void initialize_players()
{
    players[0] = (player){maze[0][6][12], maze[0][6][12], NORTH, NORTH, 100, 0, 'A'};
    players[1] = (player){maze[0][9][8], maze[0][9][8], WEST, WEST, 100, 0, 'B'};
    players[2] = (player){maze[0][9][16], maze[0][9][16], EAST, EAST, 100, 0, 'C'};

    //handle special case for maze starting cells
    maze[0][6][12]->neighbours[FORCED] = maze[0][5][12];
    maze[0][9][8]->neighbours[FORCED] = maze[0][9][7];
    maze[0][9][16]->neighbours[FORCED] = maze[0][9][17];
}


void generate_map()
{
    //ground floor
    fill_section(0, 0, 0, 9, 24, GAME);
    fill_section(0, 6, 8, 9, 16, START);
    fill_section(0, 6, 20, 9, 20, WALL);
    fill_section(0, 6, 21, 6, 24, WALL);
    fill_section(0, 7, 21, 9, 24, BAWANA);

    //middle floor
    fill_section(1, 0, 0, 9, 7, GAME);
    fill_section(1, 6, 8, 9, 16, GAME);
    fill_section(1, 0, 17, 9, 24, GAME);

    //top floor
    fill_section(2, 0, 8, 9, 16, GAME);

    fix_neighbours();
}


/*
This function, fills segment with uniform blocks of given type
to_fill here, defines a basic cell
*/
void fill_section(
    int floor, 
    int width_start, 
    int length_start,
    int width_end,
    int length_end,
    CELL_TYPE type
)
{
    for (int width = width_start; width <= width_end; width++)
    {
        for (int length = length_start; length <= length_end; length++)
        {
            if (!cell_in_maze_bounds(floor, width, length))
            {
                continue;
            }
            cell* to_fill = (cell*)malloc(sizeof(cell));
            if (to_fill == NULL)
            {
                puts("Memory allocation failed.");
                free_map();
                exit(-1);
            }

            *to_fill = empty_cell;
            to_fill->type = type;
            to_fill->floor = floor;
            to_fill->width = width;
            to_fill->length = length;
            maze[floor][width][length] = to_fill;
        }
    }
}


void fix_neighbours()
{
    iterate_map((void *)&fix_cell_neighbour);
}


void fix_cell_neighbour(cell* current_cell)
{
    for (DIRECTION d = NORTH; d < DIRECTION_COUNT; d++)
    {
        // -1 NORTH, +1 SOUTH +1 EAST -1 WEST
        int neighbour_width = current_cell->width - (d == NORTH) + (d == SOUTH);
        int neighbour_length = current_cell->length + (d == EAST) - (d == WEST);

        //check if cell is out of bounds
        if (
            !cell_in_maze_bounds(current_cell->floor, neighbour_width, neighbour_length) ||
            current_cell->type == START
        )
        {
            continue;
        }
    
        cell *neighbour = maze[current_cell->floor][neighbour_width][neighbour_length];
        //if a neighbour exists, add that neighbour
        if (neighbour != NULL)
        {
            current_cell->neighbours[d] = neighbour;
        }
    }
}


int cell_in_maze_bounds(int floor, int width, int length)
{
    return (
        (floor == 0 || floor == 1 || floor == 2) &&
        (width >= 0 && width < WIDTH) &&
        (length >= 0 && length < LENGTH)
    );
}


void free_map()
{
    iterate_map(&free);
}


//iterates over whole map, and calls a function on it
//will not call on NULL cell
void iterate_map(void (*function_to_call)(void*))
{
    for (int floor = 0; floor < FLOORS; floor++)
    {
        for (int width = 0; width < WIDTH; width++)
        {
            for (int length = 0; length < LENGTH; length++)
            {
                if (maze[floor][width][length] != NULL)
                {
                    function_to_call(maze[floor][width][length]);
                }
            }
        }
    }
}


// prints a given cell in expected output to std output
void print_cell(cell *to_print)
{
    printf(
        NAME_FORMAT, 
        (unsigned int)to_print->floor, 
        (unsigned int)to_print->width, 
        (unsigned int)to_print->length
    );
}


//prints a given direction
void print_direction(DIRECTION dir)
{
    switch (dir)
    {
        case NORTH:
            printf("North");
            break;
        case EAST:
            printf("East");
            break;
        case SOUTH:
            printf("South");
            break;
        case WEST:
            printf("West");
            break;
    }
}


unsigned char roll_dice()
{
    return (unsigned char) (rand() % 6 + 1);
}