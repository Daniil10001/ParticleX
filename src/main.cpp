#include<iostream>
#include<fstream>
#include <iomanip>
#include"particle.hpp"
#include"sle.hpp"
#include"domain.hpp"
#include "solver.hpp"

int main()
{
    Vector<3, Velocity> v({1,2,3});
    Time t=10;
    std::cout<<v*v;
    v+=v;
    std::cout<<" "<<v*v<<" "<<(v*t)<<'\n';
    Particle<2> a(0.1,2e-1,{0,1},{1,0});
    Particle<2> b(0.1,2e-1,{1,0},{0,1});
    Time T=FindIntersection(a,b);
    a.cord+=a.velocity*T;
    b.cord+=b.velocity*T;
    DoBounce(a,b);
    std::cout<<FindIntersection(a,b)<<'\n';
    std::cout<<a.cord<<" "<<b.cord<<'\n';
    std::cout<<a.velocity<<" "<<b.velocity<<'\n';
    //--------------------------------
    SLE<2> S(std::array<Vector<2,Coefficient>,1>({Vector<2,Coefficient>({0,1})}),Vector<2,Coefficient>({0,0}),
    Vector<2,Coefficient>({-3,2}),Vector<2,Coefficient>({1,-1}));
    std::cout<<S.result();
    //--------------------------------
    
    Domain<2> d({Length(10),Length(10)/*,Length(3)*/});
    std::cout<<d<<'\n';
    d.addParticlesM<2>({(PtclParametr){MolarMass(1),300,0.01,4000}});//,(PtclParametr){MolarMass(1)/Na,273,1e-6,2000}});
    d.prepare();
    /*for (u i=0;i<2;i++){
        for (u j=0;j<20;j++)
            std::cout<<(d.ptclPerDimSrt[i][j])->cord<<'\n';
        std::cout<<"\n";
    }*/
    //--------------------------------
    Solver<2> s(d);
    std::ofstream f;
    f<<std::setprecision(20);
    f.open("res.txt");
    s.solve(0.05,0.0001,f);
    f.close();
    
    return 0;
}