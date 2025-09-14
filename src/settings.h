#ifndef SETTINGS_H
#define SETTINGS_H

#include "types.h"

#define FLOORS 3
#define WIDTH 10
#define LENGTH 25
#define TURN_CAP 10000000
#define NAME_LENGTH 11
#define NUMBER_OF_PLAYERS 3
#define GAME_CELL_CAP 500
#define STAIR_CAP 1000
#define START_MOVEMENT_POINTS 100
#define STAIR_DIRECTION_CHANGE_TURNS 5
#define BAWANA_START_WIDTH 7
#define BAWANA_START_LENGTH 21
#define BAWANA_CELL_COUNT 12
#define TRIGGER_FACTOR 2
#define WALL_HIT_DICE_COST 2
#define START_LINK_WIDTH 9
#define START_LINK_LENGTH 12
#define BAWANA_LINK_WIDTH 9
#define BAWANA_LINK_LENGTH 13
#define BAWANA_NAME_LENGTH 30
#define LOOP_LINK_WIDTH 9
#define LOOP_LINK_LENGTH 11
#define PATH_DEPTH 200
#define PATH_NAME_LENGTH 1000
#define POLE_MESSAGE_LENGTH 150
#define STAIR_MESSAGE_LENGTH 150
#define DIRECTION_LENGTH 20
#define UNREACHABLE_DISTANCE 10000
#define FILE_PATH_LENGTH 100

//colors to be used mainly for visualization only
#define COLOR_GAME "\x1B[37m" //white
#define COLOR_STAIR "\x1B[31m" //red
#define COLOR_POLE "\x1B[33m" //yellow
#define COLOR_WALL "\x1B[34m" //blue
#define COLOR_BAWANA "\x1B[32m" //green
#define COLOR_START "\x1B[35m" //magenta
#define COLOR_FLAG   "\x1B[36m" //cyan
#define RESET "\x1B[0m"

//movement point operation, distribution constants
//DIST_G -> distribution game cells
#define DIST_G_ZERO 0.25
#define DIST_G_CONSUMABLE_1_4 0.35
#define DIST_G_BONUS_1_2 0.25
#define DIST_G_BONUS_3_5 0.1
#define DIST_G_MULTIPLY 0.05

extern char* SEED_TXT;
extern char* STAIRS_TXT;
extern char* POLES_TXT;
extern char* WALLS_TXT;
extern char* FLAG_TXT;
extern char* LOG_TXT;

extern const int bawana_points[OPERATION_COUNT];
extern const int bawana_duration[OPERATION_COUNT];

#endif