#ifndef particle_hpp__
#define particle_hpp__

#include "vector.hpp"

template<u dim>
class Particle
{
    public:
    Vector<dim, Velocity> velocity;
    Vector<dim, Length> cord;
    Mass m;
    Length radius;
    Particle(){}
    Particle(Mass _m, Length _r,const Vector<dim,Length>& crd,const Vector<dim,Velocity>& vel):velocity(vel),cord(crd),m(_m),radius(_r){}
    Particle(const Particle& p):velocity(p.velocity),cord(p.cord),m(p.m),radius(p.radius){}
    Particle& operator=(const Particle& p)
    {
        this->velocity=p.velocity;
        this->cord=p.cord;
        this->m=p.m;
        this->radius=p.radius;
        return *this;
    }
};

template<typename typeA, typename typeB> //(At-B)^2
class Quad
{
    public:
    decltype(typeA()*typeA()) a;
    decltype(typeA()*typeB()) b;
    decltype(typeB()*typeB()) c; 
    
    Quad(const typeA& A,const typeB& B):a(A*A),b(2*A*B),c(B*B){}
    
    template<u dim>
    Quad(const Vector<dim,typeA>& A, const Vector<dim,typeB>& B):a(0),b(0),c(0)
    {
        #pragma GCC ivdep
        for (u i=0;i<dim;i++)
        {
            a+=A[i]*A[i];
            b+=A[i]*B[i]*Coefficient(2);
            c+=B[i]*B[i];
        }
    }

    Vector<2, decltype(typeB()/typeA())> findInter()
    {
        decltype(typeB()/typeA()) x1,x2;
        //decltype(a.value) _a=a.value,_b=b.value,_c=c.value;
        if (b*b-Coefficient(4)*a*c<0 || a==0) return Vector<2, decltype(typeB()/typeA())>({-1,-1});
        x1=(-b-(b*b-Coefficient(4)*a*c).sqrt())/(Coefficient(2)*a);
        x2=(-b+(b*b-Coefficient(4)*a*c).sqrt())/(Coefficient(2)*a);
        return Vector<2, decltype(typeB()/typeA())>({x1,x2});
    }
};

template<u dim>
Time FindIntersection(Particle<dim>& p1, Particle<dim>& p2)
{
    if (abs(p1.cord-p2.cord)==0) return std::numeric_limits<decltype(p1.m.value)>::max();
    Quad<Velocity, Length> q(p1.velocity-p2.velocity,p1.cord-p2.cord);
    q.c-=(p1.radius+p2.radius)*(p1.radius+p2.radius);
    Vector<2,Time> t_inter=q.findInter();
    //std::cout<<"\n"<<q.a<<" "<<q.b<<" "<<q.c<<"\n";
    //std::cout<<t_inter<<' ';
    if (t_inter.v[0]<1e-12) return std::numeric_limits<decltype(q.c.value)>::max();
    //std::cout<<t_inter.v[0]<<" "<<t_inter.v[1]<<'\n';
    return t_inter.v[0];//+(t_inter.v[1]-t_inter.v[0])*Coefficient(0.01);
}

template<u dim>
void DoBounce(Particle<dim>& p1, Particle<dim>& p2)
{
    Vector<dim,Length> a=p1.cord-p2.cord;
    
    if (a*a<=0) return;
    
    if ((a*a).sqrt()>(p1.radius+p2.radius)*Coefficient(1.0001)) return;
    //std::cout<<a<<" "<<a*a<<'\n';
    Vector<dim, Velocity> pr1=projection(a,p1.velocity),pr2=projection(a,p2.velocity);
    //std::cout<<"b2";
    if ((pr1-pr2)*a>=0) return;
    auto v1n=(pr2*Coefficient(2)*p2.m+pr1*(p1.m-p2.m))/(p1.m+p2.m);
    auto v2n=(pr1*Coefficient(2)*p1.m-pr2*(p1.m-p2.m))/(p1.m+p2.m);
    /*if (p1.m==p2.m)
    {
        v1n=pr2;
        v2n=pr1;
    }*///std::cout<<a*a<<" "<<(p1.radius+p2.radius)<<"\n";
    //std::cout<<v1n<<" "<<pr1<<" "<<pr2<<" "<<p1.velocity<<" "<<p2.velocity<<" "<<&p2<<'\n';
    //int j;
    //std::cin>>j;
    //Energy E=p1.velocity*p1.velocity*p1.m+p2.velocity*p2.velocity*p2.m;
    p1.velocity=p1.velocity-pr1+v1n;
    p2.velocity=p2.velocity-pr2+v2n;
    //Energy E2=p1.velocity*p1.velocity*p1.m+p2.velocity*p2.velocity*p2.m;
    //std::cout<<E-E2<<'\n';
    /*p1.velocity=p1.velocity*(E/E2).sqrt();
    p2.velocity=p2.velocity*(E/E2).sqrt();*/
    //std::cout<<"b\n";
}

#endif