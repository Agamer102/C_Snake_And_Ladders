#include "init.h"

int game_cell_count = 0;
cell* game_cells[GAME_CELL_CAP];


void init()
{
    seed_rand_function();
    generate_map();
    initialize_players();
    get_file_inputs();
    fix_neighbours();
    assign_bfs_neighbours();
    get_distances_to_flag();
    print_stairs();
    assign_movement_points();
    assign_bawana_cells();
    //printf("TYPE START: %i\n", start_link_cell->type);
    //printf("TYPE BAWANA: %i\n", bawana_link_cell->type);
    print_maze();
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
    //more precise fill is needed now
    fill_section(0, 0, 0, 5, 24, GAME);
    fill_section(0, 6, 0, 9, 7, GAME);
    fill_section(0, 6, 17, 9, 19, GAME);
    
    //start section no longer exists, except for special player cells
    fill_section(0, 9, 8, 9, 8, START);
    fill_section(0, 6, 12, 6, 12, START);
    fill_section(0, 9, 16, 9, 16, START);

    //intialize a start-link, all stairs and poles that fall
    //to start now link to this
    fill_section(0, START_LINK_WIDTH, START_LINK_LENGTH, START_LINK_WIDTH, START_LINK_LENGTH, LINK_START);
    start_link_cell = maze[0][START_LINK_WIDTH][START_LINK_LENGTH];

    //similarly a bawana link, that represents a fall to bawana
    fill_section(0, BAWANA_LINK_WIDTH, BAWANA_LINK_LENGTH, BAWANA_LINK_WIDTH, BAWANA_LINK_LENGTH, LINK_BAWANA);
    bawana_link_cell = maze[0][BAWANA_LINK_WIDTH][BAWANA_LINK_LENGTH];

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

}


/*
DEAD cells, are cells from which a player can never return back
to the normal maze. This will hold true, even when the stair direction changes
as it's calculated using the ideal bidirectional stairs case.
*/
void get_distances_to_flag()
{
    //we need to now reverse the edges of each cell in the map
    //WARNING: do not call twice

    //reverse_poles();
    //execute pathfinding

    printf("TRYING BFS");
    print_maze();
    bfs(flag);
    printf("BFS OVER");

    //undo_reverse_poles();

    //here now, we can apply cost of bawana start to bawana
    bawana_link_cell->distance_to_flag = bawana_entrance->distance_to_flag;

    //for start link, it differs by player usually
    //this should be handled more dynamically
}


void reverse_poles()
{
    for (int i = 0; i < pole_count; i++)
    {
        //reverse the poles, ensure it can't point to START or BAWANA
        pole current = poles[i];
        if (
            current.bottom_cell->type == START || 
            current.bottom_cell->type == BAWANA
        )
        {
            if (current.middle_cell != NULL)
            {
                current.middle_cell->neighbours[FORCED] = NULL;
                current.middle_cell->type = GAME;
            }
            current.top_cell->neighbours[FORCED] = NULL;
        }

        //now we can safetly reverse 
        if (current.middle_cell != NULL)
        {
            current.middle_cell->neighbours[FORCED] = current.top_cell;
        }
        current.bottom_cell->neighbours[FORCED] = current.top_cell;
        current.top_cell->neighbours[FORCED] = NULL;
    }
}


void undo_reverse_poles()
{
    for (int i = 0; i < pole_count; i++)
    {
        pole current = poles[i];

        current.top_cell->neighbours[FORCED] = current.bottom_cell;
        if (current.middle_cell != NULL)
        {
            current.middle_cell->neighbours[FORCED] = current.bottom_cell;
        }
        current.bottom_cell->neighbours[FORCED] = NULL;
    }
}



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
