#ifndef SETTINGS_H
#define SETTINGS_H

#define FLOORS 3
#define WIDTH 10
#define LENGTH 25
#define NAME_LENGTH 11
#define NUMBER_OF_PLAYERS 3
#define GAME_CELL_CAP 500

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

#endif