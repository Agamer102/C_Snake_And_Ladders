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


cell* flag = NULL;

cell* start_link_cell = NULL;
cell* bawana_link_cell = NULL;
cell* loop_link_cell = NULL;

void play_game()
{
    init();
    for (DIRECTION dir = 0; dir < DIRECTION_COUNT; dir++)
    {
        bawana_link_cell->neighbours[dir] = NULL;
        start_link_cell->neighbours[dir] = NULL;
        loop_link_cell->neighbours[dir] = NULL;
    }
    recalculate_distances_to_flag();
    print_maze();
    do
    {
        turn(players + (game_ticks % 3));
        if (game_ticks % STAIR_DIRECTION_CHANGE_TURNS == STAIR_DIRECTION_CHANGE_TURNS - 1) 
        {
            randomize_stair_direction();
            add_poles_from_list();
            for (DIRECTION dir = 0; dir < DIRECTION_COUNT; dir++)
            {
                bawana_link_cell->neighbours[dir] = NULL;
                start_link_cell->neighbours[dir] = NULL;
                loop_link_cell->neighbours[dir] = NULL;
            }
            recalculate_distances_to_flag();
            //print_maze();
            //print_stairs();
        }
    } while (++game_ticks && game_ticks < TURN_CAP);
    
}

void recalculate_distances_to_flag()
{
    clear_bfs_neighbours();
    reset_flag_distances();
    assign_bfs_neighbours_trivial();
    bfs(flag);
    bawana_link_cell->distance_to_flag = bawana_entrance->distance_to_flag;
}


