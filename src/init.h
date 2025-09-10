#ifndef INIT_H
#define INIT_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "helpers.h"
#include "inputs.h"
#include "outputs.h"
#include "types.h"
#include "settings.h"

void init();
void generate_map();
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
void get_distances_to_flag();
void reverse_poles();
void undo_reverse_poles();
void assign_movement_points();
void assign_bawana_cells();
void is_game_cell(cell* current_cell);
void get_game_block_array();

extern cell* maze[FLOORS][WIDTH][LENGTH];
extern player players[NUMBER_OF_PLAYERS];
extern cell* bawana_entrance;
extern int stair_count;
extern stair* stairs;
extern cell* flag;
extern pole* poles;
extern int pole_count;

#endif