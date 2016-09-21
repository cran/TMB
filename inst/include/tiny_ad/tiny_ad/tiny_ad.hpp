// Copyright (C) 2016 Kasper Kristensen
// License: GPL-2

#ifndef TINY_AD_H
#define TINY_AD_H

/* Standalone ? */
#ifndef R_RCONFIG_H
#include <cmath>
#include <iostream>
#endif

/* Select the vector class to use (Default: tiny_vec) */
#if defined(TINY_AD_USE_STD_VALARRAY)
#include "tiny_valarray.hpp"
#define TINY_VECTOR(type,size) tiny_vector<type, size>
#elif defined(TINY_AD_USE_EIGEN_VEC)
#include <Eigen/Dense>
#define TINY_VECTOR(type,size) Eigen::Array<type, size, 1>
#else
#include "tiny_vec.hpp"
#define TINY_VECTOR(type,size) tiny_vec<type, size>
#endif

namespace tiny_ad {
  template<class Type, class Vector>
  struct ad {
    Type value;
    Vector deriv;
    ad(){}
    ad(Type v, Vector d){value = v; deriv = d;}
    ad(double v)        {value = v; deriv.setZero();}
    ad operator+ (const ad &other) const{
      return ad(value + other.value,
		deriv + other.deriv);
    }
    ad operator+ () const{
      return *this;
    }
    ad operator- (const ad &other) const{
      return ad(value - other.value,
		deriv - other.deriv);
    }
    ad operator- () const{
      return ad(-value, -deriv);
    }
    ad operator* (const ad &other) const{
      return ad(value * other.value,
		value * other.deriv +
		deriv * other.value);
    }
    ad operator/ (const ad &other) const{
      Type res = value / other.value;
      return ad(res,
		(deriv - res * other.deriv) /
		other.value );
    }
    /* Comparison operators */
#define COMPARISON_OPERATOR(OP)			\
    template<class other>			\
    bool operator OP (const other &x) const{	\
      return (value OP x);			\
    }
    COMPARISON_OPERATOR(<)
    COMPARISON_OPERATOR(>)
    COMPARISON_OPERATOR(<=)
    COMPARISON_OPERATOR(>=)
    COMPARISON_OPERATOR(==)
    COMPARISON_OPERATOR(!=)
#undef COMPARISON_OPERATOR
    /* Combine ad with other types (constants) */
    ad operator+ (const double &x) const{
      return ad(value + x, deriv);
    }
    ad operator- (const double &x) const{
      return ad(value - x, deriv);
    }
    ad operator* (const double &x) const{
      return ad(value * x, x * deriv);
    }
    ad operator/ (const double &x) const{
      return ad(value / x, deriv / x);
    }
    /* Note: 'this' and 'other' may point to the same object */
    ad& operator+=(const ad &other){
      value += other.value;
      deriv += other.deriv;
      return *this;
    }
    ad& operator-=(const ad &other){
      value -= other.value;
      deriv -= other.deriv;
      return *this;
    }
    ad& operator*=(const ad &other){
      if (this != &other) {
	deriv *= other.value;
	deriv += other.deriv * value;
	value *= other.value;
      } else {
	deriv *= value * 2.;
	value *= value;
      }
      return *this;
    }
    ad& operator/=(const ad &other){
      value /= other.value;
      deriv -= other.deriv * value;
      deriv /= other.value;
      return *this;
    }
  };
  /* Binary operators where a constant is first argument */
  template<class T, class V>
  ad<T, V> operator+ (const double &x, const ad<T, V> &y) {
    return y + x;
  }
  template<class T, class V>
  ad<T, V> operator- (const double &x, const ad<T, V> &y) {
    return -(y - x);
  }
  template<class T, class V>
  ad<T, V> operator* (const double &x, const ad<T, V> &y) {
    return y * x;
  }
  template<class T, class V>
  ad<T, V> operator/ (const double &x, const ad<T, V> &y) {
    T value = x / y.value;
    return ad<T, V>(value, (-value / y.value) * y.deriv);
  }
  /* Unary operators with trivial derivatives */
#define UNARY_MATH_ZERO_DERIV(F)		\
  template<class T, class V>			\
  double F (const ad<T, V> &x){			\
    return F(x.value);				\
  }
  using ::floor; using ::ceil;
  using ::trunc; using ::round;
  UNARY_MATH_ZERO_DERIV(floor)
  UNARY_MATH_ZERO_DERIV(ceil)
  UNARY_MATH_ZERO_DERIV(trunc)
  UNARY_MATH_ZERO_DERIV(round)
  template<class T>
  double sign(const T &x){return (x > 0) - (x < 0);}
  bool isfinite(const double &x)CSKIP( {return std::isfinite(x);} )
  template<class T, class V>
  bool isfinite(const ad<T, V> &x){return isfinite(x.value);}
#undef UNARY_MATH_ZERO_DERIV
  /* Unary operators with non-trivial derivatives */
#define UNARY_MATH_DERIVATIVE(F,DF)		\
  template<class T, class V>			\
  ad<T, V> F (const ad<T, V> &x){		\
    return ad<T, V>(F (x.value),		\
		    DF(x.value) * x.deriv);	\
  }
  using ::exp;  using ::log;
  using ::sin;  using ::cos;
  using ::sinh; using ::cosh;
  using ::sqrt; using ::fabs;
  UNARY_MATH_DERIVATIVE(exp, exp)
  UNARY_MATH_DERIVATIVE(log, 1.0/)
  UNARY_MATH_DERIVATIVE(sin, cos)
  UNARY_MATH_DERIVATIVE(cos, -sin)
  UNARY_MATH_DERIVATIVE(sinh, cosh)
  UNARY_MATH_DERIVATIVE(cosh, sinh)
  UNARY_MATH_DERIVATIVE(sqrt, 0.5/sqrt)
  UNARY_MATH_DERIVATIVE(fabs, sign)
  using ::expm1; using ::log1p;
  UNARY_MATH_DERIVATIVE(expm1, exp)
  template<class T> T D_log1p(const T &x) {return 1. / (x + 1.);}
  UNARY_MATH_DERIVATIVE(log1p, D_log1p)
#undef UNARY_MATH_DERIVATIVE
  /* A few more ... */
  template<class T, class V>
  ad<T, V> pow (const ad<T, V> &x, const ad<T, V> &y){
    return exp(y * log(x));
  }
  using ::pow;
  template<class T, class V>
  ad<T, V> pow (const ad<T, V> &x, const double &y){
    return ad<T, V> (pow(x.value, y), // Note: x.value could be 0
		     y * pow(x.value, y - 1.) * x.deriv);
  }
  /* Comparison operators where a constant is first argument */
#define COMPARISON_OPERATOR_FLIP(OP1, OP2)			\
  template<class T, class V>					\
  bool operator OP1 (const double &x, const ad<T, V> &y) {	\
    return y OP2 x;						\
  }
  COMPARISON_OPERATOR_FLIP(<,>)
  COMPARISON_OPERATOR_FLIP(<=,>=)
  COMPARISON_OPERATOR_FLIP(>,<)
  COMPARISON_OPERATOR_FLIP(>=,<=)
  COMPARISON_OPERATOR_FLIP(==,==)
  COMPARISON_OPERATOR_FLIP(!=,!=)
#undef COMPARISON_OPERATOR_FLIP
  /* Print method */
  template<class T, class V>
  std::ostream &operator<<(std::ostream &os, const ad<T, V> &x) {
    os << "{";
    os << " value=" << x.value;
    os << " deriv=" << x.deriv;
    os << "}";
    return os;
  }

