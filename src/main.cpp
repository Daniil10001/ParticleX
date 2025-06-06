#include<iostream>
#include"particle.hpp"

int main()
{
    Vector<3, Velocity> v({1,2,3});
    Time t=10;
    std::cout<<v*v;
    v+=v;
    std::cout<<" "<<v*v<<" "<<(v*t)<<'\n';
    Particle<2> a(0,0.01,{0,1},{1,0});
    Particle<2> b(0,0.01,{1,0},{0,1});
    std::cout<<FindIntersection(a,b)<<'\n';
    return 0;
}