void randomize_stair_direction()
{
    if (stair_count < 1)
    {
        return;
    }

    for (int i = 0; i < stair_count; i++)
    {
        stairs[i].direction = rand() % STAIR_TYPES;
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
    printf("\n");
    print_turn(current_player);

    unsigned char dice = 0;
    DIRECTION direction_dice = 0;

    //effects tick, but have no actual effect when player in starting
    if (current_player->status_effect != ADD) handle_effect_duration(current_player);

    //food poisoning is special, needs to be handled seperately
    if(current_player->status_effect == FOOD_POISONING)
    {
        food_poisoning_logic(current_player);
        return;
    } 

    //handle start case
    if (current_player->location == current_player->start)
    {
        dice = roll_dice();
        if (dice == 6)
        {
            //move player to their starting game square
            current_player->location = current_player->start->neighbours[FORCED];
            //print_cell(current_player->location);

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
    
    //printf("OUT OF START.\n");
    //first handle the dice
    switch (current_player->status_effect)
    {
        //default movement
        case ADD:
        case MUL:
            dice = roll_dice();
            direction_dice = roll_direction_dice_for(current_player);
            //printf("NORMAL DICE ROLLED.\n");
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
    handle_effect_movement(current_player, dice, direction_dice);
    return;
}


void food_poisoning_logic(player* current_player)
{
    if (current_player->status_duration < 1)
    {
        //food poisoning has worn off
        current_player->status_effect = ADD;
        current_player->status_duration = 0;
        current_player->status_factor = 1;
        transport_to_bawana(current_player, 1);
        return;
    }
    else
    {
        print_still_food_poisoned_message(current_player);
        return;
    }
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
                break;
            case TRIGGERED:
                print_triggered_wears_off(current_player);
                break;
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
    //direction changes depending on from where the player came
    if (current_player->from != NULL && current_player->from->type == BAWANA)
    {
        current_player->current_direction = NORTH;
    }

    int dice_m = dice;
    DIRECTION dir = direction_dice;
    cell* next = current_player->location;
    //print_cell(next);

    int movement_point_sum = 0;
    int movement_point_factor = 1;

    MOVEMENT status = SUCCESS;

    //printf("READY TO MOVE.\n");
    while (dice_m > 0 && status == SUCCESS)
    {
        //printf("DICE %i\n", dice_m);
        //only game, stair and pole,
        //special logic is handled later

        switch (next->type)
        {
            case STAIR:
            case POLE:
                movement_packet mov = move_from_stair_pole(current_player, next, 1);
                status = mov.move_result;
                next = mov.moved_to;
                if (status != SUCCESS)
                {
                    break;
                }
            case GAME:
                //printf("Maybe fallen.\n");
                //if (dice_m == dice && (next->type == POLE || next->type == STAIR)) exit(0);
                next = next->neighbours[current_player->current_direction];
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
            case BAWANA:
            case LINK_BAWANA:
                status = FELL_TO_BAWANA;
                break;
            case START:
            case LINK_START:
                status = FELL_TO_START;
                break;
            case DEAD:
                status = FELL_TO_DEAD;
                break;
            case WALL:
                status = HIT_WALL;
                break;
            default:
                //printf("UNEXPECTED STATUS %i.\n", status);
                //shouldn't happen, but just in case
                status = FELL_TO_DEAD;
                break;
        }
    }
    //printf("DICE FIN %i.\n", dice_m);
    //check where we ended up
    if (
        next == NULL ||
        (next->type != STAIR && next->type != POLE)
    )
    {
        if (next != NULL) current_player->from = current_player->location;
        status = move_to_game(next);
    }

    int net_movement_points = movement_point_sum * movement_point_factor;
    int dice_cost = WALL_HIT_DICE_COST;
    switch (status)
    {
        case SUCCESS:
            //printf("SUCCESS CASE.\n");
            //now only, we check movement points
            if (current_player->movement_points + net_movement_points <= 0)
            {
                //printf("OUT OF MOVEMENT.\n");
                status = RAN_OUT_OF_MOVEMENT_POINTS;
                break;
            }
            else
            {
                //check for captures
                for (int i = 0; i < NUMBER_OF_PLAYERS; i++)
                {
                    if (players[i].name != current_player->name)
                    {
                        if (players[i].location == next)
                        {
                            players[i].location = players[i].start;
                            print_player_captures_message(current_player, &players[i], next);
                        }
                    }
                }
                current_player->from = current_player->location;
                current_player->location = next;
                current_player->movement_points += net_movement_points;
                print_effect_movement_message(current_player, dice, dir);
                print_movement_points_consumed_message(current_player, dice, -1 * net_movement_points);
                return;
            }
        case FOUND_FLAG:
            print_found_flag_message(current_player, game_ticks);
            quit_game_safely();
            return;
        case HIT_WALL:       
            if 
            (
                current_player->movement_points <= dice_cost
            )
            {
                status = RAN_OUT_OF_MOVEMENT_POINTS;
                break;
            }
            else
            {
                current_player->movement_points -= dice_cost;
                print_hit_wall_message(current_player, dice, dir);
                return;
            }
        case FELL_TO_LOOP:
            current_player->location = current_player->start;
            print_fell_to_loop_message(current_player);
            return;
        case FELL_TO_START:
            current_player->location = current_player->start;
            print_fell_to_start_message(current_player);
            return;
        case FELL_TO_DEAD:
            current_player->location = bawana_entrance;
            print_fell_to_dead_message(current_player);
            return;
        case FELL_TO_BAWANA:
            print_fell_to_bawana_message(current_player);
            transport_to_bawana(current_player, 0);
            return;
        default:
            //printf("ERROR CASE %i", status);
            //exit(-1);
    }
    if (status == RAN_OUT_OF_MOVEMENT_POINTS)
    {
        //printf("AT RAN OUT OF MOV");
        print_ran_out_of_movement_points_message(current_player);
        current_player->movement_points = 0;
        transport_to_bawana(current_player, 0);
        return;
    }
    
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
        case START:
        case WALL:
            return HIT_WALL;
        case FLAG:
            return FOUND_FLAG;
        case LINK_LOOP:
            return FELL_TO_LOOP;
        case LINK_START:
            return FELL_TO_START;
        case BAWANA:
        case LINK_BAWANA:
            return FELL_TO_BAWANA;
        case DEAD:
            return FELL_TO_DEAD;
    }
}


//NOTE, this function affects values in maze
movement_packet move_from_stair_pole(player* p, cell* start, char output)
{
    //printf("TRYING STAIR/POLE MOVEMENT.\n");
    cell* next = start;
    cell* visited_cells[PATH_DEPTH];
    int visited_n = 0;

    MOVEMENT path_result = SUCCESS;
    while (next != NULL && (next->type == STAIR || next->type == POLE))
    {
        //printf("%i\n", game_ticks);
        //printf("Visited number: %i\n", visited_n);
        //print_cell(next);
        //if next is visited, we define a loop
        if (next->visited == game_ticks)
        {
            path_result = FELL_TO_LOOP;
            break;
        }
        //always add a visit to the array, so we can print outputs
        next->visited = game_ticks;
        visited_cells[visited_n++] = next;

        cell* forced = next->neighbours[FORCED];
        cell* second = next->neighbours[SECOND];

        //trivial case
        if (forced == NULL && second == NULL)
        {
            break;
        }

        //edge case, end of stair
        if (forced == NULL && second && second->visited == game_ticks)
        {
            //end of chain
            break;
        }
        if (second == NULL && forced && forced->visited == game_ticks)
        {
            //end of chain
            break;
        }
        //other edge case, start of stair
        if (forced == NULL && second)
        {
            next = second;
            continue;
        }
        if (second == NULL && forced)
        {
            next = forced;
            continue;
        }


        //printf("\nTRIVIAL OVER.\n");

        //visited case, pick other one
        //prevents bidirectional looping
        if (forced->visited == game_ticks && second->visited != game_ticks) { next = second; continue;}
        if (second->visited  == game_ticks && forced->visited != game_ticks) { next = forced; continue;}
        if (forced->visited == game_ticks && second->visited == game_ticks)
        {
            path_result = FELL_TO_LOOP;
            next = NULL;
            break;
        }

        int forced_cost = forced->distance_to_flag;
        int second_cost = second->distance_to_flag;

        if (forced->type == LINK_START)
        {
            forced_cost = p->start->distance_to_flag;
        }
        if (second->type == LINK_START)
        {
            second_cost = p->start->distance_to_flag;
        }


        if (forced_cost < second_cost)
        {
            next = forced;
            continue;
        }
        else if (second_cost < forced_cost)
        {
            next = second;
            continue;
        }
        else
        {
            //if equal, pick randomly
            next = rand() % 2 == 0 ? forced: second;
            continue;
        }
    }
    //printf("END of SEARCH\n");

    if (next == NULL)
    {
        //printf("LOGIC ERROR.\n");
        path_result = FELL_TO_DEAD;
    }


    switch (path_result)
    {
        case SUCCESS:
            //path is now valid, print the path the player took
            if (output == 1 && visited_n > 1)
            {
                for (int i = 0; i < visited_n - 1; i++)
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
            }
            return (movement_packet) {SUCCESS, next};
        default:
            return (movement_packet) {path_result, next};
            
    }
}


void transport_to_bawana(player* current_player, int food_poisoned)
{
    cell* to_move_to = bawana[rand() % BAWANA_CELL_COUNT];
    CELL_OPERATION op = to_move_to->movement_point_operation;
    int bonus = bawana_points[op];

    print_placed_in_bawana_message(current_player, op, food_poisoned);

    //fix player's status
    current_player->from = to_move_to;
    current_player->location = bawana_entrance;
    current_player->status_effect = op;
    current_player->status_duration = bawana_duration[op];
    current_player->movement_points += bonus;

    switch (op)
    {
        case FOOD_POISONING:
            print_get_food_poisoning_message(current_player);
            return;
        case DISORIENTED:
            print_get_disoriented_message(current_player);
            return;
        case TRIGGERED:
            current_player->status_factor *= TRIGGER_FACTOR;
            print_get_triggered_message(current_player);
            return;
        case HAPPY:
            print_get_happy_message(current_player);
            return;
        case ADD:
        case MUL:
            //overwrite for different logic
            current_player->location = to_move_to;
            bonus = to_move_to->movement_point_operand;
            print_get_bawana_points_message(current_player, bonus);
            return;
    }
}


unsigned char roll_dice()
{
    return rand() % 6 + 1;
}


//returns direction if rolled, 0 if not and 6 if EMPTY
DIRECTION roll_direction_dice_for(player* current_player)
{
    if (current_player->direction_dice == 3)
    {
        //printf("Player %c direction_dice counter: %d\n", current_player->name, current_player->direction_dice);
        DIRECTION direction_dice = roll_dice();
        if (direction_dice == 1 || direction_dice == 6)
        {
            current_player->direction_dice = 0;
            return 6;
        }
        else
        {
            current_player->direction_dice = 0;
            current_player->current_direction = direction_dice;
            return direction_dice;
        }
    }
    current_player->direction_dice++;
    return 0;
}