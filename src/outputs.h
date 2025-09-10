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
void print_bawana_cell(CELL_OPERATION op);
void print_maze();
void print_turn(player* p);
void print_start_no_6(player* p, unsigned char dice);
void print_start_rolled_6(player* p);

void print_food_poisoning_wears_off(player* p, CELL_OPERATION to_go);
void print_disoriented_wears_off(player* p);
void print_triggered_wears_off(player* p);

#endif