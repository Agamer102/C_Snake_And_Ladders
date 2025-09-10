#include "outputs.h"

// prints a given cell in expected output to std output
void print_cell(cell *to_print)
{
    //printf("Trying to print cell %p\n", to_print);
    printf(
        NAME_FORMAT, 
        (unsigned int)to_print->floor, 
        (unsigned int)to_print->width, 
        (unsigned int)to_print->length
    );
}


char* sprint_cell(cell* to_print)
{
    static char buff[NAME_LENGTH];
    snprintf(buff, sizeof(buff), NAME_FORMAT, to_print->floor, to_print->width, to_print->length);
    return buff;
}


//prints a given direction
void print_direction(DIRECTION dir)
{
    switch (dir)
    {
        case NORTH:
            printf("North direction");
            break;
        case EAST:
            printf("East direction");
            break;
        case SOUTH:
            printf("South direction");
            break;
        case WEST:
            printf("West direction");
            break;
    }
}


void print_bawana_cell(CELL_OPERATION op)
{
    switch (op)
    {
        case ADD:
            printf("Movement Point Cell");
            break;
        case FOOD_POISONING:
            printf("Food Poisoning Cell");
            break;
        case DISORIENTED:
            printf("Disoriented Cell");
            break;
        case TRIGGERED:
            printf("Triggered Cell");
            break;
        case HAPPY:
            printf("Happy Cell");
            break;
    }
}

char* sprint_bawana_cell(CELL_OPERATION type)
{
    static char buff[BAWANA_NAME_LENGTH];
    switch (type)
    {
        case ADD:
            strcpy(buff, "Movement Point Cell");
            break;
        case FOOD_POISONING:
            strcpy(buff, "Food Poisoning Cell");
            break;
        case DISORIENTED:
            strcpy(buff, "Disoriented Cell");
            break;
        case TRIGGERED:
            strcpy(buff, "Triggered Cell");
            break;
        case HAPPY:
            strcpy(buff, "Happy Cell");
            break;
    }
}


void print_maze()
{
    for (int floor = 0; floor < FLOORS; floor++)
    {
        printf("Floor %i\n\n", floor);
        for (int width = 0; width < WIDTH; width++)
        {
            for (int length = 0; length < LENGTH; length++)
            {
                cell* current_cell = maze[floor][width][length];
                if (current_cell == NULL)
                {
                    printf("   ");
                    continue;
                }

                switch (current_cell->type)
                {
                    case GAME:
                        printf(COLOR_GAME " %i " RESET, current_cell->distance_to_flag);
                        break;
                    case START:
                        printf(COLOR_START " @ " RESET);
                        break;
                    case WALL:
                        printf(COLOR_WALL " W " RESET);
                        break;
                    case POLE:
                        printf(COLOR_POLE " %i " RESET, current_cell->distance_to_flag);
                        break;
                    case STAIR:
                        printf(COLOR_STAIR " %i " RESET, current_cell->distance_to_flag);
                        break;
                    case FLAG:
                        printf(COLOR_FLAG " %i " RESET, current_cell->distance_to_flag);
                        break;
                    case BAWANA:
                        printf(COLOR_BAWANA " %i " RESET, current_cell->distance_to_flag);
                        break;
                }
            }
            printf("\n");
        }
    }
}

void print_turn(player* p)
{
    printf
    (
        "============= Player %c's Turn =============\n", p->name
    );
}

//for start, simple output
void print_start_no_6(player* p, unsigned char dice)
{
    printf
    (
        "%c is at the starting area and rolls %u on the movement dice cannot enter the maze.\n",
        p->name, dice
    );
}


void print_start_rolled_6(player* p)
{
    printf
    (
        "%c is at the starting area and rolls 6 on the movement dice and is placed on %s of the maze.\n",
        p->name, sprint_cell(p->location)
    );
}


void print_food_poisoning_wears_off(player* p, CELL_OPERATION to_go)
{
    printf
    (
        "%c is now fit to proceed from the food poisoning episode and now placed on a %s and the effects take place.",
        p->name, sprint_bawana_cell(to_go)
    );
}


void print_disoriented_wears_off(player* p)
{
    printf("%c has recovered from disorientation.\n", p->name);
}


void print_triggered_wears_off(player* p)
{
    printf("%c is no longer triggered.\n", p->name);
}