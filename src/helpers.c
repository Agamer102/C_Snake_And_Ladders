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
    -1,
    -1,
    -1
};


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
            cell* next = current_cell->neighbours[dir];
            if (next == NULL) continue;

            //cost for FORCED is 0, others is 1
            int cost = (dir == FORCED || dir == SECOND) ? 0: 1;

            //check if next is unvisited, or visited and the cost
            //is less than the current route
            if 
            (
                next->distance_to_flag == -1 ||
                next->distance_to_flag > current_cell->distance_to_flag + cost
            )
            {
                next->distance_to_flag = current_cell->distance_to_flag + cost;

                //we push to deque based on cost
                if (cost == 0)
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