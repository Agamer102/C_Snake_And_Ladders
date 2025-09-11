#include <stdio.h>
#include <stdlib.h>
#include "game.h"

cell* maze[FLOORS][WIDTH][LENGTH] = {NULL};
player players[NUMBER_OF_PLAYERS];
int game_ticks = 0;

stair* stairs = NULL;
int stair_count = 0;

pole* poles = NULL;
int pole_count = 0;

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

cell* flag = NULL;

cell* start_link_cell = NULL;
cell* bawana_link_cell = NULL;
cell* loop_link_cell = NULL;

void play_game()
{
    init();
    do
    {
        turn(players + (game_ticks % 3));
        if (game_ticks % STAIR_DIRECTION_CHANGE_TURNS == STAIR_DIRECTION_CHANGE_TURNS - 1) 
        {
            reset_flag_distances();
            reset_visited_cells();
            clear_bfs_neighbours();
            randomize_stair_direction();
            add_poles_from_list();
            assign_bfs_neighbours();
            bfs(flag);
            print_maze();
            print_stairs();
        }
    } while (++game_ticks);
    
}


void randomize_stair_direction()
{
    if (stair_count < 1)
    {
        return;
    }

    for (int i = 0; i < stair_count; i++)
    {
        stairs[i].direction = rand() % 3;
        cell* start = stairs[i].start_cell;
        cell* end = stairs[i].end_cell;
        switch (stairs[i].direction)
        {
            case BIDIRECTIONAL:
                //note because of validation, this cannot fail
                assign_to_forced_then_second(start, end, i);
                assign_to_forced_then_second(end, start, i);
                break;
            case TOP_TO_BOTTOM:
                assign_to_forced_then_second(start, end, i);
                remove_stair_from_cell(end, start);
                break;
            case BOTTOM_TO_TOP:
                remove_stair_from_cell(start, end);
                assign_to_forced_then_second(end, start, i);
                break;
        }
    }
}


void turn(player* current_player)
{
    //print_turn(current_player);
    unsigned char dice = 0;
    DIRECTION direction_dice = 0;

    //effects tick, but have no actual effect when player in starting
    handle_effect_duration(current_player);

    //food poisoning is special, needs to be handled seperately
    if(current_player->status_effect == FOOD_POISONING) return food_poisoning_logic(current_player);

    //handle start case
    if (current_player->location == current_player->start)
    {
        dice = roll_dice();
        if (dice == 6)
        {
            //move player to their starting game square
            current_player->location = current_player->start->neighbours[FORCED];

            //print AFTER
            print_start_rolled_6(current_player);
        }
        else
        {
            print_start_no_6(current_player, dice);
        }
        //end of starting logic block
        return;
    }

    //temp block
    return;

    

    //first handle the dice
    switch (current_player->status_effect)
    {
        //default movement
        case ADD:
        case MUL:
            dice = roll_dice();
            direction_dice = roll_direction_dice_for(current_player);
            break;
        case DISORIENTED:
            dice = roll_dice();
            roll_direction_dice_for(current_player);
            //roll a new random direction
            direction_dice = rand() % 4 + NORTH;
            current_player->current_direction = direction_dice;
            break;
        case TRIGGERED:
            dice = roll_dice() * current_player->status_factor; //handle it here
            direction_dice = roll_direction_dice_for(current_player);
            break;
    }

    //master function, handles the rest
    return handle_effect_movement(current_player, dice, direction_dice);
}


void food_poisoning_logic(player* current_player)
{
    //TODO
}


//ONLY disoriented and triggered
void handle_effect_duration(player* current_player)
{
    int effect_duration = current_player->status_duration;
    CELL_OPERATION effect = current_player->status_effect;

    if (effect_duration < 1)
    {
        current_player->status_effect = ADD;
        current_player->status_duration = 0;
        //effect wears off
        switch (effect)
        {
            case DISORIENTED:
                print_disoriented_wears_off(current_player);
            case TRIGGERED:
                print_triggered_wears_off(current_player);
        }
    }
    else
    {
        current_player->status_duration--;
    }
}


//ONLY disoriented and triggered and normal to consider
void handle_effect_movement(player* current_player, unsigned char dice, DIRECTION direction_dice)
{
    int dice_m = dice;
    DIRECTION dir = direction_dice;
    cell* next = current_player->location;

    int movement_point_sum = 0;
    int movement_point_factor = 1;

    MOVEMENT status = SUCCESS;
    while (dice_m > 0 && status == SUCCESS)
    {
        //only game, stair and pole,
        //special logic is handled later
        switch (next->type)
        {
            case GAME:
                next = next->neighbours[dir];
                status = move_to_game(next);
                if (status == SUCCESS)
                {
                    if (next->movement_point_operation == ADD)
                    {
                        movement_point_sum += next->movement_point_operand;
                    }
                    else if (next->movement_point_operation == MUL)
                    {
                        movement_point_factor *= next->movement_point_operand;
                    }
                }
                dice_m--;
                break;
            case STAIR:
            case POLE:
                movement_packet mov = move_from_stair_pole(current_player, next);
                status = mov.move_result;
                next = mov.moved_to;
        }
    }
    //check where we ended up
    if (
        next != NULL && next->type != STAIR && next->type != POLE
    )
    {
        status = move_to_game(next);
    }

    switch (status)
    {
        case SUCCESS:
            //now only, we check movement points
            int net_movement_points = movement_point_sum * movement_point_factor;
            if (current_player->movement_points <= -1 * net_movement_points)
            {
                //ran out of movement points
                print_ran_out_of_movement_points_message(current_player);
                //TODO
                //return place_in_bawana();
            }
            current_player->movement_points += net_movement_points;
            print_effect_movement_message(current_player, dice, dir);
            print_movement_points_consumed_message(current_player, dice, -1 * net_movement_points);
    }
    //TODO
    
}


