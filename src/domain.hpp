#ifndef domain_hpp__
#define domain_hpp__

#include "border.hpp"
#include "particle.hpp"



template<u dim>
class Domain
{
    public:
    std::array<Border, > borders;

    Domain(){}
};

#endif