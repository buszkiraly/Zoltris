#include "element.hpp"

Element::Element(Table *table, int type)
{
    this->table = table;
    this->type = type;
        
    float r = 1.0,g = 1.0,b = 1.0;
    
    switch (type)
    {
        case 0:
	    r = 0.596;
	    g = 0.96;
	    b = 1.0;
	    break;
	case 1:
	    r = 0.0;
	    g = 0.0;
	    b = 1.0;
	    break;
	case 2:
	    r = 1.0;
	    g = 0.27;
	    b = 0.0;
	    break;
	case 3:
	    r = 1.0;
	    g = 1.0;
	    b = 0.0;
	    break;
	case 4:
	    r = 0.0;
	    g = 1.0;
	    b = 0.0;
	    break;
	case 5:
	    r = 0.58;
	    g = 0.0;
	    b = 0.827;
	    break;
	case 6:
	    r = 1.0;
	    g = 0.0;
	    b = 0.0;
	    break;
	default:
	    break;
    }	
	
   
    int x = table->getSpawnX();
    int y = table->getSpawnY();
    
    switch (type)
    {
        case 0: 
	    cells[0] = new Cell(x,y,r,g,b);
	    cells[1] = new Cell(x-1,y,r,g,b);
	    cells[2] = new Cell(x+1,y,r,g,b);
	    cells[3] = new Cell(x+2,y,r,g,b); 
	    break;
	case 1:
	    cells[0] = new Cell(x,y,r,g,b);
	    cells[1] = new Cell(x-1,y,r,g,b);
	    cells[2] = new Cell(x-1,y+1,r,g,b);
	    cells[3] = new Cell(x+1,y,r,g,b);
            break; 
	case 2:
	    cells[0] = new Cell(x,y,r,g,b);
	    cells[1] = new Cell(x-1,y,r,g,b);
	    cells[2] = new Cell(x+1,y,r,g,b);
	    cells[3] = new Cell(x+1,y+1,r,g,b);
            break;
	case 3:
	    cells[0] = new Cell(x,y,r,g,b);
	    cells[1] = new Cell(x-1,y,r,g,b);
	    cells[2] = new Cell(x-1,y+1,r,g,b);
	    cells[3] = new Cell(x,y+1,r,g,b);
            break; 
	case 4:
	    cells[0] = new Cell(x,y,r,g,b);
	    cells[1] = new Cell(x-1,y,r,g,b);
	    cells[2] = new Cell(x,y+1,r,g,b);
	    cells[3] = new Cell(x+1,y+1,r,g,b);
            break;  
	case 5:
	    cells[0] = new Cell(x,y,r,g,b);
	    cells[1] = new Cell(x-1,y,r,g,b);
	    cells[2] = new Cell(x,y+1,r,g,b);
	    cells[3] = new Cell(x+1,y,r,g,b);
            break;          
	case 6:
	    cells[0] = new Cell(x,y,r,g,b);
	    cells[1] = new Cell(x,y+1,r,g,b);
	    cells[2] = new Cell(x-1,y+1,r,g,b);
	    cells[3] = new Cell(x+1,y,r,g,b);
            break; 
	default: break;
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

vector<Cell*> Element::getCells()
{
    vector<Cell*> ret;
    
    for (int i = 0; i < 4; i++) ret.push_back(cells[i]);
    return ret;
}

bool Element::reachedBottom()
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