#ifndef domain_hpp__
#define domain_hpp__

#include "border.hpp"
#include "particle.hpp"
#include <utility>
#include <algorithm>
#include <thread>
#include <exception>

// for n-d

struct PtclParametr
{
    MolarMass mu;
    Temperature T;
    Length radius;
    ul N;
};

template <u dim>
std::array<std::array<Vector<dim, Length>, dim - 1>, 2 * dim> borderGenerator(const Vector<dim, Length> &lns_borders)
{
    std::array<std::array<Vector<dim, Length>, dim - 1>, 2 * dim> res;
    for (u i = 0; i < 2 * dim; i++)
    {
        u c = 0;
        for (u j = 0; j < dim; j++)
        {
            if (j == i % dim)
                continue;
            Vector<dim, Length> zr;
            zr[j] = lns_borders[j];
            res[i][c++] = zr * (i >= dim ? Coefficient(-1) : Coefficient(1));
        }
    }
    return res;
}

template <u dim, u crd>
bool cmpPBCrd(Particle<dim> *a, Particle<dim> *b)
{
    return a->cord[crd] < b->cord[crd];
}

template <u dim>
class Domain
{
public:
    std::array<Border<dim>, 2 * dim> borders;
    Vector<dim, Length> lns;
    Volume V;
    std::array<Length, dim> start;
    std::vector<Particle<dim>> ptcls;
    std::array<std::vector<Particle<dim> *>, dim> ptclPerDimSrt;
    u State;

    Domain(const std::array<Length, dim> &lns_borders) : lns(lns_borders), State(0)
    {
        start.fill(1);
        V = 1;
        for (u i = 0; i < dim; i++)
            V = V * lns[i] / Length(1);
        auto brdrs = borderGenerator(lns);
        for (u i = 0; i < dim; i++)
        {
            borders[i] = Border(brdrs[i], Vector<dim, Length>(start));
        }
        for (u i = dim; i < 2 * dim; i++)
        {
            borders[i] = Border(brdrs[i], Vector<dim, Length>(start) + lns);
        }
    }

    // Domain(const Domain& d):borders(d.borders),lns(d.lns),V(d.V),start(d.start),ptcls(d.ptcls),ptclPerDimSrt(d.ptclPerDimSrt),State(d.State){}

    void prepare()
    {
        std::vector<std::thread> ths;
        [&]<size_t... I>(std::index_sequence<I...>)
        { (ths.emplace_back([&]()
                            {
                                for (ul i = 0; i < ptcls.size(); i++)
                                    ptclPerDimSrt[I][i] = &ptcls[i];
                                std::sort(ptclPerDimSrt[I].begin(), ptclPerDimSrt[I].end(), cmpPBCrd<dim, I>);
                            }),
           ...); }(std::make_index_sequence<dim>{});
        for (std::thread &th : ths)
            th.join();
    }

    /*void addParticlesS(MolarMass mu, Temperature T, Length radius, ul N)
    {
        if (State == 1)
            throw std::runtime_error("You can add particles only one time!");
        State = 1;
        Velocity mV = (Coefficient(dim) * T* (R / mu)).sqrt();
        ptcls.resize(N);
        for (u d = 0; d < dim; d++)
            ptclPerDimSrt[d].resize(N);
        ul Nc = std::lround(std::ceil(std::pow(N, 1. / dim)));
        for (ul i = 0; i < N; i++)
        {
            u c = 0;
            Vector<dim, Length> crd = Vector<dim, Length>(start) + lns * Coefficient(0.1);
            for (ul n = i; n > 0; n /= Nc)
            {
                crd[c] += lns[c] * Coefficient((0.8L * (n % Nc)) / Nc);
                c++;
            }
            Vector<dim, Velocity> v;
            v[0] = mV;
            for (u np = 0; np < dim - 1; np++)
            {
                long double fi = unifRandDirection();
                v[np + 1] = v[np] * Coefficient(cosl(fi));
                v[np] = v[np] * Coefficient(sinl(fi));
            }
            ptcls[i] = Particle<dim>(mu/Na, radius, crd, v);
            for (u d = 0; d < dim; d++)
                ptclPerDimSrt[d][i] = &ptcls[i];
            std::cout << crd << " " << v << '\n';
        }
    }

    void addParticlesS(const PtclParametr &p) { addParticlesS(p.mu, p.T, p.radius, p.N); }*/

    template <u Np>
    void addParticlesM(const std::array<PtclParametr, Np> pms)
    {
        if (State == 1)
            throw std::runtime_error("You can add particles only one time!");
        State = 1;
        ul N = 0;
        for (u i = 0; i < Np; i++)
            N += pms[i].N;
        ptcls.resize(N);
        for (u d = 0; d < dim; d++)
            ptclPerDimSrt[d].resize(N);
        ul Nc = std::lround(std::ceil(std::pow(N, 1. / dim)));
        ul i = 0;
        for (u tp = 0; tp < Np; tp++)
        {
            Velocity mV = (Coefficient(dim) * pms[tp].T * R / pms[tp].mu).sqrt();
            for (ul _i = 0; _i < pms[tp].N; _i++)
            {

                u c = 0;
                Vector<dim, Length> crd = Vector<dim, Length>(start) + lns * Coefficient(0.01);
                for (ul n = i; n > 0; n /= Nc)
                {
                    crd[c] += lns[c] * Coefficient((0.98L * (n % Nc)) / Nc);
                    c++;
                }
                Vector<dim, Velocity> v;
                v[0] = mV;
                for (u np = 0; np < dim - 1; np++)
                {
                    long double fi = unifRandDirection();
                    v[np + 1] = v[np] * Coefficient(cosl(fi));
                    v[np] = v[np] * Coefficient(sinl(fi));
                }
                ptcls[i] = Particle<dim>(pms[tp].mu/Na, pms[tp].radius, crd, v);
                for (u d = 0; d < dim; d++)
                    ptclPerDimSrt[d][i] = &ptcls[i];
                std::cout << crd << " " << v << '\n';
                i++;
            }
        }
    }
};

#include <ostream>
template <u dim>
std::ostream &operator<<(std::ostream &os, const Domain<dim> &d)
{
    os << "Domain " << &d << " " << d.lns << '\n';
    for (u i = 0; i < 2 * dim; i++)
    {
        os << d.borders[i].start << " | ";
        for (auto v : d.borders[i].base)
            os << v << ' ';
        os << '\n';
    }
    return os;
}

#endif