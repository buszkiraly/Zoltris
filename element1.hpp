#ifndef ELEMENT1_HPP
#define ELEMENT1_HPP

#include "cell.hpp"
#include "table.hpp"
#include "element.hpp"

class Element1
{
    Cell  *cell[4];
    Table *table;
public:
    Element(Table *table);
    ~Element(){}
   
};

#endif