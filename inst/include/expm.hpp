// Copyright (C) 2013-2015 Kasper Kristensen
// License: GPL-2

namespace atomic{
  /* Matrix class with methods required by Pade approximation */
  template<class Type>
  struct Block {
    typedef Type ScalarType;
    matrix<Type> A;
    Block(){}
    Block(matrix<Type> A_){A=A_;}
    /* Matrix multiply */
    Block<Type> operator*(Block<Type> other){
      return Block( this->A * other.A );
    }
    /* Scale matrix */
    Block<Type> scale(Type c){
      return Block( c * this->A );
    }  
    /* Add identity matrix */
    Block<Type> addIdentity(){
      int n=A.rows();
      matrix<double> I(n,n);
      I.setIdentity();
      return Block( this->A + I );
    }
    /* Infinity norm */
    Type norm(){
      matrix<Type> Aabs(this->A.rows(),this->A.cols());
      Aabs = A.array().abs();
      vector<Type> rsAabs = Aabs.rowwise().sum();
      return rsAabs.maxCoeff();
    }  
    /* Matrix inverse */
    Block<Type> inverse(){
      return Block( this->A.inverse() );
    }
    /* Increment/decrement */
    Block<Type>& operator+=(Block<Type> x){ this->A += x.A; return *this; }
    Block<Type>& operator-=(Block<Type> x){ this->A -= x.A; return *this; }
    /* Methods for operator square root */
    typedef Eigen::SelfAdjointEigenSolver<
      Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> > SAES_t;
    /* Solve *special case* of Sylvester equation: XA+AX=Y */
    Block<Type> sylvester(Block<Type> Y) {
      SAES_t saes(A);
      matrix<Type> V = saes.eigenvectors();
      vector<Type> D = saes.eigenvalues();
      // Transform
      matrix<Type> Y_ = V.transpose() * Y.A * V;
      // Solve
      for (int i=0; i<Y_.rows(); i++)
        for (int j=0; j<Y_.cols(); j++)
          Y_(i, j) /= (D(i) + D(j));
      // Transform back
      matrix<Type> X = V * Y_ * V.transpose();
      return Block(X);
    }
    /* Solve *special case* of Sylvester equation: |A|X + X|A| = AY + YA */
    Block<Type> sylvester2(Block<Type> Y) {
      SAES_t saes(A);
      matrix<Type> V = saes.eigenvectors();
      vector<Type> D = saes.eigenvalues();
      // Transform
      matrix<Type> Y_ = V.transpose() * Y.A * V;
      // Solve
      for (int i=0; i<Y_.rows(); i++) {
        for (int j=0; j<Y_.cols(); j++) {
          Type denum = std::abs(D(i)) + std::abs(D(j));
          if (denum == Type(0)) denum = 1;
          Y_(i, j) *= (D(i) + D(j)) / denum;
        }
      }
      // Transform back
      matrix<Type> X = V * Y_ * V.transpose();
      return Block(X);
    }

    /* Operator square root (PD case only) */
    Block<Type> sqrtm() {
      SAES_t saes(A);
      matrix<Type> X = saes.operatorSqrt();
      return Block(X);
    }
    /* Operator absolute value (Symmetric case only) */
    Block<Type> absm() {
      SAES_t saes(A);
      matrix<Type> V = saes.eigenvectors();
      matrix<Type> X = V * saes.eigenvalues().cwiseAbs().asDiagonal() * V.transpose();
      return Block(X);
    }
  };
  
