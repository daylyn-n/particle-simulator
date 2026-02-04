# using this to get out of C++ encapsulation hell

## demo and fps check

https://github.com/user-attachments/assets/b0d30bf6-c61e-4518-9005-b5edf6298171


## getting started
### Prereq
- SDL2
- SDL2/SDL_ttf
- GCC

### Building
```bash
gcc main.c Constants.h Collision.c FpsText.c -lSDL2 -lm -lSDL2_ttf

```

### Run
```bash
./a.out
```
or just make an output file when you compile it

## What to except
For now this is just a playground project using SDL2, C and game physics. 
But I plan to come back to this project by making it more interactive. (ie adding a restart feature, adding and deleted number of balls within the window,
changing gravity in the window, and elasticity factor in the window)


## Resources
https://www.youtube.com/watch?v=LyGKycYT2v0
https://gamedev.net/forums/topic/323489-what-is-the-collision-normal/
https://en.wikipedia.org/wiki/Bouncing_ball 
https://www.youtube.com/watch?v=0pqFytjlSDE
