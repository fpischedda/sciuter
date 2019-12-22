CXX=g++
CXX_FLAGS="-c -Wall -std=c++17"
LD_FLAGS="-lSDL2 -lSDL2_image"
SRC="src/main.cpp"
OBJS="main.o"

redo-ifchange $SRC
$CXX $CXX_FLAGS $SRC

redo-ifchange $OBJS
$CXX $LD_FLAGS $OBJS -o $3
