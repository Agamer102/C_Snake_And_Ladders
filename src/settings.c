#include "settings.h"

char* SEED_TXT = "./seed.txt";
char* STAIRS_TXT = "./stairs.txt";
char* POLES_TXT = "./poles.txt";
char* WALLS_TXT = "./walls.txt";
char* FLAG_TXT = "./flag.txt";
char* LOG_TXT = "./log.txt";

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
    0 //HAPPY
};