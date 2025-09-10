#include <stdio.h>
#include <stdlib.h>
#include "init.h"

cell* maze[FLOORS][WIDTH][LENGTH] = {NULL};
player players[NUMBER_OF_PLAYERS];
int game_ticks = 0;

stair* stairs = NULL;
int stair_count = 0;

pole* poles = NULL;
int pole_count = 0;

cell* bawana[BAWANA_CELL_COUNT];
cell* bawana_entrance;
const int bawana_points[OPERATION_COUNT] =
{
    0, //ADD
    0, //MUL: INVALID
    0, //FOOD_POISONING
    50, //DISORIENTED
    50, //TRIGGERED
    200 //HAPPY
};
const int bawana_duration[OPERATION_COUNT] = 
{
    0, //ADD
    0, //MUL: INVALID
    3, //FOOD_POISONING
    4, //DISORIENTED
    4, //TRIGGERED
    0 //HAPPY: NOTE: This is realistic
};

cell* flag = NULL;

void play_game()
{
    init();
    //now we need pathfinding
}