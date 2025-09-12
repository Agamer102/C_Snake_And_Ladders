#ifndef HELPERS_H
#define HELPERS_H
#include "outputs.h"
#include "settings.h"
#include "types.h"

int cell_in_maze_bounds(int floor, int width, int length);
int cell_in_start_area(int floor, int width, int length);
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
char assign_to_forced_then_second(cell* assignee, cell* assignor, int index);
void remove_stair_from_cell(cell* receiver, cell* remove);
void assign_bfs_neighbours_trivial();
void assign_bfs_neighbour_trivial(cell* current_cell);
void assign_bfs_neighbours();
void assign_bfs_neighbour(cell* current_cell);
void assign_dead_cells();
void assign_dead_cell(cell* current_cell);
void reset_flag_distances();
void reset_flag_distance(cell* current_cell);
void clear_bfs_neighbours();
void clear_bfs_neighbour(cell* current_cell);
void reset_visited_cells();
void reset_visited_cell(cell* current_cell);
void quit_game_safely();

extern cell* maze[FLOORS][WIDTH][LENGTH];
extern player players[NUMBER_OF_PLAYERS];

extern stair* stairs;
extern int stair_count;

extern cell* bawana[BAWANA_CELL_COUNT];
extern cell* bawana_entrance;
extern const int bawana_points[OPERATION_COUNT];
extern const int bawana_duration[OPERATION_COUNT];

extern cell* loop_link_cell;
extern pole* poles;

#endif