  /* Interface to higher order derivatives. Example:

     typedef tiny_ad::variable<3, 2> Float; // Track 3rd order derivs wrt. 2 parameters
     Float a (1.23, 0);                     // Let a = 1.23 have parameter index 0
     Float b (2.34, 1);                     // Let b = 2.34 have parameter index 1
     Float y = sin(a + b);                  // Run the algorithm
     y.getDeriv();                          // Get all 3rd order derivatives
  */
#define VARIABLE(order,n) variable<order,n>
  template<int order, int n>
  struct variable : ad< VARIABLE(order-1, n),
			TINY_VECTOR( VARIABLE(order-1, n) , n) > {
    typedef ad< VARIABLE(order-1, n),
		TINY_VECTOR(VARIABLE(order-1, n), n) > Base;
    typedef variable<order-1, n> Type;
    static const int result_size = n * Type::result_size;
    variable() { /* Do not zero-initialize */ }
    variable(Base x) : Base(x) {}
    variable(double x) : Base(x) {}
    variable(double x, int id) : Base(x) {
      setid(id);
    }
    void setid(int i0, int count = 0){
      this->value.setid(i0, count);
      this->deriv[i0].setid(i0, count + 1);
    }
    TINY_VECTOR(double, result_size) getDeriv(){
      TINY_VECTOR(double, result_size) ans;
      int stride = result_size / n;
      for(int i=0; i<n; i++)
	ans.segment(i * stride, stride) = this->deriv[i].getDeriv();
      return ans;
    }
  };
#undef VARIABLE
  template<int n>
  struct variable<1, n> : ad<double, TINY_VECTOR(double,n) >{
    typedef ad<double, TINY_VECTOR(double,n) > Base;
    static const int result_size = n;
    variable<1, n>() { /* Do not zero-initialize */ }
    variable<1, n>(Base x) : Base(x) {}
    variable<1, n>(double x) : Base(x) {}
    variable<1, n>(double x, int id) : Base(x) {
      setid(id);
    }
    void setid(int i0, int count = 0){
      if(count == 0)
	this->deriv[i0] = 1.0;
      if(count == 1)
	this->value = 1.0;
    }
    TINY_VECTOR(double, n) getDeriv(){
      return this->deriv;
    }
  };
#undef TINY_VECTOR
} // End namespace tiny_ad

#endif
