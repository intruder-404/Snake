# Snake
Simple snake game in C++ using SDL ~200 lines , &lt;100 kb. Very minimal.

# Use
Just clone the repo and play.

If compiled on another machine, do so in a different directory, for organization sake.


Also, make sure to have SDL_Image header files in the src/include path and it's dynamic lib in the main directory.

mkdir output

g++ -Isrc/Include -Lsrc/lib  -o output/application ssshh.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