  /*
    Representation of matrix of the form
    
    T(A,B) :=
    
    [ A  0 ]
    [ B  A ]
    
    with methods required by Pade approximation
  */
  template<class BlockType>
  struct Triangle {
    BlockType A, B;
    typedef typename BlockType::ScalarType Type;
    Triangle(){}
    Triangle(BlockType A_, BlockType B_){A = A_; B = B_;}
    /* Matrix multiply. Cost = 3 multiplies */
    Triangle<BlockType> operator*(Triangle<BlockType> other){
      BlockType A, B;
      A  = (this->A) * (other.A);
      B  = (this->A) * (other.B);
      B += (this->B) * (other.A);
      return Triangle(A, B);
    }
    /* Scale matrix */
    Triangle<BlockType> scale(Type c){
      return Triangle(this->A.scale(c), this->B.scale(c));
    }  
    /* Add identity matrix */
    Triangle<BlockType> addIdentity(){
      return Triangle(this->A.addIdentity(), this->B);
    }
    /* Infinity norm (Strictly, the norm is larger. But we
       wan't the number of pade iterations for the derivative
       to be the same as for the function value) */
    Type norm(){
      return this->A.norm();
    }
    /* Matrix inverse. Cost = 1 inverse + 2 multiplies */
    Triangle<BlockType> inverse(){
      BlockType A = this->A.inverse();
      BlockType B = (A * (this->B * A)).scale(-1);
      return Triangle(A, B);
    }
    /* Increment/decrement */
    Triangle<BlockType>& operator+=(Triangle<BlockType> x){
      this->A += x.A;
      this->B += x.B;
      return *this;
    }
    Triangle<BlockType>& operator-=(Triangle<BlockType> x){
      this->A -= x.A;
      this->B -= x.B;
      return *this;
    }
    /* Methods for operator square root */
    Triangle<BlockType> sylvester(Triangle<BlockType> Y) {
      Triangle<BlockType> X;
      X.A = (*this).A.sylvester(Y.A);
      Y.B -= (*this).B * X.A;
      Y.B -= X.A * (*this).B;
      X.B = (*this).A.sylvester(Y.B);
      return X;
    }
    /* Solve |A| X + X |A| = A Y + Y A */
    Triangle<BlockType> sylvester2(Triangle<BlockType> Y) {
      Triangle<BlockType> Y2 = (*this) * Y;
      Y2 += Y * (*this);
      Triangle<BlockType>  X = (*this).absm().sylvester(Y2);
      return X;
    }
    Triangle<BlockType> sqrtm() {
      BlockType A = (*this).A.sqrtm();
      BlockType B = A.sylvester((*this).B);
      return Triangle(A, B);
    }
    Triangle<BlockType> absm() {
      BlockType A = (*this).A.absm();
      BlockType B = (*this).A.sylvester2((*this).B);
      return Triangle(A, B);
    }
  };
  
  /** \internal \brief Representation of 'block-binomial' matrix

    \verbatim
    Representation of 'block-binomial' matrix of the form
    
    T( T( T(A, B), T(C, 0) ), T( T(D, 0), T(0, 0) ) ) =
    
    [ A                      ]
    [ B  A                   ]
    [ C     A                ]
    [ .  C  B  A             ]
    [ D           A          ]
    [ .  D        B  A       ]
    [ .     D     C     A    ]
    [ .  .  .  D  .  C  B  A ]
    
    with methods required by Pade approximation
    \endverbatim
  */
  template <int n>
  struct nestedTriangle : Triangle<nestedTriangle<n-1> >{
    typedef double ScalarType;
    typedef nestedTriangle<n-1> BlockType;
    typedef Triangle<nestedTriangle<n-1> > Base;
    nestedTriangle(){}
    nestedTriangle(Base x) : Base(x){}
    nestedTriangle(vector<matrix<double> > args) : Base() {
      int nargs=args.size();
      vector<matrix<double> > args1 = args.head(nargs-1);
      matrix<double> zero = args[0]*0.0;
      vector<matrix<double> > args2(nargs-1);
      for(int i=0;i<nargs-1;i++)args2[i] = zero;
      args2[0] = args[nargs-1];
      Base::A = nestedTriangle<n-1>(args1);
      Base::B = nestedTriangle<n-1>(args2);
    }
    /* For easy recursive extraction */
    matrix<double> bottomLeftCorner(){
      return this->B.bottomLeftCorner();
    }
  };
  template <>
  struct nestedTriangle<0> : Block<double>{
    typedef Block<double> Base;
    nestedTriangle(){}
    nestedTriangle(Base x) : Base(x){}
    nestedTriangle(vector<matrix<double> > args) : Block<double>(args[0]) {}
    matrix<double> bottomLeftCorner(){
      return this->A;
    }
  };


