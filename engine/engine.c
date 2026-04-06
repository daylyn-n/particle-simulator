#include "engine.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

static Particle **particles = NULL;
static int particle_count = 0;

static int randHelp(int lb, int ub)
{
    return (rand() % (ub - lb + 1)) + 1;
}

void EngineInit(int count)
{
    srand(time(NULL));
    particle_count = count;
    particles = malloc(count * sizeof(Particle *));

    for (int i = 0; i < count; i++)
    {
        particles[i] = malloc(sizeof(Particle));

        particles[i]->x           = malloc(sizeof(float));
        particles[i]->y           = malloc(sizeof(float));
        particles[i]->vx          = malloc(sizeof(float));
        particles[i]->vy          = malloc(sizeof(float));
        particles[i]->r           = malloc(sizeof(float));
        particles[i]->mass        = malloc(sizeof(float));
        particles[i]->color       = malloc(sizeof(Color));
        particles[i]->baseColor   = malloc(sizeof(Color));
        particles[i]->collideColor = malloc(sizeof(Color));
        particles[i]->colorTime   = malloc(sizeof(float));

        *particles[i]->x           = randHelp(10, 890);
        *particles[i]->y           = randHelp(10, 590);
        *particles[i]->vx          = randHelp(-20, 20);
        *particles[i]->vy          = randHelp(-20, 20);
        *particles[i]->r           = PARTICLE_RADIUS;
        *particles[i]->mass        = 80;
        *particles[i]->baseColor   = (Color){255, 255, 255, 255};
        *particles[i]->colorTime   = 2.0f;
    }
}

Particle **EngineGetParticles(void)
{
    return particles;
}

int EngineGetParticleCount(void)
{
    return particle_count;
}

/* ---- Physics ---- */

static void UpdateParticle(Particle *particle, float deltaTime)
{
    *particle->x += *particle->vx * deltaTime;
    *particle->y += *particle->vy * deltaTime;

    float x = *particle->x;
    float y = *particle->y;
    float radius = *particle->r;

    // simulate gravity
    // sdl uses the top left corner as its origin
    // so we add gravity to make it go down
    *particle->vy += GRAVITY * deltaTime;

    // apply friction
    if (*particle->y == 0)
    {
        *particle->vx -= FRICTION * *particle->vx * deltaTime;
        if (fabs(*particle->vx) < 1)
        {
            *particle->vx = 0;
        }
    }

    // collision handling
    if (x - radius < 0) // left wall
    {
        *particle->x = radius;
        *particle->vx *= -1;
    }
    if (x + radius > WIDTH) // right wall
    {
        *particle->x = WIDTH - radius;
        *particle->vx *= -1;
    }
    if (y - radius < 0) // top bound
    {
        *particle->y = radius;
        *particle->vy *= -1;
    }
    if (y + radius > HEIGHT) // bottom bound
    {
        *particle->y = HEIGHT - radius;
        if (particle->vy > 0)
        {
            *particle->vy *= -1 * gCOR;
        }
    }
}

/* ---- Collision Detection & Resolution ---- */

static bool ParticlesCollide(Particle *p1, Particle *p2)
{
    float distX = *p1->x - *p2->x;
    float distY = *p1->y - *p2->y;
    float r     = *p1->r + *p2->r;

    return (distX * distX + distY * distY) <= (r * r);
}

static void ResolveCollision(Particle *p1, Particle *p2)
{
    float distX = *p1->x - *p2->x;
    float distY = *p1->y - *p2->y;

    float distCenter = distX * distX + distY * distY;
    if (distCenter == 0.0f)
    {
        return;
    }
    float radDist = sqrtf(distCenter);

    // the distance between two centers normalized
    float nx = distX / radDist;
    float ny = distY / radDist;

    // relative velocity
    float rvx = *p1->vx - *p2->vx;
    float rvy = *p1->vy - *p2->vy;

    float velAlongNormal = rvx * nx + rvy * ny;
    if (velAlongNormal > 0) // double bounce check
        return;

    // impulse
    float massOne = 1.0f / *p1->mass;
    float massTwo = 1.0f / *p2->mass;

    // Newtons third law
    // every action, there is an equal and opposite reaction
    float j = -(1 + gCOR) * velAlongNormal;
    j /= (massOne + massTwo);

    // impulse along the normal direction
    float impulseX = j * nx;
    float impulseY = j * ny;

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

static void DrawParticleColor(Particle *p1, Particle *p2)
{
    *p1->collideColor = (Color){255, 80, 80, 255};
    *p2->collideColor = (Color){255, 80, 80, 255};
}

/* ---- Grid-based spatial partitioning ---- */

static int *gridCells = NULL;
static int *particleNext = NULL;
static int *particleCell = NULL;

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

    if (col < 0) col = 0;
    if (col >= GRID_COLS) col = GRID_COLS - 1;
    if (row < 0) row = 0;
    if (row >= GRID_ROWS) row = GRID_ROWS - 1;

    return row * GRID_COLS + col;
}

static void CollideAllParticle(void)
{
    initGrid();

    // Clear grid
    for (int i = 0; i < NUM_CELLS; i++)
        gridCells[i] = -1;

    for (int i = 0; i < particle_count; i++)
    {
        int cell = getCellIndex(*particles[i]->x, *particles[i]->y);
        particleCell[i] = cell;
        particleNext[i] = gridCells[cell];
        gridCells[cell] = i;
    }

    for (int i = 0; i < particle_count; i++)
    {
        float x = *particles[i]->x;
        float y = *particles[i]->y;
        int col = (int)(x / CELL_SIZE);
        int row = (int)(y / CELL_SIZE);

        if (col < 0) col = 0;
        if (col >= GRID_COLS) col = GRID_COLS - 1;
        if (row < 0) row = 0;
        if (row >= GRID_ROWS) row = GRID_ROWS - 1;

        // Check same cell
        int j = particleNext[i];
        while (j != -1)
        {
            if (ParticlesCollide(particles[i], particles[j]))
            {
                ResolveCollision(particles[i], particles[j]);
                DrawParticleColor(particles[i], particles[j]);
            }
            j = particleNext[j];
        }

        // Check neighboring cells
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
                if (ParticlesCollide(particles[i], particles[j]))
                {
                    ResolveCollision(particles[i], particles[j]);
                    DrawParticleColor(particles[i], particles[j]);
                }
                j = particleNext[j];
            }
        }
    }
}

/* ---- Main step ---- */

void EngineStep(float dt)
{
    for (int i = 0; i < particle_count; i++)
    {
        UpdateParticle(particles[i], dt);
    }
    CollideAllParticle();
}

void EngineCleanup(void)
{
    for (int i = 0; i < particle_count; i++)
    {
        free(particles[i]->x);
        free(particles[i]->y);
        free(particles[i]->vx);
        free(particles[i]->vy);
        free(particles[i]->r);
        free(particles[i]->mass);
        free(particles[i]->color);
        free(particles[i]->baseColor);
        free(particles[i]->collideColor);
        free(particles[i]->colorTime);
        free(particles[i]);
    }
    free(particles);
    free(gridCells);
    free(particleNext);
    free(particleCell);
    particles = NULL;
    gridCells = NULL;
    particleNext = NULL;
    particleCell = NULL;
    particle_count = 0;
}
