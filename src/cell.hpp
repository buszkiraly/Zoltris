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
    
public:
    Cell(int x, int y, float r = 1.0, float g = 1.0, float b = 0.0)
    {
        this->x = x; 
	this->y = y;
    
        this->r = r;
	this->g = g;
	this->b = b; 
    
    }
    ~Cell(){};
    
    int getX() {return x;}
    int getY() {return y;}
    
    void setX(int x) {this->x = x;}
    void setY(int y) {this->y = y;}
    
    float getR() {return r;}
    float getG() {return g;}
    float getB() {return b;}
};

#endif