#include "Collision.h"
#include "Constants.h"
// x^2 + y^2 = r^2
// dx = +- sqrt(r*2 - y^2)
// get the x points of the circle to draw a line connectng the left point and right point
void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
    for (int y = -radius; y <= radius; y++) {
        int dx = (int)floor(sqrt((double)(radius * radius - y * y)));
        SDL_RenderDrawLine(renderer,
                           cx - dx, cy + y,
                           cx + dx, cy + y);
    }
}

void DrawParticle(SDL_Renderer *renderer, Particle *particle)
{
    drawFilledCircle(renderer, *particle->x, *particle->y, *particle->r);
}
void UpdateParticle(Particle *particle, float deltaTime)
{
    *particle->x += *particle->vx * deltaTime;
    *particle->y += *particle->vy * deltaTime;

    float x = *particle->x;
    float y = *particle->y;
    float radius = *particle->r;
    
    // simulate gravity 
    // sdl uses the top left corner as its origin
    // so we add gravity to make it go down
    *particle->vy +=  GRAVITY * deltaTime;
    
    
    // apply friction
   if(*particle->y == 0)
   {
        *particle->vx -= FRICTION * *particle->vx * deltaTime;
        if(fabs(*particle->vx) < 1)
        {
            *particle->vx = 0;
        }
   }

    // collision handling
    if(x - radius < 0) // left wall
    {
        *particle->x = radius;
        *particle->vx *= -1;
    }
    if(x + radius > WIDTH) // right wall
    {
        *particle->x = WIDTH - radius;
        *particle->vx *= -1;
    }
    if(y - radius < 0) // top bound
    {
        *particle->y = radius;
        *particle->vy *= -1;
    }
    if(y + radius >  HEIGHT) // bottom bound
    {
        // bounce height based on mass
        
        *particle->y = HEIGHT - radius;
        if(particle->vy > 0)
        {
            *particle->vy *= -1 * gCOR;
        }
    }
}

// two particles collide if the 
// distance between centers is <= radii
// sqrt(distX^2 + distY^2) <= radii 
// distX^2 + distY^2 <= radii^2
bool ParticlesCollide(SDL_Renderer *renderer, Particle *p1, Particle *p2)
{
    float distX = *p1->x - *p2->x;
    float distY = *p1->y - *p2->y;
    float r     = *p1->r + *p2->r;

    return (distX*distX + distY*distY) <= (r * r);
}
void ResolveCollision(Particle *p1, Particle *p2)
{
    float distX = *p1->x - *p2->x;
    float distY = *p1->y - *p2->y;

    float distCenter = distX*distX + distY*distY;
    if (distCenter == 0.0f)
    {
        return;
    }
    float radDist = sqrtf(distCenter);
     
     // the distance between two centers normalized
     // in linear algebra, normalization allows us to
     // scale the magnitude (collision impulse) of our two particles
     // without changing the direction it goes in
     float nx = distX / radDist;
     float ny = distY / radDist;   

     // relative velocity
     float rvx = *p1->vx - *p2->vx;
     float rvy = *p1->vy - *p2->vy;

     float velAlongNormal = rvx * nx + rvy * ny;
     if(velAlongNormal > 0) // double bounce check
        return;

        
     // impulse
     float massOne = 1.0f / *p1->mass;
     float massTwo = 1.0f / *p2->mass;

     // Newtons third law :laugh: 
     // every action, there is an equal and opposite reaction
     float j = -(1 + gCOR) * velAlongNormal;
     j /= (massOne + massTwo);

     // impulse along the normal direction!!!!
     float impulseX = j * nx;
     float impulseY = j * ny;
    
   // deltaV = j / mass
     // our normal is the direction from
     // p2 -> p1
     // so p1 gets pushed forward along the Normal 
     // and p2 gets pushed away
     *p1->vx += impulseX * massOne;
     *p1->vy += impulseY * massOne;
     *p2->vx -= impulseX * massTwo;
     *p2->vy -= impulseY * massTwo;
    
    // position corrections
    float overlap = (*p1->r + *p2->r) - radDist;
    if (overlap > 0)
    {
        float percent = 0.8f; // stabilization
        float correction = overlap * percent / (massOne + massTwo);

        *p1->x += correction * massOne * nx;
        *p1->y += correction * massOne * ny;
        *p2->x -= correction * massTwo * nx;
        *p2->y -= correction * massTwo * ny;
    }
}

