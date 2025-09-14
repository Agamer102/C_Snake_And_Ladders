#include "settings.h"

char* SEED_TXT = "../tests/data/bad/6/seed.txt";
char* STAIRS_TXT = "../tests/data/bad/6/stairs.txt";
char* POLES_TXT = "../tests/data/bad/6/poles.txt";
char* WALLS_TXT = "../tests/data/bad/6/walls.txt";
char* FLAG_TXT = "../tests/data/bad/6/flag.txt";
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