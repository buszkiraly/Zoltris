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
    
public:
    Element(Table *table, int type);
    ~Element(){}
    
    bool step(int dir);
    bool rotate();
    vector<Cell*> getCells();
    bool reachedBottom();
    
};

#endif