MOVEMENT move_to_game(cell* to_move_to)
{
    if (to_move_to == NULL)
    {
        return HIT_WALL;
    }
    switch (to_move_to->type)
    {
        case GAME:
        case STAIR:
        case POLE:
            return SUCCESS;
        case WALL:
            return HIT_WALL;
        case FLAG:
            return FOUND_FLAG;
        case LINK_LOOP:
            return FELL_TO_LOOP;
        case LINK_START:
            return FELL_TO_START;
        case LINK_BAWANA:
            return FELL_TO_BAWANA;
        case DEAD:
            return FELL_TO_DEAD;
    }
}


//NOTE, this function affects values in maze
//It is IMPERATIVE that it resets the values correctly
movement_packet move_from_stair_pole(player* p, cell* start)
{
    cell* next = start;
    cell* visited_cells[PATH_DEPTH];
    int visited_n = 0;

    MOVEMENT path_result = SUCCESS;
    while (next != NULL && (next->type == STAIR || next->type == POLE))
    {
        //if next is visited, we define a loop
        if (next->visited == 1)
        {
            path_result = FELL_TO_LOOP;
        }
        //always add a visit to the array, so we can undo
        next->visited = 1;
        visited_cells[visited_n++] = next;

        cell* forced = next->neighbours[FORCED];
        cell* second = next->neighbours[SECOND];

        //trivial cases
        if (forced == NULL)
        {
            next = second;
            continue;
        }
        if (second == NULL)
        {
            next = forced;
            continue;
        }

        //visited case, pick other one
        //prevents bidirectional looping
        if (forced->visited)
        {
            next = second;
            continue;
        }
        if (second->visited)
        {
            next = forced;
            continue;
        }

        //decision case, pick a random edge, go and see what happens
        int forced_cost = 0;
        int second_cost = 0;

        //go checkout second, for this we remove FORCED
        next->neighbours[FORCED] == NULL;
        assign_bfs_neighbour(next);
        assign_bfs_neighbour(forced);
        assign_bfs_neighbour(second);
        bfs(flag);
        second_cost = second->distance_to_flag;

        //now forced
        next->neighbours[FORCED] = forced;
        next->neighbours[SECOND] = NULL;
        assign_bfs_neighbour(next);
        assign_bfs_neighbour(forced);
        assign_bfs_neighbour(second);
        bfs(flag);
        forced_cost = forced->distance_to_flag;

        //undo everyting
        next->neighbours[SECOND] = second;
        assign_bfs_neighbour(next);
        assign_bfs_neighbour(forced);
        assign_bfs_neighbour(second);
        bfs(flag);

        if (forced_cost < second_cost)
        {
            next = forced;
            continue;
        }
        else if (second_cost < forced_cost)
        {
            next = forced;
            continue;
        }
        else
        {
            // a loop is very highly likely
            printf("LOOP DETECTED.\n");
            print_cell(forced);
            print_cell(second);
            puts("");

            path_result = FELL_TO_LOOP;
        }
    }

    //undo visited
    for (int i = 0; i < visited_n; i++)
    {
        visited_cells[i]->visited = 0;
    }


    switch (path_result)
    {
        case SUCCESS:
            //path is now valid, print the path the player took
            visited_cells[visited_n++] = next;
            for (int i = 0; i < visited_n; i++)
            {
                cell* vis = visited_cells[i];

                if (vis->type == STAIR)
                {
                    print_stair_message(p, vis, visited_cells[i + 1]);
                }
                else if (vis->type == POLE)
                {
                    print_pole_message(p, vis, visited_cells[i + 1]);
                }
            }
            return (movement_packet) {SUCCESS, next};
        default:
            return (movement_packet) {path_result, next};
            
    }
}


unsigned char roll_dice()
{
    return rand() % 6 + 1;
}


//returns direction if rolled, 0 if not and 6 if EMPTY
DIRECTION roll_direction_dice_for(player* current_player)
{
    if (current_player->direction_dice % 4 == 3)
    {
        DIRECTION direction_dice = roll_dice();
        if (direction_dice != SECOND && direction_dice != DIRECTION_COUNT)
        {
            current_player->current_direction = direction_dice;
            current_player->direction_dice++;
            current_player->direction_dice = current_player->direction_dice % 4;
            return direction_dice;
        }
        current_player->direction_dice++;
        current_player->direction_dice = current_player->direction_dice % 4;
        return DIRECTION_COUNT;
    }
    current_player->direction_dice++;
    current_player->direction_dice = current_player->direction_dice % 4;
    return 0;
}