#ifndef solver_hpp__
#define solver_hpp__

#include"standarts.hpp"
#include"domain.hpp"

#define NPROC 4

template<u dim, u crd>
bool cmpPBCrdL(Particle<dim>* a,Length b)
{
    return a->cord[crd]<b;
}

template<u dim, u crd>
bool cmpPBCrdU(Length b,Particle<dim>* a)
{
    return b<a->cord[crd];
}

template<u dim>
class Solver
{
public:
    Domain<dim> d;
    Time now;
    Time delt_t;
    
    Solver(const Domain<dim>& _d):d(_d),now(0){
        if (d.State!=1) throw std::invalid_argument("get no initialized domain");
    }

    Time findInters(Particle<dim>& p)
    {
        Time t=std::numeric_limits<decltype(p.m.value)>::max();
        Length l=abs(p.velocity*delt_t)*Coefficient(2.5);
        [&]<size_t... I>(std::index_sequence<I...>){
        ([&](){
        auto its=std::lower_bound(d.ptclPerDimSrt[I].begin(),d.ptclPerDimSrt[I].end(),p.cord[I]-l,cmpPBCrdL<dim,I>);
        auto ite=std::upper_bound(d.ptclPerDimSrt[I].begin(),d.ptclPerDimSrt[I].end(),p.cord[I]+l,cmpPBCrdU<dim,I>);
        for (auto it=its;it!=ite;it++)
        {
            t=std::min(FindIntersection(**it,p),t);
        }
        }(),...);}(std::make_index_sequence<dim>{});
        for (Border<dim>& b: d.borders)
        {
            t=std::min(FindIntersection(b,p),t);
        }
        return t;
    }

    void Bounses(Particle<dim>& p)
    {
        Length l=p.radius*Coefficient(4);
        [&]<size_t... I>(std::index_sequence<I...>){
        ([&](){
        auto its=std::lower_bound(d.ptclPerDimSrt[I].begin(),d.ptclPerDimSrt[I].end(),p.cord[I]-l,cmpPBCrdL<dim,I>);
        auto ite=std::upper_bound(d.ptclPerDimSrt[I].begin(),d.ptclPerDimSrt[I].end(),p.cord[I]+l,cmpPBCrdU<dim,I>);
        for (auto it=its;it!=ite;it++)
        {
            DoBounce(**it,p);
        }
        }(),...);}(std::make_index_sequence<dim>{});
        for (Border<dim>& b: d.borders)
        {
            DoBounce(b,p);
        }
    }

    void solve(Time delt, Time dw, std::ostream& os)
    {
        Time _now=now, pw=now-dw;
        d.prepare();
        std::vector<Time> ts;
        std::vector<std::thread> ths;
        ul fi;
        while (now-_now<delt)
        {
            if (now-pw>=dw)
            {
                std::cout<<'\n'<<"dt:"<<delt_t<<" "<<now<<'\n';
                os<<now<<"|";
                for (ul i=0;i<d.ptcls.size();i++)
                    os<<d.ptcls[i].cord<<" "<<d.ptcls[i].velocity*d.ptcls[i].velocity<<";";
                os<<std::endl;
                pw=now;
            }
            delt_t=std::min(Time(1e-2),dw-(now-pw));
            for (ul i=0;i<d.ptcls.size();)
            {
                fi=i;
                ts=std::vector<Time>(NPROC,std::numeric_limits<long double>::infinity());
                ths.clear();
                while (i-fi<NPROC && i<d.ptcls.size())
                {
                    ths.emplace_back([&,i_c=i](){ts[i_c-fi]=this->findInters(d.ptcls[i_c]);});
                    i++;
                }
                for (std::thread& th: ths) th.join();
                delt_t=std::min(delt_t,*std::min_element(ts.begin(),ts.end()));
            }
            //std::cout<<'\n'<<"dt:"<<delt_t<<" "<<now<<'\n';
            for (ul i=0;i<d.ptcls.size();i++) d.ptcls[i].cord+=d.ptcls[i].velocity*delt_t;
            d.prepare();
            for (ul i=0;i<d.ptcls.size();i++) Bounses(d.ptcls[i]);
            now+=delt_t;
            //std::cout<<now<<'\n';
        }
        if (now-pw>dw)
        {
            os<<now<<"|";
            for (ul i=0;i<d.ptcls.size();i++)
                os<<d.ptcls[i].cord<<" "<<d.ptcls[i].velocity*d.ptcls[i].velocity*d.ptcls[i].m<<";";
            os<<std::endl;
            pw=now;
        }
    }
};


#endif