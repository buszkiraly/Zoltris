#ifndef CELL_HPP
#define CELL_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

class Cell
{
    int x,y;
    float r,g,b;
    int type;
    
public:
    Cell(int x, int y, int type)
    {
        this->x = x; 
	this->y = y;
    
        this->r = r;
	this->g = g;
	this->b = b; 
	
	this->type = type;
    
    }
    ~Cell(){};
    
    int getX() {return x;}
    int getY() {return y;}
    
    void setX(int x) {this->x = x;}
    void setY(int y) {this->y = y;}
    
    float getR() {return r;}
    float getG() {return g;}
    float getB() {return b;}
    
    int getType() {return type;}
};

#endif