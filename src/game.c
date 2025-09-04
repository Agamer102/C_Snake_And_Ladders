#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
int game_ticks = -1;

void game()
{
    seed_rand();
    generate_map();
    initialize_players();
    while (game_ticks++)
    {
        turn(players[game_ticks % 3]);
    }
    free_map();
}

void seed_rand()
{
    FILE *f = fopen("./inputs/seed.txt", "r");
    if (f == NULL)
    {
        puts("seed.txt not found, using random seed.");
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
    srand(seed);
}

void turn(player current_player)
{
    // tis about to get real
}

void initialize_players()
{
    players[0] = (player){maze[0][6][12], maze[0][6][12], NORTH, 100, 0, 'A'};
    players[1] = (player){maze[0][6][12], maze[0][9][8], WEST, 100, 0, 'B'};
    players[2] = (player){maze[0][6][12], maze[0][9][16], EAST, 100, 0, 'C'};
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
    fill_section(1, 6, 8, 8, 16, GAME);
    fill_section(1, 0, 17, 9, 24, GAME);

    //top floor
    fill_section(2, 0, 8, 9, 16, GAME);

    //fix neighbours
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
    for (int width = width_start; width < width_end; width++)
    {
        for (int length = length_start; length < length_end; length++)
        {
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
    for (direction d = NORTH; d < DIRECTION_COUNT; d++)
    {
        // -1 NORTH, +1 SOUTH +1 EAST -1 WEST
        int neighbour_width = current_cell->width - (d == NORTH) + (d == SOUTH);
        int neighbour_length = current_cell->length + (d == EAST) - (d == WEST);

        //check if cell is out of bounds
        if (
            neighbour_width > WIDTH ||
            neighbour_width < 0 || 
            neighbour_length > LENGTH ||
            neighbour_length < 0)
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

void free_map()
{
    iterate_map(&free);
}

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
        "[%1u, %1u, %02u]", 
        (unsigned int)to_print->floor, 
        (unsigned int)to_print->width, 
        (unsigned int)to_print->length
    );
}

int roll_dice()
{
    return rand() % 6 + 1;
}