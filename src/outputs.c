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
    return buff;
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
                    case LINK_LOOP:
                    case LINK_BAWANA:
                    case LINK_START:
                        printf(" L ");
                        break;
                    case GAME:
                        printf(COLOR_GAME " G " RESET);
                        break;
                    case START:
                        printf(COLOR_START " @ " RESET);
                        break;
                    case WALL:
                        printf(COLOR_WALL " W " RESET);
                        break;
                    case POLE:
                        printf(COLOR_POLE " P " RESET);
                        break;
                    case STAIR:
                        printf(COLOR_STAIR " S " RESET);
                        break;
                    case FLAG:
                        printf(COLOR_FLAG " F " RESET);
                        break;
                    case BAWANA:
                        printf(COLOR_BAWANA " B " RESET);
                        break;
                    case DEAD:
                        printf(" D ");
                        break;
                    default:
                        printf(" ? ");
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
        "%c is now fit to proceed from the food poisoning episode and now placed on a %s and the effects take place.\n",
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
    char pole_name[NAME_LENGTH];
    char next_name[NAME_LENGTH];
    strcpy(pole_name, sprint_cell(pole_cell));
    strcpy(next_name, sprint_cell(next_cell));
    printf
    (
        "%c lands on %s which is a pole cell.\n %c slides down and now placed at %s in floor %u.\n",
        p->name, pole_name, p->name, next_name, next_cell->floor
    );
}


void print_stair_message(player* p, cell* stair_cell, cell* next_cell)
{
    char stair_name[NAME_LENGTH];
    char next_name[NAME_LENGTH];
    strcpy(stair_name, sprint_cell(stair_cell));
    strcpy(next_name, sprint_cell(next_cell));
    printf
    (
        "%c lands on %s which is a stair cell. %c takes the stairs and now placed at %s in floor %i.\n",
        p->name, stair_name, p->name, next_name, next_cell->floor
    );
}


void print_ran_out_of_movement_points_message(player *p)
{
    printf 
    (
        "%c movement points are depleted and requires replenishment. Transporting to Bawana.\n",
        p->name
    );
}


void print_effect_movement_message(player *p, unsigned char dice, DIRECTION dir)
{
    char p_direction[DIRECTION_LENGTH];
    char dir_direction[DIRECTION_LENGTH];
    strcpy(p_direction, sprint_direction(p->current_direction));
    strcpy(dir_direction, sprint_direction(dir));
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
                    p->name, dice, p_direction, dice, sprint_cell(p->location)
                );
            }
            else
            {
                printf
                (
                    "%c rolls and %u on the movement dice and %s on the direction dice, changes direction to %s and moves %u cells and is now at %s.\n",
                    p->name, dice, dir_direction, p_direction, dice, sprint_cell(p->location)
                );
            }
            break;
        case TRIGGERED:
            if (dir == 0)
            {
                printf
                (
                    "%c is triggered and rolls and %u on the movement dice and move in the %s and moves %u cells and is placed at the %s.\n",
                    p->name, dice / 2, p_direction, dice * 2, sprint_cell(p->location)
                );
            }
            else
            {
                printf
                (
                    "%c is triggered and rolls and %u on the movement dice and %s on the direction dice, changes direction to %s and move in the %s and moves %u cells and is placed at the %s.\n",
                    p->name, dice / 2, dir_direction, p_direction, sprint_direction(p->current_direction), dice * 2, sprint_cell(p->location)
                );   
            }
            break;
        case DISORIENTED:
            printf
            (
                "%c rolls and %u on the movement dice and is disoriented and move in the %s and moves %u cells and is placed at the %s.\n",
                p->name, dice, p_direction, dice, sprint_cell(p->location)
            );
            break;

    }
}



void print_still_food_poisoned_message(player* p)
{
    printf
    (
        "%c is still food poisoned and misses the turn.\n",
        p->name
    );
}


void print_movement_points_consumed_message(player *p, int dice, int cost)
{
    printf
    (
        "%c moved %u that cost %i movement points and is left with %u and is moving in the %s.\n",
        p->name, dice, cost, p->movement_points, sprint_direction(p->current_direction)
    );
}

void print_hit_wall_message(player *p, int dice)
{
    printf
    (
        "%c rolls and %i on the movement dice and cannot move in the %s. Player remains at %s\n",
        p->name, dice, sprint_direction(p->current_direction), sprint_cell(p->location)
    );
}


void print_fell_to_loop_message(player *p)
{
    printf
    (
        "%c finds themselves in a stair/pole loop. %c is moved to starting area.\n",
        p->name, p->name
    );
}


void print_fell_to_start_message(player *p)
{
    printf
    (
        "%c has fallen to the Starting area.\n",
        p->name
    );
}


void print_fell_to_dead_message(player* p)
{
    printf
    (
        "%c finds themselves in a cell that cannot access the flag. %c is moved to the bawana entrance.\n",
        p->name, p->name
    );
}


void print_fell_to_bawana_message(player *p)
{
   printf
   (
        "%c has fallen to the Bawana area.\n", p->name
   );
}



void print_placed_in_bawana_message(player* p, CELL_OPERATION op, int food_poisoned)
{
    if (food_poisoned)
    {
        printf
        (
            "%c is now fit to proceed from the food poisoning episode and now placed on a %s and the effects take place.\n",
            p->name, sprint_bawana_cell(op)
        );
    }
    else
    {
        printf
        (
            "%c is place on a %s and effects take place.\n",
            p->name, sprint_bawana_cell(op)
        );
    }
}


void print_get_food_poisoning_message(player* p)
{
    printf
    (
        "%c eats from Bawana and have a bad case of food poisoning. Will need three rounds to recover.\n",
        p->name
    );
}


void print_get_disoriented_message(player* p)
{
    printf
    (
        "%c eats from Bawana and is disoriented and is placed at the entrance of Bawana with %i movement points.\n",
        p->name, bawana_points[DISORIENTED]
    );
}


void print_get_triggered_message(player* p)
{
    printf
    (
        "%c eats from Bawana and is triggered due to bad quality of food. %c is placed at the entrance of Bawana with %i movement points\n",
        p->name, p->name, bawana_points[TRIGGERED]
    );
}


void print_get_happy_message(player* p)
{
    printf
    (
        "%c eats from Bawana and is happy. %c is placed at the entrance of Bawana with %i movement points.\n",
        p->name, p->name, bawana_points[HAPPY]
    );
}


void print_get_bawana_points_message(player* p, int bonus)
{
    printf
    (
        "%c eats from Bawana and earns %i movement points and is placed at the %s.\n",
        p->name, bonus, sprint_cell(p->location)
    );
}


void print_found_flag_message(player* p)
{
    printf("%c finds the flag and wins !\n", p->name);
}