void DrawParticleColor(Particle *p1, Particle* p2)
{
    *p1->collideColor = (SDL_Color){255, 80, 80, 255};
    *p2->collideColor = (SDL_Color){255, 80, 80, 255};
}

// Static grid storage - allocated once, reused every frame
static int *gridCells = NULL;      // For each cell: linked list head (particle index, -1 = empty)
static int *particleNext = NULL;   // For each particle: next particle in same cell (-1 = end)
static int *particleCell = NULL;   // For each particle: which cell it belongs to

static void initGrid(void)
{
    if (gridCells == NULL)
    {
        gridCells = malloc(NUM_CELLS * sizeof(int));
        particleNext = malloc(MAX_PARTICLE * sizeof(int));
        particleCell = malloc(MAX_PARTICLE * sizeof(int));
    }
}

static inline int getCellIndex(float x, float y)
{
    int col = (int)(x / CELL_SIZE);
    int row = (int)(y / CELL_SIZE);
    
    // Clamp to valid range
    if (col < 0) col = 0;
    if (col >= GRID_COLS) col = GRID_COLS - 1;
    if (row < 0) row = 0;
    if (row >= GRID_ROWS) row = GRID_ROWS - 1;
    
    return row * GRID_COLS + col;
}

void CollideAllParticle(SDL_Renderer* renderer, Particle *particle[])
{
    initGrid();
    
    // Clear grid - set all cells to empty (-1)
    for (int i = 0; i < NUM_CELLS; i++)
        gridCells[i] = -1;
    
    // Phase 1: Insert all particles into grid (O(n))
    for (int i = 0; i < MAX_PARTICLE; i++)
    {
        int cell = getCellIndex(*particle[i]->x, *particle[i]->y);
        particleCell[i] = cell;
        
        // Insert at head of linked list for this cell
        particleNext[i] = gridCells[cell];
        gridCells[cell] = i;
    }
    
    // Phase 2: Check collisions
    // For each particle, check against particles in same cell and 4 neighboring cells
    // (right, bottom-left, bottom, bottom-right) to avoid duplicate checks
    for (int i = 0; i < MAX_PARTICLE; i++)
    {
        float x = *particle[i]->x;
        float y = *particle[i]->y;
        int col = (int)(x / CELL_SIZE);
        int row = (int)(y / CELL_SIZE);
        
        // Clamp
        if (col < 0) col = 0;
        if (col >= GRID_COLS) col = GRID_COLS - 1;
        if (row < 0) row = 0;
        if (row >= GRID_ROWS) row = GRID_ROWS - 1;
        
        // Check same cell - only particles after this one in the list
        int j = particleNext[i];
        while (j != -1)
        {
            if (ParticlesCollide(renderer, particle[i], particle[j]))
            {
                ResolveCollision(particle[i], particle[j]);
                DrawParticleColor(particle[i], particle[j]);
                SDL_SetRenderDrawColor(renderer,
                    particle[i]->collideColor->r,
                    particle[i]->collideColor->g,
                    particle[i]->collideColor->b, 255);
            }
            j = particleNext[j];
        }
        
        // Check neighboring cells (only right and bottom neighbors to avoid duplicates)
        // Neighbors: right (col+1, row), bottom-left (col-1, row+1), bottom (col, row+1), bottom-right (col+1, row+1)
        int neighbors[4][2] = {{1, 0}, {-1, 1}, {0, 1}, {1, 1}};
        
        for (int n = 0; n < 4; n++)
        {
            int ncol = col + neighbors[n][0];
            int nrow = row + neighbors[n][1];
            
            if (ncol < 0 || ncol >= GRID_COLS || nrow < 0 || nrow >= GRID_ROWS)
                continue;
            
            int neighborCell = nrow * GRID_COLS + ncol;
            j = gridCells[neighborCell];
            
            while (j != -1)
            {
                if (ParticlesCollide(renderer, particle[i], particle[j]))
                {
                    ResolveCollision(particle[i], particle[j]);
                    DrawParticleColor(particle[i], particle[j]);
                    SDL_SetRenderDrawColor(renderer,
                        particle[i]->collideColor->r,
                        particle[i]->collideColor->g,
                        particle[i]->collideColor->b, 255);
                }
                j = particleNext[j];
            }
        }
    }
}
