#ifndef border_hpp__
#define border_hpp__

#include "standarts.hpp"
#include "particle.hpp"
#include "sle.hpp"
#include <functional>

template<u dim>
class Border
{
public:
    std::array<Vector<dim,Length>,dim-1> base;
    Vector<dim,Length> start;    
    std::function<void(Border<dim> &b, Particle<dim> &p)> bounceS;
    Border(){};
    Border(const std::array<Vector<dim,Length>, dim-1>& _base, const Vector<dim,Length>& _start):base(_base), start(_start)
    {
        bounceS=std::function<void(Border<dim> &b, Particle<dim> &p)>([](Border<dim>&, Particle<dim>&){});
    }
    Border(const std::array<Vector<dim,Length>, dim-1>& _base, const Vector<dim,Length>& _start, Temperature T):base(_base), start(_start)
    {
        bounceS=std::function<void(Border<dim> &b, Particle<dim> &p)>([T](Border<dim>&, Particle<dim> &p){
            p.velocity=(p.velocity/((p.velocity*p.velocity).sqrt()))*(Coefficient(1.5l)*k*T/p.m).sqrt();
        });
    }

    Border(const Border& b):base(b.base),start(b.start),bounceS(b.bounceS){}

    Border& operator=(const Border& b)
    {
        this->base=b.base;
        this->bounceS=b.bounceS;
        this->start=b.start;
        return *this;
    }

    void setTemp(Temperature T)
    {
        bounceS=std::function<void(Border<dim> &b, Particle<dim> &p)>([T](Border<dim> &b, Particle<dim> &p){
            p.velocity=(p.velocity/((p.velocity*p.velocity).sqrt()))*(Coefficient(1.5l)*k*T/p.m).sqrt();
        });
    }

    void setAdiabat()
    {
        bounceS=std::function<void(Border<dim> &b, Particle<dim> &p)>([](Border<dim> &b, Particle<dim> &p){});
    }
};

template<u dim>
Time FindIntersection(Border<dim>& b, Particle<dim>& p)
{
    Vector<dim,Length> n=p.cord-b.start;
    for(u i=0;i<dim-1;i++) n-=projection(b.base[i], n);
    n=(n/((n*n).sqrt()))*p.radius;
    Vector<dim, Coefficient> Vr=SLE(b.base,b.start+n,p.cord,p.velocity).result();
    for(u i=0;i<dim-1;i++) if (Vr[i]>1) return std::numeric_limits<decltype(Vr[i].value)>::max();
    if (Vr[dim-1]<=0) return std::numeric_limits<decltype(Vr[dim-1].value)>::max();
    return Vr[dim-1]*Time(1);
}

template<u dim>
void DoBounce(Border<dim>& b, Particle<dim>& p)
{
    Vector<dim,Length> n=p.cord-b.start;
    for(u i=0;i<dim-1;i++) n-=projection(b.base[i], n);
    if (n*n*Coefficient(0.9999)>p.radius*p.radius) return;
    //std::cout<<"b";
    Vector<dim, Velocity> nvel=p.velocity;
    for(u i=0;i<dim-1;i++) nvel-=projection(b.base[i], nvel);
    if (n*nvel>0) return;
    p.velocity-=nvel*Coefficient(2);
    b.bounceS(b,p);
    
}

#endif