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
	
SOURCES= tetris.cpp \
         cell.cpp \
	 element.cpp
	
OBJECTS=*.o 

CFLAGS= -Wall $(INCLUDES) -g
LFLAGS= $(LIBS)

tetris: tetris.o cell.o element.o table.o
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LFLAGS)
	
tetris.o: tetris.cpp cell.cpp cell.hpp element.cpp element.hpp table.cpp table.hpp
	$(CC) $(CFLAGS) -c $(SOURCES) 
	
clean: 
	rm -r *.o 
	rm -r tetris