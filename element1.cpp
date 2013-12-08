#include "element1.hpp"

bool Element::step(int dir)
{
    int x = cell->getX();
    int y = cell->getY(); 

    switch (dir)
    {
        case DOWN:
	    y--; 
	    break;
	case LEFT: 
	    x--;
	    break;
	case RIGHT:
	    x++; 
	    break;
	case UP: 
	    y++;
	    break;
	default: break;
    }
    
    if (table->isPlaceAvailable(x,y))
    {
        cell->setX(x);
	cell->setY(y);
	
	return true;
    }
    else
    {
        return false;
    }
}

Cell* Element::getCell()
{
    return cell;
}

bool Element::reachedBottom()
{
    if (table->isPlaceAvailable(cell->getX(),cell->getY()-1))
    {	
	return false;
    }
    else
    {
        return true;
    }
}