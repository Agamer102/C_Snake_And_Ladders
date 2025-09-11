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


char* sprint_direction(DIRECTION dir)
{
    static char buff[DIRECTION_LENGTH];
    switch (dir)
    {
        case 0:
            break;
        case 1:
            break;
        case NORTH:
            strcpy(buff, "North direction");
            break;
        case EAST:
            strcpy(buff, "East directon");
            break;
        case SOUTH:
            strcpy(buff, "South direction");
            break;
        case WEST:
            strcpy(buff, "West direction");
            break;
        case 6:
            strcpy(buff, "Empty");
            break;
    }
    return buff;
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
                    case LINK_BAWANA:
                    case LINK_START:
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


void print_stairs()
{
    if (stairs == NULL) return;
    for (int i = 0; i < stair_count; i++)
    {
        stair s = stairs[i];
        puts("");
        print_cell(s.start_cell);
        switch (s.direction)
        {      
            case BIDIRECTIONAL:
                printf(" <-> ");
                break;
            case TOP_TO_BOTTOM:
                printf(" -> ");
                break;
            case BOTTOM_TO_TOP:
                printf(" <- ");
                break;
            
        }
        print_cell(s.end_cell);
        puts("");
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


void print_pole_message(player* p, cell* pole_cell, cell* next_cell)
{
    printf
    (
        "%c lands on %s which is a pole cell.\n %c slides down and now placed at %s in floor %u.\n",
        p->name, sprint_cell(pole_cell), p->name, sprint_cell(next_cell), next_cell->floor
    );
}


void print_stair_message(player* p, cell* stair_cell, cell* next_cell)
{
    printf
    (
        "%c lands on %s which is a stair cell. %c takes the stairs and now placed at %s in floor %i.\n",
        p->name, sprint_cell(stair_cell), p->name, sprint_cell(next_cell), next_cell->floor
    );
}


void print_ran_out_of_movement_points_message(player *p)
{
    printf 
    (
        "%c movement points are depleted and requires replenishment. Transporting to Bawana.",
        p->name
    );
}


void print_effect_movement_message(player *p, unsigned char dice, DIRECTION dir)
{
    switch (p->status_effect)
    {
        //normal case
        case ADD:
        case MUL:
            //returns direction if rolled, 0 if not and 6 if EMPTY
            if (dir == 0)
            {
                printf
                (
                    "%c rolls and %u on the movement dice and moves %s by %u cells and is now at %s.\n",
                    p->name, dice, sprint_direction(p->current_direction), dice, sprint_cell(p->location)
                );
            }
            else
            {
                printf
                (
                    "%c rolls and %u on the movement dice and %s on the direction dice, changes direction to %s and moves %u cells and is now at %s.\n",
                    p->name, dice, sprint_direction(dir), sprint_direction(p->current_direction), dice, sprint_cell(p->location)
                );
            }
            break;
        case TRIGGERED:
            if (dir == 0)
            {
                printf
                (
                    "%c is triggered and rolls and %u on the movement dice and move in the %s and moves %u cells and is placed at the %s.\n",
                    p->name, dice / 2, sprint_direction(p->current_direction), dice * 2, sprint_cell(p->location)
                );
            }
            else
            {
                printf
                (
                    "%c is triggered and rolls and %u on the movement dice and %s on the direction dice, changes direction to %s and move in the %s and moves %u cells and is placed at the %s.\n",
                    p->name, dice / 2, sprint_direction(dir), sprint_direction(p->current_direction), sprint_direction(p->current_direction), dice * 2, sprint_cell(p->location)
                );   
            }
        case DISORIENTED:
            printf
            (
                "%c rolls and %u on the movement dice and is disoriented and move in the %s and moves %u cells and is placed at the %s.\n",
                p->name, dice, sprint_direction(p->current_direction), dice, sprint_cell(p->location)
            );
            break;

    }
}


void print_movement_points_consumed_message(player *p, int dice, int cost)
{
    printf
    (
        "%c moved %u that cost %i movement points and is left with %u and is moving in the %s.",
        p->name, dice, cost, p->movement_points, sprint_direction(p->current_direction)
    );
}