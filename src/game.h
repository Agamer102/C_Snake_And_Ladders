#ifndef GAME_H
#define GAME_H
#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "init.h"
#include "outputs.h"
#include "types.h"
#include "settings.h"

void play_game();
void turn(player* current_player);
void food_poisoning_logic(player* current_player);
void handle_effect_duration(player* current_player);
void handle_effect_movement(player* current_player, unsigned char dice, DIRECTION direction_dice);
unsigned char roll_dice();
DIRECTION roll_direction_dice_for(player* current_player);
#endif