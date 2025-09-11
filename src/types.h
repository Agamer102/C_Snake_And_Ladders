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
    BAWANA,
    LINK_START,
    LINK_BAWANA,
    LINK_LOOP,
    DEAD,
} CELL_TYPE;

typedef enum
{
    ADD,
    MUL,
    FOOD_POISONING,
    DISORIENTED,
    TRIGGERED,
    HAPPY,
    OPERATION_COUNT
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
    FOUND_FLAG,
    HIT_WALL,
    FELL_TO_LOOP,
    FELL_TO_START,
    FELL_TO_BAWANA,
    FELL_TO_DEAD,
    RAN_OUT_OF_MOVEMENT_POINTS
} MOVEMENT;

typedef struct cell
{
    unsigned char floor;
    unsigned char width;
    unsigned char length;
    CELL_TYPE type;
    CELL_OPERATION movement_point_operation;
    char movement_point_operand;
    struct cell* neighbours[DIRECTION_COUNT]; //best neighbour will be FORCED
    struct cell* bfs_neighbours[2]; //FORCED and SECOND only
    int n1;
    int n2;
    int distance_to_flag;
    char visited;
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
    MOVEMENT move_result;
    cell* moved_to;
} movement_packet;


typedef struct
{
    cell* start;
    cell* location;
    cell* from;
    DIRECTION start_direction;
    DIRECTION current_direction;
    CELL_OPERATION status_effect;
    unsigned char status_duration;
    unsigned int status_factor;
    unsigned int movement_points;
    unsigned char direction_dice;
    char name;
} player;

typedef struct 
{
    cell* start_cell; //top cell
    cell* end_cell; //bottom cell
    STAIR_DIRECTION direction;
} stair;

typedef struct
{
    cell* top_cell;
    cell* middle_cell;
    cell* bottom_cell;
} pole;

typedef struct 
{
    cell* deque[DEQUE_CAP];
    int front, back;
} deque;


#endif