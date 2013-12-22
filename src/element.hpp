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
	Element(const Element &from);
    ~Element();
    
    bool step(int dir);
    bool rotate();
    vector<Cell*> getCells() const;
    vector<Cell*> stealCells();
    bool reachedBottom() const;
    int getType() const;
    bool isSpawned() const;
    void spawn();
    void setCenter(int x, int y);
	int getX() const {return x;}
	int getY() const {return y;}
	Table *getTable() const {return table;}
    
};

#endif