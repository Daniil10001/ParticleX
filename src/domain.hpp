#ifndef domain_hpp__
#define domain_hpp__

#include "border.hpp"
#include "particle.hpp"

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

template<u dim>
class Domain
{
    public:
    std::array<Border<dim>,2*dim> borders;
    Vector<dim, Length> lns;
    std::array<Length, dim> start;
    std::vector<Particle<dim>> ptcls;
    std::array<std::vector<Particle<dim>>,dim> ptclPerDimSrt;
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

    void addParticlesS(Mass m, Temperature T, Length radius, ul N)
    {
        Velocity mV=(Coefficient(1.5)*(k*T/m)).sqrt();
        ptcls.resize(N);
        ul Nc=std::lround(std::ceil(std::pow(N,1./dim)));
        for (ul i=0;i<N;i++)
        {
            u c=0;
            Vector<dim,Length> crd=start+0.1*lns;
            for (ul n=i;n>0;n/=Nc) 
            {
                crd[c]+=lns[c]*Coefficient((0.8L*(n%Nc))/Nc);
                c++;
            }
            ld fi1=unifRandDirection(),fi2=unifRandDirection();
            Vector<dim,Velocity> v({mV*Coefficient(cosl(fi1)*sinl(fi2)),
                    mV*Coefficient(cosl(fi1)*cosl(fi2)),mV*Coefficient(sinl(fi1))});
            ptcls=Particle<dim>(m,radius,crd,v);
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