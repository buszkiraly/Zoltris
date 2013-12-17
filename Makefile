# Makefile 

CC=g++
EXECUTABLE=tetris
INCLUDES=
	  
LIBS= -L/usr/local/lib/ \
      -lglut \
      -lGL \
      -lpthread \
      -lstdc++ \
      -lGLU \
      -lSDL2_mixer \
      -lSDL2
	
SOURCES= src/tetris.cpp \
         src/cell.cpp \
	 src/element.cpp \
	 src/table.cpp \
	 src/graphics.cpp \
	 src/music.cpp
	 
HEADERS= src/cell.hpp \
	 src/element.hpp \
	 src/table.hpp \
	 src/graphics.hpp \
	 src/music.hpp
	
OBJECTS=*.o 

CFLAGS= -Wall $(INCLUDES) -g
LFLAGS= $(LIBS)

tetris: tetris.o cell.o element.o table.o graphics.o music.o
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LFLAGS)
	
tetris.o: $(SOURCES) $(HEADERS) 
	$(CC) $(CFLAGS) -c $(SOURCES) 
	
clean: 
	rm -r *.o 
	rm -r tetris