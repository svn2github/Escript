
/*****************************************************************************
*
* Copyright (c) 2003-2016 by The University of Queensland
* http://www.uq.edu.au
*
* Primary Business: Queensland, Australia
* Licensed under the Open Software License version 3.0
* http://www.opensource.org/licenses/osl-3.0.php
*
* Development until 2012 by Earth Systems Science Computational Center (ESSCC)
* Development 2012-2013 by School of Earth Sciences
* Development from 2014 by Centre for Geoscience Computing (GeoComp)
*
*****************************************************************************/


#if !defined escript_LocalOps_H
#define escript_LocalOps_H
#include <cmath>
#include <complex>
#include "UnaryFuncs.h"
#include "DataTypes.h"
#include "DataException.h"
#ifndef M_PI
#   define M_PI           3.14159265358979323846  /* pi */
#endif


/**
\file LocalOps.h 
\brief Describes binary operations performed on double*.

For operations on DataAbstract see BinaryOp.h.
For operations on DataVector see DataMaths.h.
*/

namespace escript {


typedef enum
{
SINF,
COSF,
TANF,
ASINF,
ACOSF,
ATANF,
SINHF,
COSHF,
TANHF,
ERFF,
ASINHF,
ACOSHF,
ATANHF,
LOG10F,
LOGF,
SIGNF,
ABSF,
EXPF,
SQRTF,
POWF,
PLUSF,
MINUSF,
MULTIPLIESF,
DIVIDESF,
LESSF,
GREATERF,
GREATER_EQUALF,
LESS_EQUALF,
EQZEROF,
NEQZEROF,
GTZEROF,
GEZEROF,
LTZEROF,
LEZEROF,
CONJF
} ESFunction;



/**
\brief acts as a wrapper to isnan.
\warning if compiler does not support FP_NAN this function will always return false.
*/
inline
bool nancheck(double d)
{
		// Q: so why not just test d!=d?
		// A: Coz it doesn't always work [I've checked].
		// One theory is that the optimizer skips the test.
    return std::isnan(d);	// isNan should be a function in C++ land
}

/**
\brief returns a NaN.
\warning Should probably only used where you know you can test for NaNs
*/
inline
double makeNaN()
{
#ifdef nan
    return nan("");
#else
    return sqrt(-1.);
#endif

}


/**
   \brief
   solves a 1x1 eigenvalue A*V=ev*V problem

   \param A00 Input - A_00
   \param ev0 Output - eigenvalue
*/
inline
void eigenvalues1(const double A00,double* ev0) {

   *ev0=A00;

}
/**
   \brief
   solves a 2x2 eigenvalue A*V=ev*V problem for symmetric A

   \param A00 Input - A_00
   \param A01 Input - A_01
   \param A11 Input - A_11
   \param ev0 Output - smallest eigenvalue
   \param ev1 Output - largest eigenvalue
*/
inline
void eigenvalues2(const double A00,const double A01,const double A11,
                 double* ev0, double* ev1) {
      const double trA=(A00+A11)/2.;
      const double A_00=A00-trA;
      const double A_11=A11-trA;
      const double s=sqrt(A01*A01-A_00*A_11);
      *ev0=trA-s;
      *ev1=trA+s;
}
/**
   \brief
   solves a 3x3 eigenvalue A*V=ev*V problem for symmetric A

   \param A00 Input - A_00
   \param A01 Input - A_01
   \param A02 Input - A_02
   \param A11 Input - A_11
   \param A12 Input - A_12
   \param A22 Input - A_22
   \param ev0 Output - smallest eigenvalue
   \param ev1 Output - eigenvalue
   \param ev2 Output - largest eigenvalue
*/
inline
void eigenvalues3(const double A00, const double A01, const double A02,
                                   const double A11, const double A12,
                                                     const double A22,
                 double* ev0, double* ev1,double* ev2) {

      const double trA=(A00+A11+A22)/3.;
      const double A_00=A00-trA;
      const double A_11=A11-trA;
      const double A_22=A22-trA;
      const double A01_2=A01*A01;
      const double A02_2=A02*A02;
      const double A12_2=A12*A12;
      const double p=A02_2+A12_2+A01_2+(A_00*A_00+A_11*A_11+A_22*A_22)/2.;
      if (p<=0.) {
         *ev2=trA;
         *ev1=trA;
         *ev0=trA;

      } else {
         const double q=(A02_2*A_11+A12_2*A_00+A01_2*A_22)-(A_00*A_11*A_22+2*A01*A12*A02);
         const double sq_p=sqrt(p/3.);
         double z=-q/(2*pow(sq_p,3));
         if (z<-1.) {
            z=-1.;
         } else if (z>1.) {
            z=1.;
         }
         const double alpha_3=acos(z)/3.;
         *ev2=trA+2.*sq_p*cos(alpha_3);
         *ev1=trA-2.*sq_p*cos(alpha_3+M_PI/3.);
         *ev0=trA-2.*sq_p*cos(alpha_3-M_PI/3.);
      }
}
/**
   \brief
   solves a 1x1 eigenvalue A*V=ev*V problem for symmetric A

   \param A00 Input - A_00
   \param ev0 Output - eigenvalue
   \param V00 Output - eigenvector
   \param tol Input - tolerance to identify to eigenvalues
*/
inline
void  eigenvalues_and_eigenvectors1(const double A00,double* ev0,double* V00,const double tol)
{
      eigenvalues1(A00,ev0);
      *V00=1.;
      return;
}
/**
   \brief
   returns a non-zero vector in the kernel of [[A00,A01],[A01,A11]] assuming that the kernel dimension is at least 1.

   \param A00 Input - matrix component
   \param A10 Input - matrix component
   \param A01 Input - matrix component
   \param A11 Input - matrix component
   \param V0 Output - vector component
   \param V1 Output - vector component
*/
inline
void  vectorInKernel2(const double A00,const double A10,const double A01,const double A11,
                      double* V0, double*V1)
{
      double absA00=fabs(A00);
      double absA10=fabs(A10);
      double absA01=fabs(A01);
      double absA11=fabs(A11);
      double m=absA11>absA10 ? absA11 : absA10;
      if (absA00>m || absA01>m) {
         *V0=-A01;
         *V1=A00;
      } else {
         if (m<=0) {
           *V0=1.;
           *V1=0.;
         } else {
           *V0=A11;
           *V1=-A10;
         }
     }
}
/**
   \brief
   returns a non-zero vector in the kernel of [[A00,A01,A02],[A10,A11,A12],[A20,A21,A22]]
   assuming that the kernel dimension is at least 1 and A00 is non zero.

   \param A00 Input - matrix component
   \param A10 Input - matrix component
   \param A20 Input - matrix component
   \param A01 Input - matrix component
   \param A11 Input - matrix component
   \param A21 Input - matrix component
   \param A02 Input - matrix component
   \param A12 Input - matrix component
   \param A22 Input - matrix component
   \param V0 Output - vector component
   \param V1 Output - vector component
   \param V2 Output - vector component
*/
inline
void  vectorInKernel3__nonZeroA00(const double A00,const double A10,const double A20,
                                const double A01,const double A11,const double A21,
                                const double A02,const double A12,const double A22,
                                double* V0,double* V1,double* V2)
{
    double TEMP0,TEMP1;
    const double I00=1./A00;
    const double IA10=I00*A10;
    const double IA20=I00*A20;
    vectorInKernel2(A11-IA10*A01,A12-IA10*A02,
                    A21-IA20*A01,A22-IA20*A02,&TEMP0,&TEMP1);
    *V0=-(A10*TEMP0+A20*TEMP1);
    *V1=A00*TEMP0;
    *V2=A00*TEMP1;
}

/**
   \brief
   solves a 2x2 eigenvalue A*V=ev*V problem for symmetric A. Eigenvectors are
   ordered by increasing value and eigen vectors are normalizeVector3d such that
   length is zero and first non-zero component is positive.

   \param A00 Input - A_00
   \param A01 Input - A_01
   \param A11 Input - A_11
   \param ev0 Output - smallest eigenvalue
   \param ev1 Output - eigenvalue
   \param V00 Output - eigenvector componenent coresponding to ev0
   \param V10 Output - eigenvector componenent coresponding to ev0
   \param V01 Output - eigenvector componenent coresponding to ev1
   \param V11 Output - eigenvector componenent coresponding to ev1
   \param tol Input - tolerance to identify to eigenvalues
*/
inline
void  eigenvalues_and_eigenvectors2(const double A00,const double A01,const double A11,
                                    double* ev0, double* ev1,
                                    double* V00, double* V10, double* V01, double* V11,
                                    const double tol)
{
     double TEMP0,TEMP1;
     eigenvalues2(A00,A01,A11,ev0,ev1);
     const double absev0=fabs(*ev0);
     const double absev1=fabs(*ev1);
     double max_ev=absev0>absev1 ? absev0 : absev1;
     if (fabs((*ev0)-(*ev1))<tol*max_ev) {
        *V00=1.;
        *V10=0.;
        *V01=0.;
        *V11=1.;
     } else {
        vectorInKernel2(A00-(*ev0),A01,A01,A11-(*ev0),&TEMP0,&TEMP1);
        const double scale=1./sqrt(TEMP0*TEMP0+TEMP1*TEMP1);
        if (TEMP0<0.) {
            *V00=-TEMP0*scale;
            *V10=-TEMP1*scale;
            if (TEMP1<0.) {
               *V01=  *V10;
               *V11=-(*V00);
            } else {
               *V01=-(*V10);
               *V11= (*V00);
            }
        } else if (TEMP0>0.) {
            *V00=TEMP0*scale;
            *V10=TEMP1*scale;
            if (TEMP1<0.) {
               *V01=-(*V10);
               *V11= (*V00);
            } else {
               *V01= (*V10);
               *V11=-(*V00);
            }
        } else {
           *V00=0.;
           *V10=1;
           *V11=0.;
           *V01=1.;
       }
   }
}
/**
   \brief
   nomalizes a 3-d vector such that length is one and first non-zero component is positive.

   \param V0 - vector componenent
   \param V1 - vector componenent
   \param V2 - vector componenent
*/
inline
void  normalizeVector3(double* V0,double* V1,double* V2)
{
    double s;
    if (*V0>0) {
        s=1./sqrt((*V0)*(*V0)+(*V1)*(*V1)+(*V2)*(*V2));
        *V0*=s;
        *V1*=s;
        *V2*=s;
    } else if (*V0<0)  {
        s=-1./sqrt((*V0)*(*V0)+(*V1)*(*V1)+(*V2)*(*V2));
        *V0*=s;
        *V1*=s;
        *V2*=s;
    } else {
        if (*V1>0) {
            s=1./sqrt((*V1)*(*V1)+(*V2)*(*V2));
            *V1*=s;
            *V2*=s;
        } else if (*V1<0)  {
            s=-1./sqrt((*V1)*(*V1)+(*V2)*(*V2));
            *V1*=s;
            *V2*=s;
        } else {
            *V2=1.;
        }
    }
}
/**
   \brief
   solves a 2x2 eigenvalue A*V=ev*V problem for symmetric A. Eigenvectors are
   ordered by increasing value and eigen vectors are normalizeVector3d such that
   length is zero and first non-zero component is positive.

   \param A00 Input - A_00
   \param A01 Input - A_01
   \param A02 Input - A_02
   \param A11 Input - A_11
   \param A12 Input - A_12
   \param A22 Input - A_22
   \param ev0 Output - smallest eigenvalue
   \param ev1 Output - eigenvalue
   \param ev2 Output -
   \param V00 Output - eigenvector componenent coresponding to ev0
   \param V10 Output - eigenvector componenent coresponding to ev0
   \param V20 Output -
   \param V01 Output - eigenvector componenent coresponding to ev1
   \param V11 Output - eigenvector componenent coresponding to ev1
   \param V21 Output -
   \param V02 Output -
   \param V12 Output -
   \param V22 Output -
   \param tol Input - tolerance to identify to eigenvalues
*/
inline
void  eigenvalues_and_eigenvectors3(const double A00, const double A01, const double A02,
                                    const double A11, const double A12, const double A22,
                                    double* ev0, double* ev1, double* ev2,
                                    double* V00, double* V10, double* V20,
                                    double* V01, double* V11, double* V21,
                                    double* V02, double* V12, double* V22,
                                    const double tol)
{
      const double absA01=fabs(A01);
      const double absA02=fabs(A02);
      const double m=absA01>absA02 ? absA01 : absA02;
      if (m<=0) {
        double TEMP_V00,TEMP_V10,TEMP_V01,TEMP_V11,TEMP_EV0,TEMP_EV1;
        eigenvalues_and_eigenvectors2(A11,A12,A22,
                                      &TEMP_EV0,&TEMP_EV1,
                                      &TEMP_V00,&TEMP_V10,&TEMP_V01,&TEMP_V11,tol);
        if (A00<=TEMP_EV0) {
            *V00=1.;
            *V10=0.;
            *V20=0.;
            *V01=0.;
            *V11=TEMP_V00;
            *V21=TEMP_V10;
            *V02=0.;
            *V12=TEMP_V01;
            *V22=TEMP_V11;
            *ev0=A00;
            *ev1=TEMP_EV0;
            *ev2=TEMP_EV1;
        } else if (A00>TEMP_EV1) {
            *V02=1.;
            *V12=0.;
            *V22=0.;
            *V00=0.;
            *V10=TEMP_V00;
            *V20=TEMP_V10;
            *V01=0.;
            *V11=TEMP_V01;
            *V21=TEMP_V11;
            *ev0=TEMP_EV0;
            *ev1=TEMP_EV1;
            *ev2=A00;
        } else {
            *V01=1.;
            *V11=0.;
            *V21=0.;
            *V00=0.;
            *V10=TEMP_V00;
            *V20=TEMP_V10;
            *V02=0.;
            *V12=TEMP_V01;
            *V22=TEMP_V11;
            *ev0=TEMP_EV0;
            *ev1=A00;
            *ev2=TEMP_EV1;
        }
      } else {
         eigenvalues3(A00,A01,A02,A11,A12,A22,ev0,ev1,ev2);
         const double absev0=fabs(*ev0);
         const double absev1=fabs(*ev1);
         const double absev2=fabs(*ev2);
         double max_ev=absev0>absev1 ? absev0 : absev1;
         max_ev=max_ev>absev2 ? max_ev : absev2;
         const double d_01=fabs((*ev0)-(*ev1));
         const double d_12=fabs((*ev1)-(*ev2));
         const double max_d=d_01>d_12 ? d_01 : d_12;
         if (max_d<=tol*max_ev) {
             *V00=1.;
             *V10=0;
             *V20=0;
             *V01=0;
             *V11=1.;
             *V21=0;
             *V02=0;
             *V12=0;
             *V22=1.;
         } else {
            const double S00=A00-(*ev0);
            const double absS00=fabs(S00);
            if (absS00>m) {
                vectorInKernel3__nonZeroA00(S00,A01,A02,A01,A11-(*ev0),A12,A02,A12,A22-(*ev0),V00,V10,V20);
            } else if (absA02<m) {
                vectorInKernel3__nonZeroA00(A01,A11-(*ev0),A12,S00,A01,A02,A02,A12,A22-(*ev0),V00,V10,V20);
            } else {
                vectorInKernel3__nonZeroA00(A02,A12,A22-(*ev0),S00,A01,A02,A01,A11-(*ev0),A12,V00,V10,V20);
            }
            normalizeVector3(V00,V10,V20);;
            const double T00=A00-(*ev2);
            const double absT00=fabs(T00);
            if (absT00>m) {
                 vectorInKernel3__nonZeroA00(T00,A01,A02,A01,A11-(*ev2),A12,A02,A12,A22-(*ev2),V02,V12,V22);
            } else if (absA02<m) {
                 vectorInKernel3__nonZeroA00(A01,A11-(*ev2),A12,T00,A01,A02,A02,A12,A22-(*ev2),V02,V12,V22);
            } else {
                 vectorInKernel3__nonZeroA00(A02,A12,A22-(*ev2),T00,A01,A02,A01,A11-(*ev2),A12,V02,V12,V22);
            }
            const double dot=(*V02)*(*V00)+(*V12)*(*V10)+(*V22)*(*V20);
            *V02-=dot*(*V00);
            *V12-=dot*(*V10);
            *V22-=dot*(*V20);
            normalizeVector3(V02,V12,V22);
            *V01=(*V10)*(*V22)-(*V12)*(*V20);
            *V11=(*V20)*(*V02)-(*V00)*(*V22);
            *V21=(*V00)*(*V12)-(*V02)*(*V10);
            normalizeVector3(V01,V11,V21);
         }
   }
}

// General tensor product: arg_2(SL x SR) = arg_0(SL x SM) * arg_1(SM x SR)
// SM is the product of the last axis_offset entries in arg_0.getShape().
inline
void matrix_matrix_product(const int SL, const int SM, const int SR, const double* A, const double* B, double* C, int transpose)
{
  if (transpose == 0) {
    for (int i=0; i<SL; i++) {
      for (int j=0; j<SR; j++) {
        double sum = 0.0;
        for (int l=0; l<SM; l++) {
	  sum += A[i+SL*l] * B[l+SM*j];
        }
        C[i+SL*j] = sum;
      }
    }
  }
  else if (transpose == 1) {
    for (int i=0; i<SL; i++) {
      for (int j=0; j<SR; j++) {
        double sum = 0.0;
        for (int l=0; l<SM; l++) {
	  sum += A[i*SM+l] * B[l+SM*j];
        }
        C[i+SL*j] = sum;
      }
    }
  }
  else if (transpose == 2) {
    for (int i=0; i<SL; i++) {
      for (int j=0; j<SR; j++) {
        double sum = 0.0;
        for (int l=0; l<SM; l++) {
	  sum += A[i+SL*l] * B[l*SR+j];
        }
        C[i+SL*j] = sum;
      }
    }
  }
}

template <typename UnaryFunction>
inline void tensor_unary_operation(const int size,
			     const double *arg1,
			     double * argRes,
			     UnaryFunction operation)
{
  for (int i = 0; i < size; ++i) {
    argRes[i] = operation(arg1[i]);
  }
  return;
}

// ----------------------


// -------------------------------------

template <typename BinaryFunction, typename T, typename U, typename V>
inline void tensor_binary_operation(const int size,
			     const T *arg1,
			     const U *arg2,
			     V * argRes,
			     BinaryFunction operation)
{
  for (int i = 0; i < size; ++i) {
    argRes[i] = operation(arg1[i], arg2[i]);
  }
  return;
}

template <typename BinaryFunction, typename T, typename U, typename V>
inline void tensor_binary_operation(const int size,
			     T arg1,
			     const U *arg2,
			     V *argRes,
			     BinaryFunction operation)
{
  for (int i = 0; i < size; ++i) {
    argRes[i] = operation(arg1, arg2[i]);
  }
  return;
}

template <typename BinaryFunction, typename T, typename U, typename V>
inline void tensor_binary_operation(const int size,
			     const T *arg1,
			     U arg2,
			     V *argRes,
			     BinaryFunction operation)
{
  for (int i = 0; i < size; ++i) {
    argRes[i] = operation(arg1[i], arg2);
  }
  return;
}

// following the form of negate from <functional>
template <typename T>
struct sin_func
{
    T operator() (const T& x) const {return sin(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct cos_func
{
    T operator() (const T& x) const {return cos(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct tan_func
{
    T operator() (const T& x) const {return tan(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct asin_func
{
    T operator() (const T& x) const {return asin(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct acos_func
{
    T operator() (const T& x) const {return acos(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct atan_func
{
    T operator() (const T& x) const {return atan(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct sinh_func
{
    T operator() (const T& x) const {return sinh(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct cosh_func
{
    T operator() (const T& x) const {return cosh(x);}
    typedef T argument_type;
    typedef T result_type;
};


template <typename T>
struct tanh_func
{
    T operator() (const T& x) const {return tanh(x);}
    typedef T argument_type;
    typedef T result_type;
};

#if defined (_WIN32) && !defined(__INTEL_COMPILER)
#else
template <typename T>
struct erf_func
{
    T operator() (const T& x) const {return ::erf(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <>
struct erf_func<escript::DataTypes::cplx_t>		// dummy instantiation
{
    DataTypes::cplx_t operator() (const DataTypes::cplx_t& x) const {return makeNaN();}
    typedef DataTypes::cplx_t argument_type;
    typedef DataTypes::cplx_t result_type;
};

#endif
    
template <typename T>
struct asinh_func
{
    T operator() (const T& x) const
    {
#if defined (_WIN32) && !defined(__INTEL_COMPILER)
    return escript::asinh_substitute(x);
#else
    return asinh(x);
#endif      
    }
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct acosh_func
{
    T operator() (const T& x) const
    {
#if defined (_WIN32) && !defined(__INTEL_COMPILER)
    return escript::acosh_substitute(x);
#else
    return acosh(x);
#endif
    }
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct atanh_func
{
    T operator() (const T& x) const
    {
#if defined (_WIN32) && !defined(__INTEL_COMPILER)
    return escript::atanh_substitute(x);
#else
    return atanh(x);
#endif
    }    
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct log10_func
{
    T operator() (const T& x) const {return log10(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct log_func
{
    T operator() (const T& x) const {return log(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct sign_func
{
    T operator() (const T& x) const {return escript::fsign(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <>
struct sign_func<DataTypes::cplx_t>	// dummy instantiation
{
    DataTypes::cplx_t operator() (const DataTypes::cplx_t& x) const {return makeNaN();}
    typedef DataTypes::cplx_t argument_type;
    typedef DataTypes::cplx_t result_type;
};



template <typename T>
struct abs_func
{
    T operator() (const T& x) const {return fabs(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct exp_func
{
    T operator() (const T& x) const {return exp(x);}
    typedef T argument_type;
    typedef T result_type;
};

template <typename T>
struct sqrt_func
{
    T operator() (const T& x) const {return sqrt(x);}
    typedef T argument_type;
    typedef T result_type;
};
    
// following the form of plus from <functional>
template <typename T, typename U, typename V>
struct pow_func
{
    V operator() (const T& x, const U& y) const {return pow(static_cast<V>(x),static_cast<V>(y));}
    typedef T first_argument_type;
    typedef U second_argument_type;
    typedef V result_type;
};

// following the form of plus from <functional>
template <typename T, typename U, typename V>
struct plus_func
{
    V operator() (const T& x, const U& y) const {return x+y;}
    typedef T first_argument_type;
    typedef U second_argument_type;
    typedef V result_type;
};

template <typename T, typename U, typename V>
struct minus_func
{
    V operator() (const T& x, const U& y) const {return x-y;}
    typedef T first_argument_type;
    typedef U second_argument_type;
    typedef V result_type;
};

template <typename T, typename U, typename V>
struct multiplies_func
{
    V operator() (const T& x, const U& y) const {return x*y;}
    typedef T first_argument_type;
    typedef U second_argument_type;
    typedef V result_type;
};

template <typename T, typename U, typename V>
struct divides_func
{
    V operator() (const T& x, const U& y) const {return x/y;}
    typedef T first_argument_type;
    typedef U second_argument_type;
    typedef V result_type;
};


// using this instead of ::less because that returns bool and we need a result type of T
template <typename T>
struct less_func
{
    T operator() (const T& x, const T& y) const {return x<y;}
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef T result_type;
};

// using this instead of ::less because that returns bool and we need a result type of T
template <typename T>
struct greater_func
{
    T operator() (const T& x, const T& y) const {return x>y;}
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef T result_type;
};

template <typename T>
struct greater_equal_func
{
    T operator() (const T& x, const T& y) const {return x>=y;}
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef T result_type;
};

template <typename T>
struct less_equal_func
{
    T operator() (const T& x, const T& y) const {return x<=y;}
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef T result_type;
};

template <typename T>
struct gtzero_func
{
    T operator() (const T& x) const {return x>0;}
    typedef T first_argument_type;
    typedef T result_type;
};

template <>
struct gtzero_func<DataTypes::cplx_t>		// to keep the templater happy
{
    DataTypes::cplx_t operator() (const DataTypes::cplx_t& x) const {return makeNaN();}
    typedef DataTypes::cplx_t first_argument_type;
    typedef DataTypes::cplx_t result_type;
};



template <typename T>
struct gezero_func
{
    T operator() (const T& x) const {return x>=0;}
    typedef T first_argument_type;
    typedef T result_type;
};

template <>
struct gezero_func<DataTypes::cplx_t>		// to keep the templater happy
{
    DataTypes::cplx_t operator() (const DataTypes::cplx_t& x) const {return makeNaN();}
    typedef DataTypes::cplx_t first_argument_type;
    typedef DataTypes::cplx_t result_type;
};


template <typename T>
struct ltzero_func
{
    T operator() (const T& x) const {return x<0;}
    typedef T first_argument_type;
    typedef T result_type;
};

template <>
struct ltzero_func<DataTypes::cplx_t>		// to keep the templater happy
{
    DataTypes::cplx_t operator() (const DataTypes::cplx_t& x) const {return makeNaN();}
    typedef DataTypes::cplx_t first_argument_type;
    typedef DataTypes::cplx_t result_type;
};



template <typename T>
struct lezero_func
{
    T operator() (const T& x) const {return x>=0;}
    typedef T first_argument_type;
    typedef T result_type;
};

template <>
struct lezero_func<DataTypes::cplx_t>		// to keep the templater happy
{
    DataTypes::cplx_t operator() (const DataTypes::cplx_t& x) const {return makeNaN();}
    typedef DataTypes::cplx_t first_argument_type;
    typedef DataTypes::cplx_t result_type;
};


template <class IN, typename OUT, class UnaryFunction>
inline void tensor_unary_operation_helper(const int size,
                             const IN *arg1,
                             OUT * argRes,
                             UnaryFunction operation)
{

  for (int i = 0; i < size; ++i) {
    argRes[i] = operation(arg1[i]);
  }
}

// In most cases, IN and OUT will be the same
// but not ruling out putting Re() and Im()
// through this
template <class IN, typename OUT>
inline void tensor_unary_array_operation(const int size,
                             const IN *arg1,
                             OUT * argRes,
                             escript::ESFunction operation,
			     DataTypes::real_t tol=0)
{
  switch (operation)
  {
    case SINF: tensor_unary_operation_helper(size, arg1, argRes, sin_func<IN>()); break;
    case COSF: tensor_unary_operation_helper(size, arg1, argRes, cos_func<IN>()); break;
    case TANF: tensor_unary_operation_helper(size, arg1, argRes, tan_func<IN>()); break;
    case ASINF: tensor_unary_operation_helper(size, arg1, argRes, asin_func<IN>()); break;
    case ACOSF: tensor_unary_operation_helper(size, arg1, argRes, acos_func<IN>()); break;
    case ATANF: tensor_unary_operation_helper(size, arg1, argRes, atan_func<IN>()); break;
    case SINHF: tensor_unary_operation_helper(size, arg1, argRes, sinh_func<IN>()); break; 
    case COSHF: tensor_unary_operation_helper(size, arg1, argRes, cosh_func<IN>()); break;
    case TANHF: tensor_unary_operation_helper(size, arg1, argRes, tanh_func<IN>()); break;
    case ERFF: tensor_unary_operation_helper(size, arg1, argRes, erf_func<IN>()); break;
    case ASINHF: tensor_unary_operation_helper(size, arg1, argRes, asinh_func<IN>()); break;
    case ACOSHF: tensor_unary_operation_helper(size, arg1, argRes, acosh_func<IN>()); break;
    case ATANHF: tensor_unary_operation_helper(size, arg1, argRes, atanh_func<IN>()); break;
    case LOG10F: tensor_unary_operation_helper(size, arg1, argRes, log10_func<IN>()); break;
    case LOGF: tensor_unary_operation_helper(size, arg1, argRes, log_func<IN>()); break;
    case SIGNF: tensor_unary_operation_helper(size, arg1, argRes, sign_func<IN>()); break;
    case ABSF: tensor_unary_operation_helper(size, arg1, argRes, abs_func<IN>()); break;
    case EXPF: tensor_unary_operation_helper(size, arg1, argRes, exp_func<IN>()); break;
    case SQRTF: tensor_unary_operation_helper(size, arg1, argRes, sqrt_func<IN>()); break;
    
    case EQZEROF:   
	  for (int i = 0; i < size; ++i) {
	      argRes[i] = (fabs(arg1[i])<=tol);
	  }
	  break;
    case NEQZEROF: 
	  for (int i = 0; i < size; ++i) {
	      argRes[i] = (fabs(arg1[i])>tol);
	  }
	  break;
    case GTZEROF: tensor_unary_operation_helper(size, arg1, argRes, gtzero_func<IN>()); break;
    case GEZEROF: tensor_unary_operation_helper(size, arg1, argRes, gezero_func<IN>()); break;
    case LTZEROF: tensor_unary_operation_helper(size, arg1, argRes, ltzero_func<IN>()); break;
    case LEZEROF: tensor_unary_operation_helper(size, arg1, argRes, lezero_func<IN>()); break;   
    case CONJF: 
	  for (int i = 0; i < size; ++i) {
	      argRes[i] = static_cast<OUT>(std::conj(arg1[i]));
	  }
	  break;    
    default:
      throw DataException("Unsupported unary operation");
  }
  return;
}

bool supports_cplx(escript::ESFunction operation);


} // end of namespace
#endif