  /* 
     Pade approximation of matrix exponential.
     Can be applied to any of the previously implemented matrix 
     classes.
  */
  template<class matrix_pade>
  matrix_pade expm(matrix_pade A){
    double log2NormInf = log( A.norm() );
    log2NormInf /= log(double(2.0));
    double e = std::floor(log2NormInf) + 1.0;
    double s = e + 1.0;
    s = (s  <  0  ?  0  :  s);
    matrix_pade AA = A.scale( 1.0 / pow( 2.0, s ) );
    matrix_pade X = AA;
    double c = 0.5;
    matrix_pade E = AA.scale( c).addIdentity();
    matrix_pade D = AA.scale(-c).addIdentity();
    int q = 8; /* R uses 8 - ADMB 6 */
    int p = 1;
    for(int k = 2;  k<=q; ++k){
      c *= double(q-k+1) / double(k*(2*q-k+1));
      X = AA * X;
      matrix_pade cX = X.scale(c);
      E += cX;
      if (p == 1) { D += cX; } else { D -= cX; }
      p = (p == 1) ? 0 : 1;
    }
    matrix_pade invD = D.inverse();
    E = invD * E;
    for(int k = 1; k<=s; k++){
      E = E * E;
    }
    return E;
  }

  /* Matrix exponential and derivatives up to (in principle) any order.
     Orders from 0 to 3 are compiled and the appropriate order is dispatched
     at run-time.
  */
  matrix<double> expm(vector<matrix<double> > args)CSKIP({
    int nargs = args.size();
    matrix<double> ans;
    if      (nargs==1) ans=expm(nestedTriangle<0>(args)).bottomLeftCorner();
    else if (nargs==2) ans=expm(nestedTriangle<1>(args)).bottomLeftCorner();
    else if (nargs==3) ans=expm(nestedTriangle<2>(args)).bottomLeftCorner();
    else if (nargs==4) ans=expm(nestedTriangle<3>(args)).bottomLeftCorner();
    else Rf_error("expm: order not implemented.");
    return ans;
  })

  /* Helper to convert list of matrices to CppAD::vector 
     of the format: 'c(length(args), unlist(args))'
  */
  template<class Type>
  CppAD::vector<Type> args2vector(vector<matrix<Type> > args, int skip=-1){
    int nargs = args.size();
    int nkeep = nargs - (skip >= 0);
    int blocksize = args[0].size();
    int n = 1 + nkeep * blocksize;
    CppAD::vector<Type> ans(n);
    ans[0] = nkeep;
    int k=1;
    for(int i=0; i<nargs; i++){
      if(i != skip){
	for(int j=0; j < blocksize; j++){
	  ans[k] = args[i](j);
	  k++;
	}
      }
    }
    return ans;
  }


