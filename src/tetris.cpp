#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <set>

// Linux specific headers
#include <unistd.h>

// OpenGL headers
#include <GL/freeglut.h>
#include <GL/gl.h>

// Game headers
#include "graphics.hpp"
#include "cell.hpp"
#include "element.hpp"
#include "table.hpp"
#include "tetris.hpp"
#include "music.hpp"

// Board
#define HEIGHT                   (20) 
#define WIDTH                    (12)

using namespace std;

void attachElement();
void restart();

long score = 0;
bool _pause = false;

Element* elements[2];
Table *table;
Music *music;

int main(int argc, char** argv)
{

    // Music init
    music = new Music(100,
                      25,
		      "/home/zoltan/prog/tetris/background.mp3",
		      "/home/zoltan/prog/tetris/row.wav");  
    music->play_background();	      

    // Table init
    table = new Table(WIDTH,HEIGHT);

    // elements[0] init
    elements[0] = new Element(table,rand()%7);
    elements[0]->spawn();
    elements[1] = new Element(table,rand()%7);

    // OpenGL Init
    graphics_init(argc,argv);
    return 0;
}

void logic()
{
    
    static unsigned int ctr = 0;
    ctr++;

    if (_pause) music->pause_background();

    
    if (ctr%20 != 0) return; 
 
 
    if (!_pause)
    {
        music->resume_background();
    
	int rows = table->clear_full_rows();

	cout << "score: " << score << endl;

        if (rows) music->play_chunk();

	switch (rows)
	{
	    case 1: score += 40; break;
	    case 2: score += 100; break;
	    case 3: score += 300; break;
	    case 4: score += 1200; break;
	    default: break;
	}
	if (elements[0])
	{
	    if (elements[0]->reachedBottom())
	    {
		attachElement();   
	    }
	    elements[0]->step(DOWN);
	}
    }
	
}

void attachElement()
{
    try
    {
        table->attachCells(elements[0]->stealCells());
    }
    catch(const char* e)
    {
        cerr << "Exception: " << e << endl;
	exit(1);
    }
    
    delete elements[0];
    elements[0] = elements[1];
    elements[0]->spawn();
    elements[1] = new Element(table,rand()%7);
    if (elements[0]->reachedBottom()) 
    {
        restart();
    }
}

void keyboard_callback(unsigned char key,int x, int y)
{
    if (key == 'p') _pause = !_pause;
    if (key == 'r') restart();
    if (key == 27) exit(0);
    
    if (_pause) return;
    
    if (key == 0x20) elements[0]->rotate();
}

void special_callback(int key,int x, int y)
{
    if (_pause) return;

    if (elements[0] == NULL) return; 
    
    int dir = -1;

    switch(key)
    {
    case GLUT_KEY_UP:
        elements[0]->rotate();
        break;
    case GLUT_KEY_DOWN:
        while (!elements[0]->reachedBottom()) elements[0]->step(DOWN);
	return;
        break;
    case GLUT_KEY_LEFT:
        dir = LEFT;
        break;
    case GLUT_KEY_RIGHT:
        dir = RIGHT;
        break;
    default:
        break;
    }
    
    elements[0]->step(dir);
   
}

void restart()
{
    delete table;
    delete elements[0];
    delete elements[1];
    
    table = new Table(WIDTH, HEIGHT);
    elements[0] = new Element(table,rand()%7);
    elements[0]->spawn();
    elements[1] = new Element(table,rand()%7);
    
    score = 0;
    _pause = false;
}























