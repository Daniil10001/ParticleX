#ifndef sle_hpp__
#define sle_hpp__

#include "vector.hpp"

template<u dim>
class SLE
{
    public:

    /*

    system 
               (crds) (dp)
    (base|vel)*(----)=(dp)
               ( dt ) (dp)
    
    */
    
    std::array<Vector<dim,Coefficient>,dim> A;
    Vector<dim,Coefficient> b;

    template<typename t1, typename t2>
    SLE(const std::array<Vector<dim,t1>, dim-1>& base,const Vector<dim,t1>& pose_border,const Vector<dim,t1>& pose_molecule,const Vector<dim,t2>& vel_vector)
    {
        #pragma GCC ivdep
        for (u i=0;i<dim-1;i++) A[i]=base[i]/t1(1);
        A[dim-1]=vel_vector/t2(-1);
        for (u i=1;i<dim;i++)
            for (u j=0;j<i;j++) std::swap(A[i][j],A[j][i]); // now this is system of rows
        b=(pose_molecule-pose_border)/t2(1);
    }

    Vector<dim,Coefficient> result()
    {
        /*for (u i=0;i<dim;i++)
                std::cout<<A[i]<<'\n';
            std::cout<<'\n';*/
        for (u i = 0; i < dim; i++)
        {
            if (A[i][i]==0)
                for (u j=i+1;j<dim;j++)
                if (A[j][i]!=0) {
                std::swap(A[i],A[j]);
                std::swap(b[i],b[j]);
                //std::cout<<i<<" "<<j<<'\n';
                break;
            }
            if (A[i][i]==0) return Vector<dim,Coefficient>();
            b[i]=b[i]/A[i][i];
            A[i]=A[i]/A[i][i];
            for (u j=i+1;j<dim;j++)
            {
                b[j]-=b[i]*A[j][i];
                A[j]-=A[i]*A[j][i];
            }
        }
        for (u i = dim-1; i >= 0; i--)
        {
            if (i>0)
            for (u j=i-1;j>=0;j--)
            {
                b[j]-=(b[i]/(A[i][i]))*A[j][i];
                A[j]-=(A[i]/(A[i][i]))*A[j][i];
                if (j==0) break;
            }
            if (i==0) break;
        }
        /*for (u i=0;i<dim;i++)
                std::cout<<A[i]<<'\n';
            std::cout<<'\n';*/
        return b;
    }
};

#endif