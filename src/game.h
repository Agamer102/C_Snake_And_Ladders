#ifndef GAME_H
#define GAME_H
#include "types.h"
#include "inputs.h"

//colors to be used mainly for visualization only
#define COLOR_GAME "\x1B[37m" //white
#define COLOR_STAIR "\x1B[31m" //red
#define COLOR_POLE "\x1B[33m" //yellow
#define COLOR_WALL "\x1B[34m" //blue
#define COLOR_BAWANA "\x1B[32m" //green
#define COLOR_START "\x1B[35m" //magenta
#define COLOR_FLAG   "\x1B[36m" //cyan
#define RESET "\x1B[0m"

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
void print_maze();
unsigned char roll_dice();
#endif