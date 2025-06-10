#ifndef types_hpp__
#define types_hpp__

#include<cmath>

template <int L, int M, int t, int T, int mu>
class DimQ
{
public:
    long double value;
    static constexpr const long double epsilon = 1e-9l;

    DimQ(long double _value) : value(_value) {}
    DimQ() : DimQ(0) {}

    // Шаблонный оператор изменения знака
    DimQ operator-() const
    {
        return DimQ(-this->value);
    }

    // Шаблонный оператор сложения
    DimQ operator+(const DimQ &q2) const
    {
        return DimQ(this->value + q2.value);
    }

    DimQ& operator+=(const DimQ &q2)
    {
        this->value += q2.value;
        return *this;
    }

    // Шаблонный оператор деления
    template <int L1, int M1, int t1, int T1, int mu1>
    DimQ<L - L1, M - M1, t - t1, T - T1, mu-mu1> operator/(const DimQ<L1, M1, t1, T1, mu1> &q2) const
    {
        return DimQ<L - L1, M - M1, t - t1, T - T1, mu-mu1>(this->value / q2.value);
    }


    DimQ<L/2,M/2,t/2,T/2,mu/2> sqrt()
    {
        static_assert(L%2==0);
        static_assert(M%2==0);
        static_assert(t%2==0);
        static_assert(T%2==0);
        static_assert(mu%2==0);
        return sqrtl(this->value);
    }

    // Шаблонный оператор вычитания
    DimQ operator-(const DimQ &q2) const
    {
        return DimQ(this->value - q2.value);
    }

    DimQ& operator-=(const DimQ &q2)
    {
        this->value -= q2.value;
        return *this;
    }

    // Шаблонный оператор умножения
    template <int L1, int M1, int t1, int T1, int mu1>
    DimQ<L + L1, M + M1, t + t1, T + T1, mu+mu1> operator*(const DimQ<L1, M1, t1, T1, mu1> &q2) const
    {
        return DimQ<L + L1, M + M1, t + t1, T + T1, mu+mu1>(this->value * q2.value);
    }

    bool operator>(const DimQ&q2) const
    {
        return this->value > q2.value;
    }

    bool operator<(const DimQ&q2) const
    {
        return this->value < q2.value;
    }

    bool operator==(const DimQ&q2) const
    {
        return std::abs(this->value - q2.value)/
            (std::abs(this->value)+std::abs(q2.value)+epsilon) < epsilon;
    }

    bool operator>=(const DimQ&q2) const
    {
        return *this > q2 || *this == q2;
    }

    bool operator<=(const DimQ&q2) const
    {
        return *this <= q2 || *this == q2;
    }

    bool operator!=(const DimQ&q2) const
    {
        return !(*this == q2);
    }
};

#include<ostream>
template <int L, int M, int t, int T, int mu>
std::ostream& print_Dim(std::ostream& os, const DimQ<L,M,t,T,mu>&)
{
    if (L!=0)
    os<<"m^"<<L;
    if (M!=0)
    os<<"kg^"<<M;
    if (t!=0)
    os<<"s^"<<t;
    if (T!=0)
    os<<"K^"<<T;
    if (mu!=0)
    os<<"mol^"<<mu;
    return os;
}

template <int L, int M, int t, int T, int mu>
std::ostream& operator<<(std::ostream& os, const DimQ<L,M,t,T,mu>& obj)
{
    os<<obj.value;
    return print_Dim(os,obj);
}

// Псевдонимы типов
typedef DimQ<0, 0, 0, 0, 0> Coefficient;
typedef DimQ<1, 0, 0, 0, 0> Length;
typedef DimQ<0, 1, 0, 0, 0> Mass;
typedef DimQ<0, 0, 1, 0, 0> Time;
typedef DimQ<0, 0, 0, 1, 0> Temperature;
typedef DimQ<0, 0, 0, 0, 1> Mol;

typedef DimQ<3, 0, 0, 0, 0> Volume;
typedef DimQ<2, 0, 0, 0, 0> Area;

// Производные типов
typedef decltype(Length() / Time()) Velocity;
typedef decltype(Velocity() / Time()) Acceleration;
typedef decltype(Acceleration() * Mass()) Force;
typedef decltype(Force()/Area()) Pressure;
typedef decltype(Force() * Length()) Energy;
typedef decltype(Energy() / Time()) Power;
typedef decltype(Mass()/Mol()) MolarMass;
typedef decltype(Velocity()*Mass()) Momentum;

// Константы
static const DimQ<0,0,0,0,-1> Na(6.02e23L);
static const decltype(Energy()/Temperature()) k(1.38e-23l);
static const auto R=Na*k;


// Базовый шаблон: false для любых типов
template <typename T>
struct is_DimQ : std::false_type {};

// Специализация для DimQ с 5 int-параметрами
template <int L, int M, int t, int T, int mu>
struct is_DimQ<DimQ<L, M, t, T, mu>> : std::true_type {};

template <typename T>
inline constexpr bool is_DimQ_v = is_DimQ<T>::value;
#endif