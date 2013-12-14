#include "table.hpp"
#include <set>

Table::Table(int width, int height)
{

    if ( (width < 50) && (width > 10) )this->width = width;
    else this->width = 10;
    if ( (height < 80) && (height > 20) )this->height = height;
    else this->height = 20;
    
    table = new Cell**[height];
    for (int i = 0; i < height; i++)
    {
        table[i] = new Cell*[width];
    }
    
    
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
	{
	    table[i][j] = NULL;
	}
    }

    spawnX = width/2;
    spawnY = height-3;

}

Table::~Table()
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
	{
	    if (table[i][j] != NULL) delete table[i][j];
	}
    }
}

int Table::clear_full_rows()
{

    bool full;
    set<int> full_rows;


    // detecting full rows
    for (int i = 0; i < height ; i++)
    {
        full = true;
	 
        for (int j = 0; j < width; j++)
	{
	    if (table[i][j] == NULL) 
	    {
	        full = false;
	    }
	}
	
	if (full)
	{
	    full_rows.insert(i);
	}
	
	
    }
    
    // erasing full rows
    // creating temporary table and init it with NULL
    Cell* temp_table[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
	{
	    temp_table[i][j] = NULL;
	}
    }
    
    // copy row if it's not full
    int temp_index = 0;
    for (int i = 0; i < height ; i++)
    {
        if (full_rows.find(i) == full_rows.end()) 
	{
	    for (int j = 0; j < width; j++)
	    {
	        if (table[i][j] != NULL )
		{
		    temp_table[temp_index][j] = table[i][j];
		    temp_table[temp_index][j]->setY(temp_index);
		}
		else
		{
		    temp_table[temp_index][j] = table[i][j];
		}
	    }
	    
	    temp_index++;
	}
	else {}
    
        for (int j = 0; j < width; j++)
	{
	    table[i][j] = NULL;
	}
    }
    
    // loading back the modified table
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
	{
	    table[i][j] = temp_table[i][j];
	}
    }
    
    return full_rows.size();
    
}

vector<Cell*> Table::getCells()
{
    vector<Cell*> ret;
   
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
	{
	    if (table[i][j] != NULL) ret.push_back(table[i][j]);
	}
    }
    
    return ret;
}

bool Table::isPlaceAvailable(int x, int y)
{
    if (y < 0) return false;
    if (x < 0) return false;
    if (x >= width) return false;

    if (table[y][x] == NULL) return true;
    else return false;
}

bool Table::attachCells(vector<Cell*> cells)
{
 
    for (vector<Cell*>::iterator it = cells.begin(); it != cells.end(); ++it)
    {
        Cell* cell = *it;
	int i = cell->getY();
	int j = cell->getX();
	
	if (i<0 || i>=height || j<0 || j>=width) throw "Cell attachment out of range";
		
	table[i][j] = cell;	
    } 

}

int Table::getSpawnX()
{
    return spawnX;
}

int Table::getSpawnY()
{
    return spawnY;
}




















