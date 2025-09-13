#ifndef INPUTS_H
#define INPUTS_H

#include <stdio.h>
#include "helpers.h"
#include "types.h"
#include "settings.h"

//from types
extern cell* maze[FLOORS][WIDTH][LENGTH];
extern int stair_count;
extern stair* stairs;
extern cell* flag;
extern pole* poles;
extern int pole_count;
extern cell* start_link_cell;
extern cell* bawana_link_cell;

void get_file_inputs();
void seed_rand_function();
void get_valid_stairs();
void get_valid_poles();
void add_valid_walls();
void add_flag();

#endif