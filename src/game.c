#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "calc.h"
#include "game.h"

const cell empty_cell =
{
    255,
    255,
    255,
    GAME,
    ADD,
    0,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    -1,
    -1
};

cell* maze[FLOORS][WIDTH][LENGTH] = {NULL};
player players[NUMBER_OF_PLAYERS];
int game_ticks = 0;

stair* stairs = NULL;
int stair_count = 0;

cell* bawana[BAWANA_CELL_COUNT];
cell* bawana_entrance;
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
    0 //HAPPY: NOTE: This is realistic
};


void game()
{
    //seed_rand();
    generate_map();
    initialize_players();
    get_file_inputs();
    assign_movement_points();
    assign_bawana_cells();
    print_maze();
    printf("NORTH OF [0, 0, 12]: %p\n", maze[0][0][12]->neighbours[NORTH]);
    do
    {
        turn(players + game_ticks % 3);
    } while(++game_ticks);
    free_map();
}


void turn(player* current_player)
{
    printf("\n\nPlayer: %c's turn.\n", current_player->name);
    unsigned char dice = roll_dice();

    //handle starting area case
    if (current_player->location == current_player->start)
    {
        if (dice != 6)
        {
            printf
            (
                "%c is at the starting area and rolls %u on the movement dice cannot enter the maze.\n",
                current_player->name, dice
            );
        }
        else
        {
            current_player->location = current_player->start->neighbours[FORCED];
            current_player->current_direction = current_player->start_direction;
            printf
            (
                "%c is at the starting area and rolls 6 on the movement dice and is placed on ",
                current_player->name
            );
            print_cell(current_player->location);
            printf(" of the maze.\n");
        }
        return;
    }

    if (current_player->status_duration < 1)
    {
        //for special output messages
        switch (current_player->status_effect)
        {
            case FOOD_POISONING:
                printf
                (
                    "%c is now fit to proceed from the food poisoning episode.\n",
                    current_player->name
                );
                break;
            case DISORIENTED:
                printf("%c has recovered from disorientation.\n", current_player->name);
                break;
        } 
        //ensuring proper setting for safety
        current_player->status_duration = 0;
        current_player->status_factor = 1;
        current_player->status_effect = ADD;
    }
    else
    {
        current_player->status_duration--;
    }

    //handle effects
    //ADD is default status effect
    switch (current_player->status_effect)
    {
        case FOOD_POISONING:
            //player is guarenteed to miss this turn
            printf("%c is still food poisoned and misses the turn.", current_player->name);
            return;
        case DISORIENTED:
            printf
            (
                "%c rolls and %u on the movement dice and is disoriented and move in the ",
                current_player->name, dice
            );
            DIRECTION direction = (rand() % 4) + 2;
            print_direction(direction);
            current_player->current_direction = direction;
            break;
        //ADD or TRIGGERED
        default:
            //standard movement section
            printf("%c ", current_player->name);
            if (current_player->status_effect == TRIGGERED)
            {
                printf("is triggered and ");
                dice *= current_player->status_factor;
            }
            printf("rolls and %u on the movement dice", dice);

            //check if direction dice must be rolled, once every 4 player ticks
            if (current_player->direction_dice == 3)
            {
                DIRECTION movement_dice = roll_dice();
                if (movement_dice != SECOND && movement_dice != DIRECTION_COUNT)
                {
                    //printf("\n\n %i \n\n", movement_dice);
                    current_player->current_direction = movement_dice;
                    printf(" and ");
                    print_direction(movement_dice);
                    printf(" on the direction dice, changes direction to ");
                    print_direction(movement_dice);
                    //printf(".\n");
                }
                else
                {
                    printf(" and Empty on the direction dice, does not change direction");
                }
            }
            else
            {
                //printf(".\n");
            }
            break;
            
    }
    
    //tick direction dice 
    current_player->direction_dice = (current_player->direction_dice + 1) % 4;
    //printf("DIRDICE: %u", current_player->direction_dice);

    DIRECTION dir = current_player->current_direction;
    cell* move = current_player->location;
    int movement_points_to_add = 0;
    int movement_point_factor = 1;

    MOVEMENT status = SUCCESS;
    for (int i = dice; i > 0; i--)
    {
        if (status != SUCCESS)
        {
            break;
        }
        switch (move->type)
        {
            case GAME:
                //update movement points
                if (move->movement_point_operation == ADD)
                {
                    movement_points_to_add += move->movement_point_operand;
                }
                else if (move->movement_point_operation == MUL)
                {
                    movement_point_factor *= move->movement_point_operand;
                }
                move = move->neighbours[dir];
                if (move == NULL)
                {
                    status = HIT_WALL;
                }

                break;
            case POLE:
                //only true on pole activation cell
                move = follow_the_path(move);
                if (move == NULL)
                {
                    status = STAIR_POLE_LOOP;
                }
                break;
            case STAIR:
                //don't take stair already taken on first dice roll
                if (i == dice && move->neighbours[FORCED] == current_player->from)
                {
                    if (move->neighbours[SECOND] == NULL)
                    {
                        move = move->neighbours[dir];
                        if (move == NULL)
                        {
                            status = HIT_WALL;
                        }
                    }
                    else
                    {
                        move = follow_the_path(move->neighbours[SECOND]);
                    }          
                }
                else
                {
                    move = follow_the_path(move);
                }
                
                if (move == NULL)
                {
                    status = STAIR_POLE_LOOP;
                }
                //print_cell(move);
                break;
            case START:
                //edge case, check whether fall to start or barrier
                if (move->floor == 0)
                {
                    status = HIT_WALL;
                }
                else
                {
                    //fall to start
                    move = current_player->start;
                    status = FELL_TO_START;
                    return;
                }
                break;
            case BAWANA:
                status = FELL_TO_BAWANA;
                break;
            case FLAG:
                //TODO: flag capture 
                flag_found(current_player);
                break;
            case WALL:
                status = HIT_WALL;
        }
    }
    //handle movement points once more
    int movement_point_cost = movement_points_to_add * movement_point_factor;
    if (status == SUCCESS)
    {
        int net_movement_points = 
            current_player->movement_points - movement_point_cost;
        if (net_movement_points <= 0)
        {
            current_player->movement_points = 0;
            status = RAN_OUT_OF_MOVEMENT_POINTS;
        }
        else
        {
            current_player->movement_points = net_movement_points;
        }
    }
    //wall hit movement logic and edge case
    else if (status == HIT_WALL)
    {
        if ( (current_player->movement_points) - WALL_HIT_DICE_COST <= 0)
        {
            current_player->movement_points = 0;
            status = RAN_OUT_OF_MOVEMENT_POINTS;
        }
        else
        {
            current_player->movement_points -= WALL_HIT_DICE_COST;
        }
    }

    
    switch (status)
    {
        case SUCCESS:
            //we can now move the player
            current_player->from = current_player->location;
            current_player->location = move;
            printf(" and moves ");
            if(current_player->direction_dice == 0)
            {
                print_direction(current_player->current_direction);
                printf(" by ");
            }
            printf("%i cells and is now at ", dice);
            print_cell(move);
            printf(".\n");

            //movement point message
            printf
            (
                "%c moved %u that cost %i movement points and is left with %i and is moving in the ",
                current_player->name, dice, movement_point_cost, current_player->movement_points
            );
            print_direction(current_player->current_direction);
            printf(".\n");
            break;
        case HIT_WALL:
            //hit wall special message
            printf(" and cannot move in the ");
            print_direction(current_player->current_direction);
            printf(". Player remains at ");
            print_cell(current_player->location);
            printf("\n");
            break;
        case RAN_OUT_OF_MOVEMENT_POINTS:
            //terminate previous message assuming player completes movement
            printf(" and moves ");
            if(current_player->direction_dice == 0)
            {
                print_direction(current_player->current_direction);
                printf(" by ");
            }
            
            //WARNING MOVE MAY BE NULL HERE, CHECK
            //This happens when player has been staring at a wall
            //and runs out of movement points
            if (move == NULL)
            {
                printf("0 cells and is now at ");
                print_cell(current_player->location);
            }
            else
            {
                printf("%i cells and is now at ", dice);
                print_cell(move);
            }
            printf(".\n");
            printf
            (
                "%c movement points are depleted and requires replenishment. Transporting to Bawana.\n",
                current_player->name
            );
            bawana_entrance_logic(current_player);
            break;
        case FELL_TO_BAWANA:
            //also move to bawana with diff output msg
            //identical to when player needs to be moved to another bawana cell
            printf(".\n");
            bawana_entrance_logic(current_player);
            break;
        case FELL_TO_START:          
        //TODO: Fell to start temp fix here
            current_player->location = current_player->start;
            break;
    }
    
}

