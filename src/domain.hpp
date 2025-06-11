#ifndef domain_hpp__
#define domain_hpp__

#include "border.hpp"
#include "particle.hpp"
#include <utility> 
#include <algorithm>
#include <thread>
#include <exception>

// for n-d

template<u dim>
std::array<std::array<Vector<dim, Length>,dim-1>,2*dim> borderGenerator(const Vector<dim,Length>& lns_borders)
{
    std::array<std::array<Vector<dim, Length>,dim-1>,2*dim> res;
    for (u i=0;i<2*dim;i++)
    {
        u c=0;
        for (u j=0;j<dim;j++)
        {
            if (j==i%dim) continue;
            Vector<dim, Length> zr;
            zr[j]=lns_borders[j];
            res[i][c++]=zr*(i>=dim?Coefficient(-1):Coefficient(1));
        }
    }
    return res;
}

template<u dim, u crd>
bool sortPtclsByCord(Particle<dim>* a,Particle<dim>* b)
{
    return a->cord[crd]<b->cord[crd];
}

template<u dim>
class Domain
{
    public:
    std::array<Border<dim>,2*dim> borders;
    Vector<dim, Length> lns;
    std::array<Length, dim> start;
    std::vector<Particle<dim>> ptcls;
    std::array<std::vector<Particle<dim>*>,dim> ptclPerDimSrt;
    u State;

    Domain(const std::array<Length, dim>& lns_borders):lns(lns_borders),State(0){
        start.fill(1);
        auto brdrs=borderGenerator(lns);
        for (u i=0;i<dim;i++)
        {
            borders[i]=Border(brdrs[i],Vector<dim,Length>(start));
        }
        for (u i=dim;i<2*dim;i++)
        {
            borders[i]=Border(brdrs[i],Vector<dim,Length>(start)+lns);
        }
    }

    Domain(const Domain& d):borders(d.borders),lns(d.lns),start(d.start),ptcls(d.ptcls),ptclPerDimSrt(d.ptclPerDimSrt),State(d.State){}

    void prepare()
    {
        std::vector<std::thread> ths;
        [&]<size_t... I>(std::index_sequence<I...>){
        (ths.emplace_back([&](){std::sort(ptclPerDimSrt[I].begin(),ptclPerDimSrt[I].end(),sortPtclsByCord<dim,I>);}), ...);}
        (std::make_index_sequence<dim>{});
        for (std::thread& th: ths) th.join();
    }

    void addParticlesS(Mass m, Temperature T, Length radius, ul N)
    {
        if (State==1) throw std::runtime_error("You can add particles only one time!");
        State=1; 
        Velocity mV=(Coefficient(1.5)*(k*T/m)).sqrt();
        ptcls.resize(N);
        for (u d=0;d<dim;d++)
            ptclPerDimSrt[d].resize(N);
        ul Nc=std::lround(std::ceil(std::pow(N,1./dim)));
        for (ul i=0;i<N;i++)
        {
            u c=0;
            Vector<dim,Length> crd=Vector<dim,Length>(start)+lns*Coefficient(0.1);
            for (ul n=i;n>0;n/=Nc) 
            {
                crd[c]+=lns[c]*Coefficient((0.8L*(n%Nc))/Nc);
                c++;
            }
            
            ld fi1=unifRandDirection(),fi2=unifRandDirection();
            Vector<dim,Velocity> v({mV*Coefficient(cosl(fi1)*sinl(fi2)),
                    mV*Coefficient(cosl(fi1)*cosl(fi2)),mV*Coefficient(sinl(fi1))});
            ptcls[i]=Particle<dim>(m,radius,crd,v);
            for (u d=0;d<dim;d++)
                ptclPerDimSrt[d][i]=&ptcls[i];
            std::cout<<crd<<" "<<v<<'\n';
        }
    }
};

#include<ostream>
template<u dim>
std::ostream& operator<<(std::ostream& os, const Domain<dim>& d)
{
    os<<"Domain "<<&d<<" "<<d.lns<<'\n';
    for (u i=0;i<2*dim;i++)
    {
        os<<d.borders[i].start<<" | ";
        for (auto v: d.borders[i].base)
            os<<v<<' ';
        os<<'\n';
    }
    return os;
}

#endif