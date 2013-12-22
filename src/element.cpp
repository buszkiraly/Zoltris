#include "element.hpp"

Element::Element(Table *table, int type)
{
    this->table = table;
    this->type = type;
        
    spawned = false;
	
    x = 0;
    y = 5;
    
    switch (type)
    {
        case 0: 
	    cells[0] = new Cell(x,y,type);
	    cells[1] = new Cell(x-1,y,type);
	    cells[2] = new Cell(x+1,y,type);
	    cells[3] = new Cell(x+2,y,type);
	    break;
	case 1:
	    cells[0] = new Cell(x,y,type);
	    cells[1] = new Cell(x-1,y,type);
	    cells[2] = new Cell(x-1,y+1,type);
	    cells[3] = new Cell(x+1,y,type);
            break; 
	case 2:
	    cells[0] = new Cell(x,y,type);
	    cells[1] = new Cell(x-1,y,type);
	    cells[2] = new Cell(x+1,y,type);
	    cells[3] = new Cell(x+1,y+1,type);
            break;
	case 3:
	    cells[0] = new Cell(x,y,type);
	    cells[1] = new Cell(x-1,y,type);
	    cells[2] = new Cell(x-1,y+1,type);
	    cells[3] = new Cell(x,y+1,type);
            break; 
	case 4:
	    cells[0] = new Cell(x,y,type);
	    cells[1] = new Cell(x-1,y,type);
	    cells[2] = new Cell(x,y+1,type);
	    cells[3] = new Cell(x+1,y+1,type);
            break;  
	case 5:
	    cells[0] = new Cell(x,y,type);
	    cells[1] = new Cell(x-1,y,type);
	    cells[2] = new Cell(x,y+1,type);
	    cells[3] = new Cell(x+1,y,type);
            break;          
	case 6:
	    cells[0] = new Cell(x,y,type);
	    cells[1] = new Cell(x,y+1,type);
	    cells[2] = new Cell(x-1,y+1,type);
	    cells[3] = new Cell(x+1,y,type);
            break; 
	default: break;
    }
}

Element::Element(const Element &from)
{
    table = from.getTable();
	type = from.getType();
	
	spawned = false;
	
	x = from.getX();
	y = from.getY();
	
	vector<Cell*> v_cells = from.getCells();
	for (vector<Cell*>::iterator it = v_cells.begin(); it != v_cells.end(); it++)
	{
	    Cell *temp_cell = (Cell*)*it;
	    cells[it-v_cells.begin()] = new Cell(temp_cell->getX(), temp_cell->getY(), type);
	}
	
	 
}

Element::~Element()
{
    for (int i = 0; i < 4; i++)
    {
        if (cells[i]) delete cells[i];
    }
}

bool Element::step(int dir) 
{
    int i = 0;

    switch (dir)
    {
        case DOWN:
	    for (i = 0; i < 4; i++)
	    {
	        if ( !table->isPlaceAvailable(cells[i]->getX(),cells[i]->getY()-1) )
		{
		    cout << "step error" << endl;
		    return false;
		}
		else
		{
		}
	    }
	    
	    for (i = 0; i < 4; i++) cells[i]->setY(cells[i]->getY()-1);
	    return true;
		
	    break;
	case LEFT: 
	    for (i = 0; i < 4; i++)
	    {
	        if ( !table->isPlaceAvailable(cells[i]->getX()-1,cells[i]->getY()) )
		{
		    return false;
		}
		else
		{
		}
	    }

	    for (i = 0; i < 4; i++) cells[i]->setX(cells[i]->getX()-1);
	    return true;

	    break;
	case RIGHT:
	    for (i = 0; i < 4; i++)
	    {
	        if ( !table->isPlaceAvailable(cells[i]->getX()+1,cells[i]->getY()) )
		{
		    return false;
		}
		else
		{
		}
	    }

	    for (i = 0; i < 4; i++) cells[i]->setX(cells[i]->getX()+1);
	    return true;

	    break;
	case UP: 
	    for (i = 0; i < 4; i++)
	    {
	        if ( !table->isPlaceAvailable(cells[i]->getX(),cells[i]->getY()+1) )
		{
		    return false;
		}
		else
		{
		}

	    }
	    
	    for (i = 0; i < 4; i++) cells[i]->setY(cells[i]->getY()+1);
	    return true;
	    
	    break;
	default: 
	    return false;
	    break;
    }
    
}

