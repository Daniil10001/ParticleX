#ifndef particle_hpp__
#define particle_hpp__

#include "standarts.hpp"
#include "types.hpp"
#include<array>
#include <cassert>
#include <type_traits>

template<u dim, typename type>
class Vector
{
    static_assert(is_DimQ_v<type>);
    public:
    std::array<type,dim> v;

    Vector(const std::initializer_list<type>& l)
    {
        assert(l.size()==dim);
        #pragma GCC ivdep
        for (unsigned int i=0;i<dim;i++)
            this->v[i]=*(l.begin()+i);
    }
    Vector(const Vector &V):v(V.v){}
    Vector(){v.fill(0);}

    type& operator[](std::size_t at) {return this->v[at];}
    const type& operator[](std::size_t at) const {return this->v[at];}

    decltype(type()*type()) operator*(const Vector&v2)
    {
        decltype(type()*type()) sp=0;
        #pragma GCC ivdep
        for (unsigned int i=0;i<dim;i++)
            sp+=this->v[i]*v2[i];
        return sp;
    }

    template<typename type2>
    Vector<dim, decltype(type()*type2())> operator*(const type2&q2)
    {
        Vector<dim, decltype(type()*type2())> v3;
        for (unsigned int i=0;i<dim;i++)
            v3[i]=this->v[i]*q2;
        return v3;
    }

    template<typename type2>
    Vector<dim, decltype(type()/type2())> operator/(const type2&q2)
    {
        Vector<dim, decltype(type()/type2())> v3;
        for (unsigned int i=0;i<dim;i++)
            v3[i]=this->v[i]/q2;
        return v3;
    }

    Vector operator+(const Vector &v2) const
    {
        Vector v3;
        #pragma GCC ivdep
        for (unsigned int i=0;i<dim;i++)
            v3[i]=this->v[i]+v2[i];
        return v3;
    }

    Vector& operator+=(const Vector &v2)
    {
        #pragma GCC ivdep
        for (unsigned int i=0;i<dim;i++)
            this->v[i]+=v2[i];
        return *this;
    }

    Vector operator-(const Vector &v2) const
    {
        Vector v3;
        #pragma GCC ivdep
        for (unsigned int i=0;i<dim;i++)
            v3[i]=this->v[i]-v2[i];
        return v3;
    }

    Vector& operator-=(const Vector &v2)
    {
        #pragma GCC ivdep
        for (unsigned int i=0;i<dim;i++)
            this->v[i]-=v2[i];
        return *this;
    }
};

#include<ostream>
template <u dim, typename type>
std::ostream& operator<<(std::ostream& os, const Vector<dim, type>& obj)
{
    os<<"<";
    for (unsigned int i=0;i<dim;i++)
        os<<obj.v[i].value<<(i!=dim-1?",":"");
    os<<">";
    return print_Dim(os,type());
}




template<u dim>
class Particle
{
    public:
    Vector<dim, Velocity> velocity;
    Vector<dim, Length> cord;
    Mass m;
    Length radius;
    Particle(Mass _m, Length _r,const std::initializer_list<Length>& crd,const std::initializer_list<Velocity>& vel):velocity(vel),cord(crd),m(_m),radius(_r){}
};

template<typename typeA, typename typeB> //(At-B)^2
class Quad
{
    public:
    decltype(typeA()*typeA()) a;
    decltype(typeA()*typeB()) b;
    decltype(typeB()*typeB()) c; 
    
    Quad(typeA A, typeB B):a(A*A),b(2*A*B),c(B*B){}
    
    template<u dim>
    Quad(Vector<dim,typeA> A, Vector<dim,typeB> B):a(0),b(0),c(0)
    {
        #pragma GCC ivdep
        for (unsigned int i=0;i<dim;i++)
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
    Quad<Velocity, Length> q(p1.velocity-p2.velocity,p1.cord-p2.cord);
    q.c-=(p1.radius+p2.radius)*(p1.radius+p2.radius);
    Vector<2,Time> t_inter=q.findInter();
    std::cout<<"\n"<<q.a<<" "<<q.b<<" "<<q.c<<"\n";
    std::cout<<t_inter<<' ';
    if (t_inter.v[0]<0) return std::numeric_limits<decltype(q.c.value)>::max();
    return t_inter.v[0];
}

#endif