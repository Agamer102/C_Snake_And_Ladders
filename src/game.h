#include <stdio.h>
#include "types.h"

cell maze[FLOORS][WIDTH][LENGTH];

void generate_map();
void fill_section(
    int floor, 
    int width_start, 
    int length_start,
    int width_end,
    int length_end,
    CELL_TYPE type
);

void game()
{
    generate_map();
    printf("Cell size is %ld\n", sizeof(cell));
    printf("Name of cell 0 is %s", maze[0][4][6].name);
}

void generate_map()
{
    //first floor
    fill_section(0, 0, 0, 9, 24, GAME);
    fill_section(0, 6, 8, 9, 16, START);
    fill_section(0, 6, 20, 9, 20, WALL);
    fill_section(0, 6, 21, 6, 24, WALL);
    fill_section(0, 7, 21, 9, 24, BAWANA);
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
    cell to_fill = empty_cell;
    to_fill.type = type;

    for (int width = width_start; width < width_end; width++)
    {
        for (int length = length_start; length < length_end; length++)
        {
            sprintf(to_fill.name, "[%d, %d, %02d]", floor, width, length);
            maze[floor][width][length] = to_fill;
        }
    }
}