/*
Follows all stairs and poles from given cell, and returns
final cell*. Returns NULL if loop is detected
*/
cell* follow_the_path(cell* start)
{
    cell* visited[GAME_CELL_CAP];
    int n = 0;

    cell* next = start;
    while (1)
    {
        //check for loop condition 
        for (int i = 0; i < n; i++)
        {
            if (next == visited[i])
            {
                return NULL;
            }
        }
        visited[n++] = next;
        //print_cell(next);

        //move to the next node
        
        //check if we are now no longer in POLE or STAIR loop
        if (next->type != POLE && next->type != STAIR)
        {
            return next;
        }

        cell *forced = next->neighbours[FORCED];
        cell* second = next->neighbours[SECOND];

        //pole case
        if (next->type == POLE)
        {
            //forced cannot be NULL here, by definition
            next = forced;
            continue;
        }

        //only remaining case is the stair case

        //check for no valid stairs case
        if (forced == NULL && second == NULL)
        {
            return next;
        }

        //if we haven't visited forced, visit it
        int v = 0;

        //we MUST check for NULL case here, otherwise causes
        //undefined behaviour
        if (forced != NULL)
        {
            for (int i = 0; i < n; i++)
            {
                if (forced == visited[i])
                {
                    v = 1;
                }
            }
            if (v == 0)
            {
                next = forced;
                continue;
            }
            //edge case, no second and visited forced
            if (second == NULL)
            {
                return next;
            }
        }

        
        v = 0;
        //now check second
        //we MUST check for NULL case here, otherwise causes
        //undefined behaviour
        if (second != NULL)
        {
            for (int i = 0; i < n; i++)
            {
                if (second == visited[i])
                {
                    v = 1;
                }
            }
            if (v == 0)
            {
                next = second;
                continue;
            }
            if (forced == NULL)
            {
                return next;
            }
        }

        //terminal condition ?


        //if we made it to this point, we have also detected a loop
        return NULL;
    }
}


