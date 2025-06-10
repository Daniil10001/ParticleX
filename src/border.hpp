#ifndef border_hpp__
#define border_hpp__

#include "standarts.hpp"
#include "particle.hpp"
#include "sle.hpp"

template<u dim>
class Border
{
public:
    Vector<dim,Length> start;
    std::array<Vector<dim,Length>,dim-1> base;
};

template<u dim>
Time FindIntersection(Border<dim>& b, Particle<dim>& p)
{
    Vector<dim, Coefficient> Vr=SLE(b.base,b.start,p.cord,p.velocity).result();
}


#endif