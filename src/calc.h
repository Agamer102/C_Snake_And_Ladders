#ifndef CALC_H
#define CALC_H
#include "settings.h"
#include "types.h"

void assign_movement_points();
void assign_bawana_cells();
void is_game_cell(cell* current_cell);
void get_game_block_array();
void fisher_yates(cell** array, int length);
void assign_to_range_array(
    cell** array, 
    CELL_OPERATION operation, 
    int op_range_start,
    int op_range_end,
    int start,
    int end
);

extern cell* maze[FLOORS][WIDTH][LENGTH];

#endif