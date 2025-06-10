#ifndef solver_hpp__
#define solver_hpp__

#include"standarts.hpp"
#include"particle.hpp"
#include<vector>
#include<array>


class Solver
{
private:
    
public:
    
};

template<u dim>
class Domain
{
    public:
    std::vector<Particle<dim>> pcs;
    std::array<const std::vector<Particle<dim>> const *,3> dpsort;

};

#endif