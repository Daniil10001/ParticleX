#include<iostream>
#include"particle.hpp"
#include"sle.hpp"
#include"domain.hpp"

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
    //--------------------------------
    SLE<2> S(std::array<Vector<2,Coefficient>,1>({Vector<2,Coefficient>({0,1})}),Vector<2,Coefficient>({0,0}),
    Vector<2,Coefficient>({-3,2}),Vector<2,Coefficient>({1,-1}));
    std::cout<<S.result();
    //--------------------------------
    Domain<3> d({Length(1),Length(2),Length(3)});
    std::cout<<d<<'\n';
    d.addParticlesS(MolarMass(1)/Na,200,1e-10,20);
    d.prepare();
    for (u i=0;i<3;i++){
        for (u j=0;j<20;j++)
            std::cout<<(d.ptclPerDimSrt[i][j])->cord<<'\n';
        std::cout<<"\n";
    }
    //--------------------------------
    return 0;
}