#ifndef vector_hpp__
#define vector_hpp__

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

    Vector& operator=(const Vector& v2)
    {
        this->v=v2.v;
        return *this;
    }

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

    Vector operator-() const
    {
        return Coefficient(-1)*(*this);
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
    return os;
}




#endif