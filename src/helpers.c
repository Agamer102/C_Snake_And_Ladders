#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

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
    {
        NULL,
        NULL
    },
    -1,
    -1,
    UNREACHABLE_DISTANCE,
    0
};


int cell_in_maze_bounds(int floor, int width, int length)
{
    if (floor < 0 || floor >= FLOORS) return 0;
    if (width < 0 || width >= WIDTH) return 0;
    if (length < 0 || length >= LENGTH) return 0;
    return 1;
}


int cell_in_start_area(int floor, int width, int length)
{
    if (floor != 0) return 0;
    if (width < 6 || width >= WIDTH) return 0;
    if (length < 8 || length > 16) return 0;
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
                log_issue(MEMORY_ALLOCATION_ERROR, type, 1, NULL, "Qutting game.");
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


//for graph traversal
void deque_push_front(deque* dq, cell* c)
{
    dq->deque[--dq->front] = c;
}


void deque_push_back(deque* dq, cell* c)
{
    dq->deque[dq->back++] = c;
}


cell* deque_pop_front(deque* dq)
{
    return dq->deque[dq->front++];
}


void bfs(cell* flag)
{
    deque dq;
    dq.front = dq.back = DEQUE_CAP / 2;

    flag->distance_to_flag = 0;
    deque_push_front(&dq, flag);

    //do bfs while the deque is not empty
    while (!(dq.front == dq.back))
    {
        cell* current_cell = deque_pop_front(&dq);

        for (int dir = 0; dir < DIRECTION_COUNT; dir++)
        {
            cell* next;
            if (dir == FORCED || dir == SECOND)
            {
                next = current_cell->bfs_neighbours[dir];
                if (next != NULL)
                {
                    //printf("BFS_CELL: ");
                    //print_cell(next);
                }
            }
            else
            {
                next = current_cell->neighbours[dir];
            }
            
            if (next == NULL) continue;

            int cost = (dir == FORCED || dir == SECOND) ? 1: 100;

            //check if next is unvisited, or visited and the cost
            //is less than the current route
            if 
            (
                next->distance_to_flag == UNREACHABLE_DISTANCE ||
                next->distance_to_flag > current_cell->distance_to_flag + cost
            )
            {
                next->distance_to_flag = current_cell->distance_to_flag + cost;

                //we push to deque based on cost
                if (cost == 1)
                {
                    deque_push_front(&dq, next);
                }
                else
                {
                    deque_push_back(&dq, next);
                }
            }
        }
    }
}


//returns F if assigned to forced, S if assigned to second, 0 if failed
//assignee is cell that is to be assinged with assignor
//NOTE: also sets cell type, but should not mess with poles
char assign_to_forced_then_second(cell* assignee, cell* assignor, int index)
{
    //if (assignee->type == POLE) return 0;
    //check if forced is free or already assigned to correct
    if 
    (
        assignee->neighbours[FORCED] == NULL ||
        assignee->neighbours[FORCED] == assignor
    )
    {
        assignee->neighbours[FORCED] = assignor;
        assignee->n1 = index;
        //only for GAME cells
        if (assignee->type == GAME) assignee->type = STAIR;

        return 'F';
    }

    //check if second is free next, or already assigned to correct
    if 
    (
        assignee->neighbours[SECOND] == NULL ||
        assignee->neighbours[SECOND] == assignor
    )
    {
        assignee->neighbours[SECOND] = assignor;
        assignee->n2 = index;
        if (assignee->type == GAME) assignee->type = STAIR;
        return 'S';
    }

    return 0;
}


void remove_stair_from_cell(cell* receiver, cell* remove)
{
    //if (receiver->type == POLE) return;
    if (receiver->neighbours[FORCED] == remove)
    {
        //if block is no longer a stair cell, reflect that
        if (receiver->neighbours[SECOND] == NULL)
        {
            //only ovewrite types if stair
            if (receiver->type == STAIR) receiver->type = GAME;
        }
        receiver->neighbours[FORCED] = NULL;
    }
    else if (receiver->neighbours[SECOND] == remove)
    {
        //if block is no longer stair, reflect that
        if (receiver->neighbours[FORCED] == NULL)
        {
            if (receiver->type == STAIR) receiver->type = GAME;
        }
        receiver->neighbours[SECOND] = NULL;
    }
}


void assign_bfs_neighbours_trivial()
{
    iterate_map((void*) &assign_bfs_neighbour_trivial);
}


void assign_bfs_neighbour_trivial(cell* current_cell)
{
    if (current_cell->type != STAIR && current_cell->type != POLE) return;
    //special pole logic

    if (current_cell->type == POLE)
    {
        //the pole HAS NO BFS neighbours, because it's fixed
        current_cell->bfs_neighbours[FORCED] = NULL;
        current_cell->bfs_neighbours[SECOND] = NULL;
        if (current_cell->neighbours[FORCED != NULL])
            current_cell->neighbours[FORCED]->bfs_neighbours[FORCED] = current_cell;
        return;
    }

    for (DIRECTION dir = FORCED; dir <= SECOND; dir++)
    {
        cell* neighbour = current_cell->neighbours[dir];
        //do NOT mess with the poles if you're a stair
        if (neighbour == NULL || neighbour->type == POLE) continue;

        for (int i = 0; i < BFS_NEIGHBOURS; i++)
        {
            if (neighbour->bfs_neighbours[i] == current_cell) break;
            if (neighbour->bfs_neighbours[i] == NULL)
            {
                neighbour->bfs_neighbours[i] = current_cell;
                break;
            }
        }
    }
}


void assign_bfs_neighbours()
{
    iterate_map((void*) &assign_bfs_neighbour);
}


void assign_bfs_neighbour(cell* current_cell)
{
    if (current_cell->type != STAIR && current_cell->type != POLE) return;
    //special pole logic
    if (current_cell->type == POLE)
    {
        //the pole HAS NO BFS neighbours, because it's fixed
        current_cell->bfs_neighbours[FORCED] = NULL;
        current_cell->bfs_neighbours[SECOND] = NULL;
        cell* landing = current_cell->neighbours[FORCED];
        
        if (landing != NULL)
        {
            //3 is max bfs neighbour cap
            for (int i = 0; i < BFS_NEIGHBOURS; i++)
            {
                if (landing->bfs_neighbours[i] == NULL)
                {
                    landing->bfs_neighbours[i] = current_cell;
                    break;
                }
            }
        }
    }
    else
    {
        //stair logic
        cell* forced_ne = current_cell->neighbours[FORCED];
        cell* second_ne = current_cell->neighbours[SECOND];

        int forced_back = forced_ne && 
        (
            forced_ne->neighbours[FORCED] == current_cell || forced_ne->neighbours[SECOND] == current_cell
        );
        int second_back = second_ne && 
        (
            second_ne->neighbours[FORCED] == current_cell || second_ne->neighbours[SECOND] == current_cell
        );    

        //if this is the middle of a bidirectional stair, this cell is UNREACHABLE
        if (forced_back && second_back)
        {
            for (int i = 0; i < BFS_NEIGHBOURS; i++) current_cell->bfs_neighbours[i] = NULL;
            return;
        }

        //now we must follow the chain until it terminates
        for (int dir = FORCED; dir <= SECOND; dir++)
        {
            cell* next = current_cell->neighbours[dir];
            if (next == NULL) continue;

            cell* prev = current_cell;
            while (next != NULL && (next->type == STAIR || next->type == POLE))
            {
                cell* temp;
                if (next->type == POLE)
                {
                    temp = next->neighbours[FORCED];
                }
                else
                {
                    //if you are pointing to it and you are a stair it MUST be the case
                    //that at init, it pointed to you too
                    //THEREFORE due to the 2 stair cap, we can take this amazing shortcut
                    if (next->neighbours[FORCED] != prev) temp = next->neighbours[FORCED];
                    else if (next->neighbours[SECOND] != prev) temp = next->neighbours[SECOND];
                    else temp = NULL;
                }
                prev = next;
                next = temp;
            }

            //for this edge, now we MUST be pointing to it's terminal destination
            //Assign value to prev, as it is the last stair/pole which must have this value
            if (prev != NULL)
            {
                for (int i = 0; i < BFS_NEIGHBOURS; i++)
                {
                    if (prev->bfs_neighbours[i] == NULL)
                    {
                        prev->bfs_neighbours[i] = current_cell;
                        break;
                    }
                }
            }
        }
    }

    /*

    for (DIRECTION dir = FORCED; dir <= SECOND; dir++)
    {
        cell* neighbour = current_cell->neighbours[dir];
        //do NOT mess with the poles if you're a stair
        if (neighbour == NULL || neighbour->type == POLE) continue;

        //warning, here we may overwriting
        if (
            neighbour->bfs_neighbours[FORCED] == NULL ||
            neighbour->bfs_neighbours[FORCED] == current_cell
        )
        {
            neighbour->bfs_neighbours[FORCED] = current_cell;
        }
        else if 
        (
            neighbour->bfs_neighbours[SECOND] == NULL ||
            neighbour->bfs_neighbours[SECOND] == current_cell
        )
        {
            neighbour->bfs_neighbours[SECOND] = current_cell;
        }
        else    
        {
            printf("FULL CASE.\n");
            exit(-1);
        }


        //printf("BFS NEIGHBOUR ASSIGNED to %i: of ", dir);
        //print_cell(current_cell);
        //printf(" to ");
        //print_cell(neighbour);
    }
    */
}


void assign_dead_cells()
{
    iterate_map((void*) &assign_dead_cell);
}


void assign_dead_cell(cell* current_cell)
{
    if (current_cell->type == GAME || current_cell->type == STAIR || current_cell->type == POLE)
    {
        if (current_cell->distance_to_flag == UNREACHABLE_DISTANCE) current_cell->type = DEAD;
    }
}


void reset_flag_distances()
{
    iterate_map((void*) &reset_flag_distance);
}


void reset_flag_distance(cell* current_cell)
{
    current_cell->distance_to_flag = UNREACHABLE_DISTANCE;
}


void clear_bfs_neighbours()
{
    iterate_map((void*) &clear_bfs_neighbour);
}


void clear_bfs_neighbour(cell* current_cell)
{
    current_cell->bfs_neighbours[FORCED] = NULL;
    current_cell->bfs_neighbours[SECOND] = NULL;
}


void reset_visited_cell(cell* current_cell)
{
    //print_cell(current_cell);
    current_cell->visited = 0;
}

void reset_visited_cells()
{
    iterate_map((void*) &reset_visited_cell);
}

void quit_game_safely()
{
    free_map();
    if (stairs != NULL)
    {
        free(stairs);
    }
    if (poles != NULL)
    {
        free(poles);
    }
    exit(0);
}


void swap(int* a, int *b)
{
    int temp = *a;
    *a = *b;
    *a = temp;
}