  /** \internal \brief Atomic version of generalized matrix exponential (closed under reverse mode differentiation).
  \verbatim
     Generalized matrix exponential:
     
     C(x1,x2,...,xn)
     
     Example: n=3
     
     [ C(x1)       ]        [ x1          ]   [ I ]
     [ C(x1,x2)    ] = expm [ x2 x1       ] * [ 0 ]
     [ C(x1,x3)    ]        [ x3    x1    ]   [ 0 ]
     [ C(x1,x2,x3) ]        [    x3 x2 x1 ]   [ 0 ]

     d/dx1 ( sum( C(x1,x2,x3) * x4 ) ) = C(x1^T, x2^T, x3^T, x4)
     d/dx2 ( sum( C(x1,x2,x3) * x4 ) ) = C(x1^T,       x3^T, x4)
     d/dx3 ( sum( C(x1,x2,x3) * x4 ) ) = C(x1^T, x2^T,       x4)
  \endverbatim
  */
  TMB_ATOMIC_VECTOR_FUNCTION(
			     // ATOMIC_NAME
			     expm
			     ,
			     // OUTPUT_DIM
			     (tx.size()-1)/CppAD::Integer(tx[0])
			     ,
			     // ATOMIC_DOUBLE
			     int nargs=CppAD::Integer(tx[0]);
			     int n=sqrt((double)(tx.size()-1)/nargs);
			     vector<matrix<double> > args(nargs);
			     for(int i=0;i<nargs;i++){
			       args[i] = vec2mat(tx, n, n, 1 + i*n*n);
			     }
			     matrix<double> res = expm(args);
			     for(int i=0;i<n*n;i++)ty[i] = res(i);
			     ,
			     // ATOMIC_REVERSE
			     int nargs=CppAD::Integer(tx[0]);
			     int n=sqrt((double)ty.size());
			     vector<matrix<Type> > args(nargs+1);
			     for(int i=0;i<nargs;i++){
			       args[i] = vec2mat(tx, n, n, 1 + i*n*n).transpose();
			     }
			     args[nargs] = vec2mat(py,n,n);
			     vector<CppAD::vector<Type> > res(nargs);
			     res[0] = expm(args2vector(args));
			     for(int i=1;i<nargs;i++){
			       res[i] = expm(args2vector(args, i));
			     }
			     px[0] = Type(0);
			     for(int j=0;j<res.size();j++){
			       for(int i=0;i<n*n;i++){
				 px[1 + i + j*n*n] = res[j][i];
			       }
			     }
			     )

  /** \brief Matrix exponential

      Calculate the matrix exponential of a dense matrix.
      \ingroup matrix_functions
  */
  template<class Type>
  matrix<Type> expm(matrix<Type> x){
    vector<matrix<Type> > args(1);
    args[0]=x;
    int n=x.rows();
    return vec2mat(expm(args2vector(args)),n,n);
  }

  template<class matrix_pade>
  matrix_pade sqrtm(matrix_pade A){
    return A.sqrtm();
  }

  matrix<double> sqrtm(vector<matrix<double> > args)CSKIP({
    int nargs = args.size();
    matrix<double> ans;
    if      (nargs==1) ans=sqrtm(nestedTriangle<0>(args)).bottomLeftCorner();
    else if (nargs==2) ans=sqrtm(nestedTriangle<1>(args)).bottomLeftCorner();
    else if (nargs==3) ans=sqrtm(nestedTriangle<2>(args)).bottomLeftCorner();
    else if (nargs==4) ans=sqrtm(nestedTriangle<3>(args)).bottomLeftCorner();
    else Rf_error("sqrtm: order not implemented.");
    return ans;
  })

  TMB_ATOMIC_VECTOR_FUNCTION(
			     // ATOMIC_NAME
			     sqrtm
			     ,
			     // OUTPUT_DIM
			     (tx.size()-1)/CppAD::Integer(tx[0])
			     ,
			     // ATOMIC_DOUBLE
			     int nargs=CppAD::Integer(tx[0]);
			     int n=sqrt((double)(tx.size()-1)/nargs);
			     vector<matrix<double> > args(nargs);
			     for(int i=0;i<nargs;i++){
			       args[i] = vec2mat(tx, n, n, 1 + i*n*n);
			     }
			     matrix<double> res = sqrtm(args);
			     for(int i=0;i<n*n;i++)ty[i] = res(i);
			     ,
			     // ATOMIC_REVERSE
			     int nargs=CppAD::Integer(tx[0]);
			     int n=sqrt((double)ty.size());
			     vector<matrix<Type> > args(nargs+1);
			     for(int i=0;i<nargs;i++){
			       args[i] = vec2mat(tx, n, n, 1 + i*n*n).transpose();
			     }
			     args[nargs] = vec2mat(py,n,n);
			     vector<CppAD::vector<Type> > res(nargs);
			     res[0] = sqrtm(args2vector(args));
			     for(int i=1;i<nargs;i++){
			       res[i] = sqrtm(args2vector(args, i));
			     }
			     px[0] = Type(0);
			     for(int j=0;j<res.size();j++){
			       for(int i=0;i<n*n;i++){
				 px[1 + i + j*n*n] = res[j][i];
			       }
			     }
			     )