bool Element::rotate() 
{

    // rotate the element
    switch (type)
    {
        case 0: 
	
	    // check if it can rotate
	    
	    int startx, starty;
	    
	    if ( (cells[0]->getY() == cells[1]->getY()) 
	         && (cells[0]->getX() > cells[1]->getX()) )
	    {
	        startx = -1;
		starty = -2;
	    }
	    else if ( (cells[0]->getX() == cells[1]->getX()) 
	         && (cells[0]->getY() > cells[1]->getY()) )
	    {
	        startx = -2;
		starty = -2;
	    }
	    else if ( (cells[0]->getY() == cells[1]->getY()) 
	         && (cells[0]->getX() < cells[1]->getX()) )
	    {
	        startx = -2;
		starty = -1;
	    }
	    else if ( (cells[0]->getX() == cells[1]->getX()) 
	         && (cells[0]->getY() < cells[1]->getY()) )
	    {
	        startx = -1;
		starty = -1; 
	    }
	    else {}
	    
	    for (int i = startx; i < startx+4; i++)
	        for (int j = starty; j < starty+4; j++)
		{
		    if (!table->isPlaceAvailable(cells[0]->getX() + i, cells[0]->getY() + j)) return false;
		}
	
	    // rotate the element
            for (int i = 1; i < 4 ; i++)
	    {
	        int x = this->cells[0]->getX() - this->cells[i]->getX();
		int y = this->cells[0]->getY() - this->cells[i]->getY();
		cells[i]->setX(this->cells[0]->getX() + y);
		cells[i]->setY(this->cells[0]->getY() - x);
	    }
	    break;
	    break;
        case 3:
	    break;
	case 1:
	case 2:
        case 4:
	case 5:
	case 6:
	
	    // check if it can rotate
	    for (int i = -1; i < 2; i++)
	        for (int j = -1; j < 2; j++)
		{
		    if (!table->isPlaceAvailable(cells[0]->getX() + i, cells[0]->getY() + j)) return false;
		}
	       
	    // rotate the element
            for (int i = 1; i < 4 ; i++)
	    {
	        int x = this->cells[0]->getX() - this->cells[i]->getX();
		int y = this->cells[0]->getY() - this->cells[i]->getY();
		cells[i]->setX(this->cells[0]->getX() + y);
		cells[i]->setY(this->cells[0]->getY() - x);
	    }
	    break;
    
    
        default:
	    break;
    }
    
    
    
    
}

vector<Cell*> Element::getCells() const
{
    vector<Cell*> ret;
    
    for (int i = 0; i < 4; i++) ret.push_back(cells[i]);
    return ret;
}

vector<Cell*> Element::stealCells() 
{
    vector<Cell*> ret;
    
    for (int i = 0; i < 4; i++) 
    {
        ret.push_back(cells[i]);
        cells[i] = NULL;
    }
    return ret;
}

bool Element::reachedBottom() const
{
    for (int i = 0; i < 4; i++)
    {
        if ( !table->isPlaceAvailable(cells[i]->getX(),cells[i]->getY()-1) )
	{
	    return true;
	}
	else
	{
	}
    }
    
    return false;
}

int Element::getType() const
{
    return type;
}

bool Element::isSpawned() const
{
    return spawned;
}

void Element::spawn()
{
    if (spawned) return;
    else
    {
        setCenter(table->getSpawnX(), table->getSpawnY());
	spawned = true;
    }
}

void Element::setCenter(int x, int y)
{

    for (int i = 0; i < 4; i++)
    {
        cells[i]->setX(cells[i]->getX() - this->x + x);
        cells[i]->setY(cells[i]->getY() - this->y + y);
    }
    
    this->x = x;
    this->y = y;
}


