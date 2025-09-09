#include <stdio.h>
#include <stdlib.h>
#include "calc.h"
#include "game.h"

int game_cell_count = 0;
cell* game_cells[GAME_CELL_CAP];


void assign_movement_points()
{
    get_game_block_array();
    fisher_yates(game_cells, game_cell_count);

    //now game cell array, containig all game cells
    //is obtained. assign movement points

    //we assign largest prob last to remaining cells to minimize error
    //printf("Total game cells: %i\n\n", game_cell_count);

    int start = 0;
    int end = game_cell_count * DIST_G_ZERO; 
    //printf("Zeros from: %i to %i\n", start, end);
    assign_to_range_array(game_cells, ADD, 0, 0, start, end);

    start = end;
    end = start + game_cell_count * DIST_G_BONUS_1_2;
    //printf("+1..+2 from: %i to %i\n", start, end);
    assign_to_range_array(game_cells, ADD, 1, 2, start, end);

    start = end;
    end = start + game_cell_count * DIST_G_BONUS_3_5;
    //printf("+3..+5 from: %i to %i\n", start, end);
    assign_to_range_array(game_cells, ADD, 3, 5, start, end);

    start = end;
    end = start + game_cell_count * DIST_G_MULTIPLY;
    //printf("x2..x3 from: %i to %i\n", start, end);
    assign_to_range_array(game_cells, MUL, 2, 3, start, end);

    start = end;
    // for remaining cells, to prevent overflow
    end = game_cell_count;
    //printf("-4..-1 from: %i to %i\n", start, end);
    assign_to_range_array(game_cells, ADD, -4, -1, start, end);
}

void assign_bawana_cells()
{
    int index = 0;
    for (int width = BAWANA_START_WIDTH; width < WIDTH; width++)
    {
        for (int length = BAWANA_START_LENGTH; length < LENGTH; length++)
        {
            bawana[index++] = maze[0][width][length];
        }
    }

    fisher_yates(bawana, BAWANA_CELL_COUNT);
    
    //food poisoning to happy
    //NOTE: movement point operand not assigned here for these
    assign_to_range_array(bawana, FOOD_POISONING, 0, 0, 0, 2);
    assign_to_range_array(bawana, DISORIENTED, 0, 0, 2, 4);
    assign_to_range_array(bawana, TRIGGERED, 0, 0, 4, 6);
    assign_to_range_array(bawana, HAPPY, 0, 0, 6, 8);
    //movement point bonus cells
    assign_to_range_array(bawana, ADD, 10, 100, 8, BAWANA_CELL_COUNT);
}


void is_game_cell(cell* current_cell)
{
    if (current_cell->type == GAME)
    {
        game_cells[game_cell_count] = current_cell;
        game_cell_count++;
    }
}


void get_game_block_array()
{
    iterate_map((void *)&is_game_cell);
}


//algorithm to randomize the order of given cell
//guarentees random permutation of array
void fisher_yates(cell** array, int length)
{
    for (int i = length - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        cell* temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}


//start inclusive, end exclusive
void assign_to_range_array(
    cell** array, 
    CELL_OPERATION operation, 
    int op_range_start,
    int op_range_end,
    int start,
    int end
)
{
    for (int i = start; i < end; i++)
    {
        array[i]->movement_point_operation = operation;
        array[i]->movement_point_operand = 
            op_range_start + rand() % (op_range_end - op_range_start + 1);
    }
}
