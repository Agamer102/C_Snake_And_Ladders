#ifndef TYPES_H
#define TYPES_H

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define NAME_FORMAT "[%1u, %1u, %02u]"
#define STAIR_FORMAT "[%1u, %1u, %2u, %1u, %1u, %2u]"
#define POLE_FORMAT "[%1u, %1u, %1u, %2u]"
#define WALL_FORMAT "[%1u, %1u, %2u, %1u, %2u]"
#define FLAG_FORMAT "[%1u, %1u, %2u]"

typedef enum
{
    FORCED,
    SECOND,
    NORTH,
    EAST,
    SOUTH,
    WEST,
    DIRECTION_COUNT
} DIRECTION;

typedef enum 
{
    GAME,
    START,
    WALL,
    POLE,
    STAIR,
    FLAG,
    BAWANA
} CELL_TYPE;

typedef enum
{
    ADD,
    MUL,
    FOOD_POISONING,
    DISORIENTED,
    TRIGGERED,
    HAPPY
} CELL_OPERATION;

typedef enum
{
    BIDIRECTIONAL,
    TOP_TO_BOTTOM,
    BOTTOM_TO_TOP
} STAIR_DIRECTION;

typedef enum
{
    SUCCESS,
    HIT_WALL,
    STAIR_POLE_LOOP,
    FELL_TO_START,
    FELL_TO_BAWANA
} MOVEMENT;

typedef struct cell
{
    unsigned char floor;
    unsigned char width;
    unsigned char length;
    CELL_TYPE type;
    CELL_OPERATION movement_point_operation;
    int movement_point_operand;
    struct cell* neighbours[DIRECTION_COUNT];
    int n1;
    int n2;
} cell;
//n1, n2 is set by default to -1 and are used
/*
GAME: -1
STAIR: stair#1, stair#2
POLE: -1
START: -1
WALL: -1
BAWANA: 
*/


typedef struct
{
    cell* start;
    cell* location;
    cell* from;
    DIRECTION start_direction;
    DIRECTION current_direction;
    int movement_points;
    unsigned char direction_dice;
    char name;
} player;

typedef struct 
{
    cell* start_cell;
    cell* end_cell;
    STAIR_DIRECTION direction;
} stair;

#endif