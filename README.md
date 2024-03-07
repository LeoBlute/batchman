# batchman
A project to try batch rendering, font rendering and text batching

# How to build
 1.install gcc
 2.install sdl dependencies to your system
 3.cd into the build directory
 4.build it with: gcc ../code/batchman.c -o batchman -O1 `sdl2-config --cflags --libs` -lm
 5.run ./batchman, if vsync is enabled try running it with: vblank_mode=0 ./batchman

# Screenshot
![Screenshot from 2024-03-07 15-23-58](https://github.com/LeoBlute/TestingGround/assets/130486459/2f83c64c-b9ac-44fd-9af1-2d67868c859d)
