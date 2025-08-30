#define FLOORS 3
#define WIDTH 10
#define LENGTH 25
#define NAME_LENGTH 11

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
    VOID,
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
    char name[NAME_LENGTH];
    CELL_TYPE type;
    CELL_OPERATION operation;
    char movement_point_operand;
    struct cell* neighbours[DIRECTION_COUNT];
} cell;

const cell empty_cell =
{
    "[E, E, EE]",
    VOID,
    ADD,
    0,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    }
};