void flag_found(player* winner)
{
    printf(".\n%c finds the flag and wins !", winner->name);
    free_map();
    free(stairs);
    exit(0);
}


void bawana_entrance_logic(player* current_player)
{
    //select a random cell of bawana
    cell* bawana_landing = bawana[rand() % BAWANA_CELL_COUNT];

    //at bawana output
    printf("%c is place on a ", current_player->name);
    print_bawana_cell(bawana_landing->movement_point_operation);
    printf(" and effects take place.\n");

    //default non ADD case is processed here
    current_player->location = bawana_landing;
    current_player->status_effect = bawana_landing->movement_point_operation;
    current_player->status_duration = bawana_duration[bawana_landing->movement_point_operation];

    //process effects
    switch (bawana_landing->movement_point_operation)
    {
        //ADD is special case here
        case ADD:
            printf
            ("%c eats from Bawana and earns %u movement points and is placed at the ",
                current_player->name, bawana_landing->movement_point_operand
            );
            print_cell(current_player->location);
            printf(".\n");
            current_player->movement_points += bawana_landing->movement_point_operand;
            break;
        case FOOD_POISONING:
            printf
            (
                "%c eats from Bawana and have a bad case of food poisoning. Will need three rounds to recover.\n",
                current_player->name
            );
            break;
        case DISORIENTED:
            printf
            (
                "%c eats from Bawana and is disoriented and is placed at the entrance of Bawana with %i movement points.\n",
                current_player->name, bawana_points[bawana_landing->movement_point_operation]
            );
            place_at_bawana_entrance(current_player);
            break;
        case TRIGGERED:
            printf
            (
                "%c eats from Bawana and is triggered due to bad quality of food. %c is placed at the entrance of Bawana with %i movement points.\n",
                current_player->name, current_player->name, bawana_points[bawana_landing->movement_point_operation]
            );
            current_player->status_factor *= TRIGGER_FACTOR;
            place_at_bawana_entrance(current_player);
            break;
        case HAPPY:
            printf
            (
                "%c eats from Bawana and is happy. %c is placed at the entrance of Bawana with %i movement points.\n",
                current_player->name, current_player->name, bawana_points[bawana_landing->movement_point_operation]
            );
            place_at_bawana_entrance(current_player);
            break;

    }
}


void place_at_bawana_entrance(player* current_player)
{
    current_player->location = bawana_entrance;
    current_player->current_direction = NORTH;
}