  /** \brief Matrix square root

      Calculate the matrix square root of a dense **symmetric positive definite** matrix (represented by its lower triangle).
      \ingroup matrix_functions
  */
  template<class Type>
  matrix<Type> sqrtm(matrix<Type> x) {
    int n=x.rows();
    // Copy lower triangle to upper
    for (int i=0; i<n; i++)
      for (int j=0; j<i; j++)
        x(j, i) = x(i, j);
    vector<matrix<Type> > args(1);
    args[0]=x;
    return vec2mat(sqrtm(args2vector(args)),n,n);
  }

  template<class matrix_pade>
  matrix_pade absm(matrix_pade A){
    return A.absm();
  }

  matrix<double> absm(vector<matrix<double> > args)CSKIP({
    int nargs = args.size();
    matrix<double> ans;
    if      (nargs==1) ans=absm(nestedTriangle<0>(args)).bottomLeftCorner();
    else if (nargs==2) ans=absm(nestedTriangle<1>(args)).bottomLeftCorner();
    else if (nargs==3) ans=absm(nestedTriangle<2>(args)).bottomLeftCorner();
    else if (nargs==4) ans=absm(nestedTriangle<3>(args)).bottomLeftCorner();
    else Rf_error("absm: order not implemented.");
    return ans;
  })

  TMB_ATOMIC_VECTOR_FUNCTION(
			     // ATOMIC_NAME
			     absm
			     ,
			     // OUTPUT_DIM
			     (tx.size()-1)/CppAD::Integer(tx[0])
			     ,
			     // ATOMIC_DOUBLE
			     int nargs=CppAD::Integer(tx[0]);
			     int n=sqrt((double)(tx.size()-1)/nargs);
			     vector<matrix<double> > args(nargs);
			     for(int i=0;i<nargs;i++){
			       args[i] = vec2mat(tx, n, n, 1 + i*n*n);
			     }
			     matrix<double> res = absm(args);
			     for(int i=0;i<n*n;i++)ty[i] = res(i);
			     ,
			     // ATOMIC_REVERSE
			     int nargs=CppAD::Integer(tx[0]);
			     int n=sqrt((double)ty.size());
			     vector<matrix<Type> > args(nargs+1);
			     for(int i=0;i<nargs;i++){
			       args[i] = vec2mat(tx, n, n, 1 + i*n*n).transpose();
			     }
			     args[nargs] = vec2mat(py,n,n);
			     vector<CppAD::vector<Type> > res(nargs);
			     res[0] = absm(args2vector(args));
			     for(int i=1;i<nargs;i++){
			       res[i] = absm(args2vector(args, i));
			     }
			     px[0] = Type(0);
			     for(int j=0;j<res.size();j++){
			       for(int i=0;i<n*n;i++){
				 px[1 + i + j*n*n] = res[j][i];
			       }
			     }
			     )

  /** \brief Matrix absolute value

      Calculate the absolute value matrix of a dense **symmetric** matrix (represented by its lower triangle).
      The result will be **symmetric positive semi-definite**.
      The matrix abs is defined as `absm(x)=sqrtm(matmul(x,x))`, but it is more numerically robust especially for singular `x`.
      \ingroup matrix_functions
  */
  template<class Type>
  matrix<Type> absm(matrix<Type> x) {
    int n=x.rows();
    // Copy lower triangle to upper
    for (int i=0; i<n; i++)
      for (int j=0; j<i; j++)
        x(j, i) = x(i, j);
    vector<matrix<Type> > args(1);
    args[0]=x;
    return vec2mat(absm(args2vector(args)),n,n);
  }

} // end namespace atomic

using atomic::expm;
