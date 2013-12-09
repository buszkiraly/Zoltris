#ifndef TABLE_HPP
#define TABLE_HPP

#include <vector>
#include "cell.hpp"

class Table
{
    int width;
    int height;
    
    Cell ***table;
    
    int spawnX, spawnY;
    
public:

    Table(int width, int height);
    ~Table() { delete[] table;} 
    
    vector<Cell*> getCells();        
    int clear_full_rows();
    bool isPlaceAvailable(int x, int y);
    bool attachCells(vector<Cell*> cells);
    int getSpawnX();
    int getSpawnY();

};


#endif