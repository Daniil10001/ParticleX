#ifndef border_hpp__
#define border_hpp__

#include "standarts.hpp"
#include "vector.hpp"

template<u dim>
class Border
{
public:
    Vector<dim,Length> start;
    std::array<Vector<dim,Length>,dim-1> base;



};



#endif