# Particle Simulator using C and SDL2

## Why
Just wanted to get out of C++ object oriented style of programming and try a more procedural style.
I've also been learning openGL on the side of classes and the guide uses SDL2 as its window generator, so I thought this project would
be a good opportunity to learn it
 
## demo and fps check


https://github.com/user-attachments/assets/fc020dff-923c-4af6-afb6-688457cae28b

### first iteration
https://github.com/user-attachments/assets/b0d30bf6-c61e-4518-9005-b5edf6298171


## getting started
### Prereq
- SDL2
- SDL2/SDL_ttf
- GCC

### Building
```bash
gcc main.c Constants.h Collision.c FpsText.c -lSDL2 -lm -lSDL2_ttf -o output

```

### Run
```bash
./output
```

## What to except
For now this is just a playground project using SDL2, C and game physics. 
But I plan to come back to this project by making it more interactive. (ie adding a restart feature, adding and deleted number of balls within the window,
changing gravity in the window, and elasticity factor in the window)

## FIXES
### BEFORE
<img width="1901" height="1068" alt="before" src="https://github.com/user-attachments/assets/87f2bfc1-98e0-4005-9abb-b7ee8ca71a4b" />

### AFTER
<img width="1919" height="975" alt="after" src="https://github.com/user-attachments/assets/b2d5cd91-985d-4778-ad8c-108e6152422f" />
Using valgrind and callgrind, I found what was eating away at the peformance of the simualtor. It was a rendering issue, where I would render every particle at every frame. The fix was simply having a pre-rendered texture and adding it onto the particle coordinates.

if you want to run valgrind and call grind for yourself use these commands:
```bash
 valgrind --tool=callgrind --instr-atstart=no ./output

# then after particles are bouncing for a bit or are settled
kcachegrind callgrind.out.*
```





## Resources
https://www.youtube.com/watch?v=LyGKycYT2v0
https://gamedev.net/forums/topic/323489-what-is-the-collision-normal/
https://en.wikipedia.org/wiki/Bouncing_ball 
https://www.youtube.com/watch?v=0pqFytjlSDE
