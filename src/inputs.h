#ifndef INPUTS_H
#define INPUTS_H

#include <stdio.h>
#include "types.h"

extern cell* maze[FLOORS][WIDTH][LENGTH];
extern int stair_count;
extern stair* stairs;

void get_file_inputs();
FILE* open_file(char* filename);
void seed_rand_function();
int can_place_object(int floor, int width, int length, CELL_TYPE type);
stair* add_valid_stairs();
void add_valid_poles();
void add_valid_walls();
void add_flag();

#endif