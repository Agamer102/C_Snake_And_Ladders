#ifndef OUTPUTS_H
#define OUTPUTS_H
#include <stdio.h>
#include <string.h>
#include "helpers.h"
#include "settings.h"
#include "types.h"

void print_cell(cell *to_print);
char* sprint_cell(cell* to_print);
char* sprint_bawana_cell(CELL_OPERATION type);
void print_direction(DIRECTION dir);
char* sprint_direction(DIRECTION dir);
void print_bawana_cell(CELL_OPERATION op);
void print_maze();
void print_stairs();
void print_turn(player* p);
void print_start_no_6(player* p, unsigned char dice);
void print_start_rolled_6(player* p);

void print_food_poisoning_wears_off(player* p, CELL_OPERATION to_go);
void print_disoriented_wears_off(player* p);
void print_triggered_wears_off(player* p);
void print_pole_message(player* p, cell* pole_cell, cell* next_cell);
void print_stair_message(player* p, cell* stair_cell, cell* next_cell);
void print_ran_out_of_movement_points_message(player *p);
void print_effect_movement_message(player *p, unsigned char dice, DIRECTION dir);
void print_still_food_poisoned_message(player* p);
void print_movement_points_consumed_message(player *p, int dice, int cost);
void print_hit_wall_message(player *p, int dice);
void print_fell_to_loop_message(player *p);
void print_fell_to_start_message(player *p);
void print_fell_to_dead_message(player* p);
void print_fell_to_bawana_message(player *p);
void print_placed_in_bawana_message(player* p, CELL_OPERATION op, int food_poisoned);
void print_get_food_poisoning_message(player* p);
void print_get_disoriented_message(player* p);
void print_get_triggered_message(player* p);
void print_get_happy_message(player* p);
void print_get_bawana_points_message(player* p, int bonus);
void print_found_flag_message(player* p);
#endif