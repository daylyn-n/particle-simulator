#ifndef CONSTANTS_H
#define CONSTANTS_H

#define WIDTH 1200
#define HEIGHT 1000
#define MAX_PARTICLE 1000
#define GRAVITY 9.8
#define FRICTION 0.1
#define PARTICLE_RADIUS 10

// Grid-based spatial partitioning
// Cell size should be >= 2 * max_particle_radius
#define CELL_SIZE 20
#define GRID_COLS ((WIDTH + CELL_SIZE - 1) / CELL_SIZE)
#define GRID_ROWS ((HEIGHT + CELL_SIZE - 1) / CELL_SIZE)
#define NUM_CELLS (GRID_COLS * GRID_ROWS)
// how elastic our particles are
#define gCOR 0.97

#endif
