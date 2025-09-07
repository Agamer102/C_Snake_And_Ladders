#ifndef GAME_H
#define GAME_H
#include "types.h"
#include "inputs.h"

void game();
void generate_map();
void turn(player* current_player);
void fill_section(
    int floor, 
    int width_start, 
    int length_start,
    int width_end,
    int length_end,
    CELL_TYPE type
);
void initialize_players();
void fix_neighbours();
void fix_cell_neighbour(cell* current_cell);
int cell_in_maze_bounds(int floor, int width, int length);
void free_map();
void iterate_map(void (*function_to_call)(void*));
void print_cell(cell *to_print);
void print_direction(DIRECTION dir);
unsigned char roll_dice();
#endif