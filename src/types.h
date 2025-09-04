#ifndef TYPES_H
#define TYPES_H

#define FLOORS 3
#define WIDTH 10
#define LENGTH 25
#define NAME_LENGTH 11
#define NUMBER_OF_PLAYERS 3
#define NAME_FORMAT "[%c, %c, %c%c]"

typedef enum
{
    FORCED,
    SECOND,
    NORTH,
    EAST,
    SOUTH,
    WEST,
    DIRECTION_COUNT
} direction;

typedef enum 
{
    GAME,
    START,
    WALL,
    POLE,
    STAIR,
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

typedef struct cell
{
    unsigned char floor;
    unsigned char width;
    unsigned char length;
    CELL_TYPE type;
    CELL_OPERATION operation;
    char movement_point_operand;
    struct cell* neighbours[DIRECTION_COUNT];
} cell;

typedef struct
{
    cell* start;
    cell* location;
    direction current_direction;
    int movement_points;
    char direction_dice;
    char name;
} player;
#endif