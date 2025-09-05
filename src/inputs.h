#ifndef INPUTS_H
#define INPUTS_H

#include <stdio.h>
#include "types.h"

extern cell* maze[FLOORS][WIDTH][LENGTH];
extern int stair_count;

FILE* open_file(char* filename);
void seed_rand_function();

#endif