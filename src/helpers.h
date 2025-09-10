#ifndef HELPERS_H
#define HELPERS_H
#include "settings.h"
#include "types.h"

int cell_in_maze_bounds(int floor, int width, int length);
void free_map();
void iterate_map(void (*function_to_call)(void*));
void fill_section(
    int floor, 
    int width_start, 
    int length_start,
    int width_end,
    int length_end,
    CELL_TYPE type
);
void fix_neighbours();
void fix_cell_neighbour(cell* current_cell);
void print_cell(cell *to_print);
void print_direction(DIRECTION dir);
void print_bawana_cell(CELL_OPERATION op);
void print_maze();
void fisher_yates(cell** array, int length);
void assign_to_range_array(
    cell** array, 
    CELL_OPERATION operation, 
    int op_range_start,
    int op_range_end,
    int start,
    int end
);
void bfs(cell* flag);

extern cell* maze[FLOORS][WIDTH][LENGTH];
extern player players[NUMBER_OF_PLAYERS];

extern stair* stairs;
extern int stair_count;

extern cell* bawana[BAWANA_CELL_COUNT];
extern cell* bawana_entrance;
extern const int bawana_points[OPERATION_COUNT];
extern const int bawana_duration[OPERATION_COUNT];

#endif