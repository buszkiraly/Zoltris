# Makefile 

CC=gcc
EXECUTABLE=tetris
INCLUDES=
	  
LIBS= -L/usr/local/lib/ \
      -lglut \
      -lGL \
      -lpthread \
      -lstdc++ \
      -lGLU
	
SOURCES= src/tetris.cpp \
         src/cell.cpp \
	 src/element.cpp \
	 src/table.cpp
	 
HEADERS= src/cell.hpp \
	 src/element.hpp \
	 src/table.hpp
	
OBJECTS=*.o 

CFLAGS= -Wall $(INCLUDES) -g
LFLAGS= $(LIBS)

tetris: tetris.o cell.o element.o table.o
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LFLAGS)
	
tetris.o: $(SOURCES) $(HEADERS) 
	$(CC) $(CFLAGS) -c $(SOURCES) 
	
clean: 
	rm -r *.o 
	rm -r tetris