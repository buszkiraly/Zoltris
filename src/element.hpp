#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include "cell.hpp"
#include "table.hpp"

#define DOWN  0
#define LEFT  1
#define RIGHT 2
#define UP    4

class Element
{
    Cell  *cells[4];
    Table *table;
    int   type;
    bool  spawned;
    int   x,y;
    
public:
    Element(Table *table, int type);
    ~Element();
    
    bool step(int dir);
    bool rotate();
    vector<Cell*> getCells();
    vector<Cell*> stealCells();
    bool reachedBottom();
    int getType();
    bool isSpawned();
    void spawn();
    void setCenter(int x, int y);
    
};

#endif