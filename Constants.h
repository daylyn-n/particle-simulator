#ifndef CONSTANTS_H
#define CONSTANTS_H
 
#define WIDTH 1000
#define HEIGHT 800
#define MAX_PARTICLE 5000
#define GRAVITY 9.8
#define FRICTION 0.1

// Grid-based spatial partitioning
// Cell size should be >= 2 * max_particle_radius
#define CELL_SIZE 20
#define GRID_COLS ((WIDTH + CELL_SIZE - 1) / CELL_SIZE)
#define GRID_ROWS ((HEIGHT + CELL_SIZE - 1) / CELL_SIZE)
#define NUM_CELLS (GRID_COLS * GRID_ROWS)
// how elsatic our particles are
#define gCOR 0.97
enum RUNNING
{
    IS_RUNNING, NOT_RUNNING
};

#endif

