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

void play_game()
{
    init();
    do
    {
        turn(players + (game_ticks % 3));
        if (game_ticks % STAIR_DIRECTION_CHANGE_TURNS == STAIR_DIRECTION_CHANGE_TURNS - 1) 
        {
            reset_flag_distances();
            clear_bfs_neighbours();
            randomize_stair_direction();
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
            //roll a new random direction
            direction_dice = rand() % 4 + NORTH;
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


//ONLY disoriented and triggered
void handle_effect_movement(player* current_player, unsigned char dice, DIRECTION direction_dice)
{

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
            return direction_dice;
        }
        return DIRECTION_COUNT;
    }
    return 0;
}