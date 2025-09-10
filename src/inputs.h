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

void get_file_inputs();
FILE* open_file(char* path);
void seed_rand_function();
int can_place_object(int floor, int width, int length, CELL_TYPE type);
void add_valid_stairs();
void add_valid_poles();
void add_valid_walls();
void add_flag();

#endif