void initialize_players()
{
    player player_base;
    player_base.status_effect = ADD;
    player_base.status_duration = 0;
    player_base.status_factor = 1;
    player_base.direction_dice = 0;
    player_base.movement_points = START_MOVEMENT_POINTS;
    players[0] = players[1] = players[2] = player_base;

    //A
    players[0].start = players[0].location = players[0].from = maze[0][6][12];
    players[0].start_direction = players[0].current_direction = NORTH;
    players[0].name = 'A';

    //B
    players[1].start = players[1].location = players[1].from = maze[0][9][8];
    players[1].start_direction = players[1].current_direction = WEST;
    players[1].name = 'B';

    //C
    players[2].start = players[2].location = players[2].from = maze[0][9][16];
    players[2].start_direction = players[2].current_direction = EAST;
    players[2].name = 'C';

    //handle special case for maze starting cells
    maze[0][6][12]->neighbours[FORCED] = maze[0][5][12];
    maze[0][9][8]->neighbours[FORCED] = maze[0][9][7];
    maze[0][9][16]->neighbours[FORCED] = maze[0][9][17];
}


void generate_map()
{
    //ground floor
    fill_section(0, 0, 0, 9, 24, GAME);
    fill_section(0, 6, 8, 9, 16, START);
    fill_section(0, 6, 20, 9, 20, WALL);
    fill_section(0, 6, 21, 6, 24, WALL);
    fill_section(0, 7, 21, 9, 24, BAWANA);
    //fix bawana entrance
    bawana_entrance = maze[0][9][19];

    //middle floor
    fill_section(1, 0, 0, 9, 7, GAME);
    fill_section(1, 6, 8, 9, 16, GAME);
    fill_section(1, 0, 17, 9, 24, GAME);

    //top floor
    fill_section(2, 0, 8, 9, 16, GAME);

    fix_neighbours();
}


/*
This function, fills segment with uniform blocks of given type
to_fill here, defines a basic cell
*/
void fill_section(
    int floor, 
    int width_start, 
    int length_start,
    int width_end,
    int length_end,
    CELL_TYPE type
)
{
    for (int width = width_start; width <= width_end; width++)
    {
        for (int length = length_start; length <= length_end; length++)
        {
            if (!cell_in_maze_bounds(floor, width, length))
            {
                continue;
            }
            cell* to_fill = (cell*)malloc(sizeof(cell));
            if (to_fill == NULL)
            {
                puts("Memory allocation failed.");
                free_map();
                exit(-1);
            }

            *to_fill = empty_cell;
            to_fill->type = type;
            to_fill->floor = floor;
            to_fill->width = width;
            to_fill->length = length;
            maze[floor][width][length] = to_fill;
        }
    }
}


void fix_neighbours()
{
    iterate_map((void *)&fix_cell_neighbour);
}


void fix_cell_neighbour(cell* current_cell)
{
    if (
        current_cell->type == START ||
        current_cell->type == WALL ||
        current_cell->type == BAWANA
    )
    {
        return;
    }
    for (DIRECTION d = NORTH; d < DIRECTION_COUNT; d++)
    {
        // -1 NORTH, +1 SOUTH +1 EAST -1 WEST
        int neighbour_width = current_cell->width - (d == NORTH) + (d == SOUTH);
        int neighbour_length = current_cell->length + (d == EAST) - (d == WEST);

        //check if cell is out of bounds
        if (!cell_in_maze_bounds(current_cell->floor, neighbour_width, neighbour_length))
        {
            continue;
        }
    
        cell *neighbour = maze[current_cell->floor][neighbour_width][neighbour_length];
        //if a neighbour exists and is valid, add that neighbour
        if (
            neighbour != NULL &&
            neighbour->type != START &&
            neighbour->type != WALL &&
            neighbour->type != BAWANA 
        )
        {
            current_cell->neighbours[d] = neighbour;
        }
    }
}


int cell_in_maze_bounds(int floor, int width, int length)
{
    if (floor < 0 || floor >= FLOORS) return 0;
    if (width < 0 || width >= WIDTH) return 0;
    if (length < 0 || length >= LENGTH) return 0;
    return 1;
}


void free_map()
{
    iterate_map(&free);
}


//iterates over whole map, and calls a function on it
//will not call on NULL cell
void iterate_map(void (*function_to_call)(void*))
{
    for (int floor = 0; floor < FLOORS; floor++)
    {
        for (int width = 0; width < WIDTH; width++)
        {
            for (int length = 0; length < LENGTH; length++)
            {
                if (maze[floor][width][length] != NULL)
                {
                    function_to_call(maze[floor][width][length]);
                }
            }
        }
    }
}


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
                        printf(COLOR_GAME " %i " RESET, current_cell->movement_point_operand);
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
                        printf(COLOR_BAWANA " %i " RESET, current_cell->movement_point_operand);
                        break;
                }
            }
            printf("\n");
        }
    }
}


unsigned char roll_dice()
{
    return (unsigned char) (rand() % 6 + 1);
}