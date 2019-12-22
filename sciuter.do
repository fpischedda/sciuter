CXX=g++
CXX_FLAGS="-c -Wall -str=c++17"
LD_FLAGS="-lSDL2"
SRC="src/main.cpp"
OBJS="main.o"
BIN=sciuter

redo-ifchange $SRC
$CXX $CXX_FLAGS $SRC

redo-ifchange $OBJS
$CXX $LD_FLAGS $OBJS -o $BIN
