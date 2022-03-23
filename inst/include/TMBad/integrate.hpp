#ifndef HAVE_INTEGRATE_HPP
#define HAVE_INTEGRATE_HPP
// Autogenerated - do not edit by hand !
#define GLOBAL_HASH_TYPE unsigned int
#define GLOBAL_COMPRESS_TOL 16
#define GLOBAL_UNION_OR_STRUCT union
#define stringify(s) #s
#define xstringify(s) stringify(s)
#define THREAD_NUM 0
#define GLOBAL_INDEX_VECTOR std::vector<GLOBAL_INDEX_TYPE>
#define GLOBAL_INDEX_TYPE unsigned int
#define CONSTEXPR constexpr
#define ASSERT2(x, msg)                          \
  if (!(x)) {                                    \
    Rcerr << "ASSERTION FAILED: " << #x << "\n"; \
    Rcerr << "POSSIBLE REASON: " << msg << "\n"; \
    abort();                                     \
  }
#define GLOBAL_MAX_NUM_THREADS 48
#define INDEX_OVERFLOW(x) \
  ((size_t)(x) >= (size_t)std::numeric_limits<GLOBAL_INDEX_TYPE>::max())
#define ASSERT(x)                                \
  if (!(x)) {                                    \
    Rcerr << "ASSERTION FAILED: " << #x << "\n"; \
    abort();                                     \
  }
#define GLOBAL_REPLAY_TYPE ad_aug
#define GLOBAL_MIN_PERIOD_REP 10
#define INHERIT_CTOR(A, B)                                       \
  A() {}                                                         \
  template <class T1>                                            \
  A(const T1 &x1) : B(x1) {}                                     \
  template <class T1, class T2>                                  \
  A(const T1 &x1, const T2 &x2) : B(x1, x2) {}                   \
  template <class T1, class T2, class T3>                        \
  A(const T1 &x1, const T2 &x2, const T3 &x3) : B(x1, x2, x3) {} \
  template <class T1, class T2, class T3, class T4>              \
  A(const T1 &x1, const T2 &x2, const T3 &x3, const T4 &x4)      \
      : B(x1, x2, x3, x4) {}
#define GLOBAL_SCALAR_TYPE double
#include <float.h>  // INFINITY etc
#include "global.hpp"

namespace TMBad {

/**
   \brief Namespace with utility functions for adaptive numerical integration

   Interfaces to R's integrator that can be used with forward mode AD.
*/
template <class T>
double value(T x) {
  return TMBad::Value(x);
}
double value(double x);
template <class S, class T>
int imin2(S x, T y) {
  return (x < y) ? x : y;
}
template <class S, class T>
double fmin2(S x, T y) {
  return (value(x) < value(y)) ? value(x) : value(y);
}
template <class S, class T>
double fmax2(S x, T y) {
  return (value(x) < value(y)) ? value(y) : value(x);
}
template <class Float, class integr_fn>
static void rdqagie(integr_fn f, void *ex, Float *, int *, Float *, Float *,
                    int *, Float *, Float *, int *, int *, Float *, Float *,
                    Float *, Float *, int *, int *);

template <class Float, class integr_fn>
static void rdqk15i(integr_fn f, void *ex, Float *, int *, Float *, Float *,
                    Float *, Float *, Float *, Float *);

template <class Float, class integr_fn>
static void rdqagse(integr_fn f, void *ex, Float *, Float *, Float *, Float *,
                    int *, Float *, Float *, int *, int *, Float *, Float *,
                    Float *, Float *, int *, int *);

template <class Float, class integr_fn>
static void rdqk21(integr_fn f, void *ex, Float *, Float *, Float *, Float *,
                   Float *, Float *);

template <class Float>
static void rdqpsrt(int *, int *, int *, Float *, Float *, int *, int *);

template <class Float>
static void rdqelg(int *, Float *, Float *, Float *, Float *, int *);

template <class Float, class integr_fn>
void Rdqagi(integr_fn f, void *ex, Float *bound, int *inf, Float *epsabs,
            Float *epsrel, Float *result, Float *abserr, int *neval, int *ier,
            int *limit, int *lenw, int *last, int *iwork, Float *work) {
  int l1, l2, l3;
  *ier = 6;
  *neval = 0;
  *last = 0;
  *result = 0.;
  *abserr = 0.;
  if (*limit < 1 || *lenw < *limit << 2) return;

  l1 = *limit;
  l2 = *limit + l1;
  l3 = *limit + l2;

  rdqagie(f, ex, bound, inf, epsabs, epsrel, limit, result, abserr, neval, ier,
          work, &work[l1], &work[l2], &work[l3], iwork, last);

  return;
}

template <class Float, class integr_fn>
static void rdqagie(integr_fn f, void *ex, Float *bound, int *inf,
                    Float *epsabs, Float *epsrel, int *limit, Float *result,
                    Float *abserr, int *neval, int *ier, Float *alist,
                    Float *blist, Float *rlist, Float *elist, int *iord,
                    int *last) {
  Float area, dres;
  int ksgn;
  Float boun;
  int nres;
  Float area1, area2, area12;
  int k;
  Float small = 0.0, erro12;
  int ierro;
  Float a1, a2, b1, b2, defab1, defab2, oflow;
  int ktmin, nrmax;
  Float uflow;
  bool noext;
  int iroff1, iroff2, iroff3;
  Float res3la[3], error1, error2;
  int id;
  Float rlist2[52];
  int numrl2;
  Float defabs, epmach, erlarg = 0.0, abseps, correc = 0.0, errbnd, resabs;
  int jupbnd;
  Float erlast, errmax;
  int maxerr;
  Float reseps;
  bool extrap;
  Float ertest = 0.0, errsum;
  --iord;
  --elist;
  --rlist;
  --blist;
  --alist;

  epmach = DBL_EPSILON;

  *ier = 0;
  *neval = 0;
  *last = 0;
  *result = 0.;
  *abserr = 0.;
  alist[1] = 0.;
  blist[1] = 1.;
  rlist[1] = 0.;
  elist[1] = 0.;
  iord[1] = 0;
  if (*epsabs <= 0. && (*epsrel < fmax2(epmach * 50., 5e-29))) *ier = 6;
  if (*ier == 6) return;
  boun = *bound;
  if (*inf == 2) {
    boun = 0.;
  }

  static Float c_b6 = 0.;
  static Float c_b7 = 1.;

  rdqk15i(f, ex, &boun, inf, &c_b6, &c_b7, result, abserr, &defabs, &resabs);

  *last = 1;
  rlist[1] = *result;
  elist[1] = *abserr;
  iord[1] = 1;
  dres = fabs(*result);
  errbnd = fmax2(*epsabs, *epsrel * dres);
  if (*abserr <= epmach * 100. * defabs && *abserr > errbnd) *ier = 2;
  if (*limit == 1) *ier = 1;
  if (*ier != 0 || (*abserr <= errbnd && *abserr != resabs) || *abserr == 0.)
    goto L130;

  uflow = DBL_MIN;
  oflow = DBL_MAX;
  rlist2[0] = *result;
  errmax = *abserr;
  maxerr = 1;
  area = *result;
  errsum = *abserr;
  *abserr = oflow;
  nrmax = 1;
  nres = 0;
  ktmin = 0;
  numrl2 = 2;
  extrap = false;
  noext = false;
  ierro = 0;
  iroff1 = 0;
  iroff2 = 0;
  iroff3 = 0;
  ksgn = -1;
  if (dres >= (1. - epmach * 50.) * defabs) {
    ksgn = 1;
  }

  for (*last = 2; *last <= *limit; ++(*last)) {
    a1 = alist[maxerr];
    b1 = (alist[maxerr] + blist[maxerr]) * .5;
    a2 = b1;
    b2 = blist[maxerr];
    erlast = errmax;
    rdqk15i(f, ex, &boun, inf, &a1, &b1, &area1, &error1, &resabs, &defab1);
    rdqk15i(f, ex, &boun, inf, &a2, &b2, &area2, &error2, &resabs, &defab2);

    area12 = area1 + area2;
    erro12 = error1 + error2;
    errsum = errsum + erro12 - errmax;
    area = area + area12 - rlist[maxerr];
    if (!(defab1 == error1 || defab2 == error2)) {
      if (fabs(rlist[maxerr] - area12) <= fabs(area12) * 1e-5 &&
          erro12 >= errmax * .99) {
        if (extrap)
          ++iroff2;
        else
          ++iroff1;
      }
      if (*last > 10 && erro12 > errmax) ++iroff3;
    }

    rlist[maxerr] = area1;
    rlist[*last] = area2;
    errbnd = fmax2(*epsabs, *epsrel * fabs(area));

    if (iroff1 + iroff2 >= 10 || iroff3 >= 20) *ier = 2;
    if (iroff2 >= 5) ierro = 3;

    if (*last == *limit) *ier = 1;

    if (fmax2(fabs(a1), fabs(b2)) <=
        (epmach * 100. + 1.) * (fabs(a2) + uflow * 1e3)) {
      *ier = 4;
    }

    if (error2 <= error1) {
      alist[*last] = a2;
      blist[maxerr] = b1;
      blist[*last] = b2;
      elist[maxerr] = error1;
      elist[*last] = error2;
    } else {
      alist[maxerr] = a2;
      alist[*last] = a1;
      blist[*last] = b1;
      rlist[maxerr] = area2;
      rlist[*last] = area1;
      elist[maxerr] = error2;
      elist[*last] = error1;
    }

    rdqpsrt(limit, last, &maxerr, &errmax, &elist[1], &iord[1], &nrmax);
    if (errsum <= errbnd) {
      goto L115;
    }
    if (*ier != 0) break;
    if (*last == 2) {
      small = .375;
      erlarg = errsum;
      ertest = errbnd;
      rlist2[1] = area;
      continue;
    }
    if (noext) continue;

    erlarg -= erlast;
    if (fabs(b1 - a1) > small) {
      erlarg += erro12;
    }
    if (!extrap) {
      if (fabs(blist[maxerr] - alist[maxerr]) > small) {
        continue;
      }
      extrap = true;
      nrmax = 2;
    }

    if (ierro != 3 && erlarg > ertest) {
      id = nrmax;
      jupbnd = *last;
      if (*last > *limit / 2 + 2) {
        jupbnd = *limit + 3 - *last;
      }
      for (k = id; k <= jupbnd; ++k) {
        maxerr = iord[nrmax];
        errmax = elist[maxerr];
        if (fabs(blist[maxerr] - alist[maxerr]) > small) {
          goto L90;
        }
        ++nrmax;
      }
    }

    ++numrl2;
    rlist2[numrl2 - 1] = area;
    rdqelg(&numrl2, rlist2, &reseps, &abseps, res3la, &nres);
    ++ktmin;
    if (ktmin > 5 && *abserr < errsum * .001) {
      *ier = 5;
    }
    if (abseps >= *abserr) {
      goto L70;
    }
    ktmin = 0;
    *abserr = abseps;
    *result = reseps;
    correc = erlarg;
    ertest = fmax2(*epsabs, *epsrel * fabs(reseps));
    if (*abserr <= ertest) {
      break;
    }

  L70:
    if (numrl2 == 1) {
      noext = true;
    }
    if (*ier == 5) {
      break;
    }
    maxerr = iord[1];
    errmax = elist[maxerr];
    nrmax = 1;
    extrap = false;
    small *= .5;
    erlarg = errsum;
  L90:;
  }

  if (*abserr == oflow) {
    goto L115;
  }
  if (*ier + ierro == 0) {
    goto L110;
  }
  if (ierro == 3) {
    *abserr += correc;
  }
  if (*ier == 0) {
    *ier = 3;
  }
  if (*result == 0. || area == 0.) {
    if (*abserr > errsum) goto L115;

    if (area == 0.) goto L130;
  } else {
    if (*abserr / fabs(*result) > errsum / fabs(area)) {
      goto L115;
    }
  }

L110:
  if (ksgn == -1 && fmax2(fabs(*result), fabs(area)) <= defabs * .01) {
    goto L130;
  }
  if (.01 > *result / area || *result / area > 100. || errsum > fabs(area)) {
    *ier = 6;
  }
  goto L130;

L115:
  *result = 0.;
  for (k = 1; k <= *last; ++k) *result += rlist[k];

  *abserr = errsum;
L130:
  *neval = *last * 30 - 15;
  if (*inf == 2) {
    *neval <<= 1;
  }
  if (*ier > 2) {
    --(*ier);
  }
  return;
}

template <class Float, class integr_fn>
void Rdqags(integr_fn f, void *ex, Float *a, Float *b, Float *epsabs,
            Float *epsrel, Float *result, Float *abserr, int *neval, int *ier,
            int *limit, int *lenw, int *last, int *iwork, Float *work) {
  int l1, l2, l3;
  *ier = 6;
  *neval = 0;
  *last = 0;
  *result = 0.;
  *abserr = 0.;
  if (*limit < 1 || *lenw < *limit * 4) return;

  l1 = *limit;
  l2 = *limit + l1;
  l3 = *limit + l2;

  rdqagse(f, ex, a, b, epsabs, epsrel, limit, result, abserr, neval, ier, work,
          &work[l1], &work[l2], &work[l3], iwork, last);

  return;
}

template <class Float, class integr_fn>
static void rdqagse(integr_fn f, void *ex, Float *a, Float *b, Float *epsabs,
                    Float *epsrel, int *limit, Float *result, Float *abserr,
                    int *neval, int *ier, Float *alist, Float *blist,
                    Float *rlist, Float *elist, int *iord, int *last) {
  bool noext, extrap;
  int k, ksgn, nres;
  int ierro;
  int ktmin, nrmax;
  int iroff1, iroff2, iroff3;
  int id;
  int numrl2;
  int jupbnd;
  int maxerr;
  Float res3la[3];
  Float rlist2[52];
  Float abseps, area, area1, area2, area12, dres, epmach;
  Float a1, a2, b1, b2, defabs, defab1, defab2, oflow, uflow, resabs, reseps;
  Float error1, error2, erro12, errbnd, erlast, errmax, errsum;

  Float correc = 0.0, erlarg = 0.0, ertest = 0.0, small = 0.0;
  --iord;
  --elist;
  --rlist;
  --blist;
  --alist;

  epmach = DBL_EPSILON;

  *ier = 0;
  *neval = 0;
  *last = 0;
  *result = 0.;
  *abserr = 0.;
  alist[1] = *a;
  blist[1] = *b;
  rlist[1] = 0.;
  elist[1] = 0.;
  if (*epsabs <= 0. && *epsrel < fmax2(epmach * 50., 5e-29)) {
    *ier = 6;
    return;
  }

  uflow = DBL_MIN;
  oflow = DBL_MAX;
  ierro = 0;
  rdqk21(f, ex, a, b, result, abserr, &defabs, &resabs);

  dres = fabs(*result);
  errbnd = fmax2(*epsabs, *epsrel * dres);
  *last = 1;
  rlist[1] = *result;
  elist[1] = *abserr;
  iord[1] = 1;
  if (*abserr <= epmach * 100. * defabs && *abserr > errbnd) *ier = 2;
  if (*limit == 1) *ier = 1;
  if (*ier != 0 || (*abserr <= errbnd && *abserr != resabs) || *abserr == 0.)
    goto L140;

  rlist2[0] = *result;
  errmax = *abserr;
  maxerr = 1;
  area = *result;
  errsum = *abserr;
  *abserr = oflow;
  nrmax = 1;
  nres = 0;
  numrl2 = 2;
  ktmin = 0;
  extrap = false;
  noext = false;
  iroff1 = 0;
  iroff2 = 0;
  iroff3 = 0;
  ksgn = -1;
  if (dres >= (1. - epmach * 50.) * defabs) {
    ksgn = 1;
  }

  for (*last = 2; *last <= *limit; ++(*last)) {
    a1 = alist[maxerr];
    b1 = (alist[maxerr] + blist[maxerr]) * .5;
    a2 = b1;
    b2 = blist[maxerr];
    erlast = errmax;
    rdqk21(f, ex, &a1, &b1, &area1, &error1, &resabs, &defab1);
    rdqk21(f, ex, &a2, &b2, &area2, &error2, &resabs, &defab2);

    area12 = area1 + area2;
    erro12 = error1 + error2;
    errsum = errsum + erro12 - errmax;
    area = area + area12 - rlist[maxerr];
    if (!(defab1 == error1 || defab2 == error2)) {
      if (fabs(rlist[maxerr] - area12) <= fabs(area12) * 1e-5 &&
          erro12 >= errmax * .99) {
        if (extrap)
          ++iroff2;
        else
          ++iroff1;
      }
      if (*last > 10 && erro12 > errmax) ++iroff3;
    }
    rlist[maxerr] = area1;
    rlist[*last] = area2;
    errbnd = fmax2(*epsabs, *epsrel * fabs(area));

    if (iroff1 + iroff2 >= 10 || iroff3 >= 20) *ier = 2;
    if (iroff2 >= 5) ierro = 3;

    if (*last == *limit) *ier = 1;

    if (fmax2(fabs(a1), fabs(b2)) <=
        (epmach * 100. + 1.) * (fabs(a2) + uflow * 1e3)) {
      *ier = 4;
    }

    if (error2 > error1) {
      alist[maxerr] = a2;
      alist[*last] = a1;
      blist[*last] = b1;
      rlist[maxerr] = area2;
      rlist[*last] = area1;
      elist[maxerr] = error2;
      elist[*last] = error1;
    } else {
      alist[*last] = a2;
      blist[maxerr] = b1;
      blist[*last] = b2;
      elist[maxerr] = error1;
      elist[*last] = error2;
    }

    rdqpsrt(limit, last, &maxerr, &errmax, &elist[1], &iord[1], &nrmax);

    if (errsum <= errbnd) goto L115;
    if (*ier != 0) break;
    if (*last == 2) {
      small = fabs(*b - *a) * .375;
      erlarg = errsum;
      ertest = errbnd;
      rlist2[1] = area;
      continue;
    }
    if (noext) continue;

    erlarg -= erlast;
    if (fabs(b1 - a1) > small) {
      erlarg += erro12;
    }
    if (!extrap) {
      if (fabs(blist[maxerr] - alist[maxerr]) > small) {
        continue;
      }
      extrap = true;
      nrmax = 2;
    }

    if (ierro != 3 && erlarg > ertest) {
      id = nrmax;
      jupbnd = *last;
      if (*last > *limit / 2 + 2) {
        jupbnd = *limit + 3 - *last;
      }
      for (k = id; k <= jupbnd; ++k) {
        maxerr = iord[nrmax];
        errmax = elist[maxerr];
        if (fabs(blist[maxerr] - alist[maxerr]) > small) {
          goto L90;
        }
        ++nrmax;
      }
    }

    ++numrl2;
    rlist2[numrl2 - 1] = area;
    rdqelg(&numrl2, rlist2, &reseps, &abseps, res3la, &nres);
    ++ktmin;
    if (ktmin > 5 && *abserr < errsum * .001) {
      *ier = 5;
    }
    if (abseps < *abserr) {
      ktmin = 0;
      *abserr = abseps;
      *result = reseps;
      correc = erlarg;
      ertest = fmax2(*epsabs, *epsrel * fabs(reseps));
      if (*abserr <= ertest) {
        break;
      }
    }

    if (numrl2 == 1) {
      noext = true;
    }
    if (*ier == 5) {
      break;
    }
    maxerr = iord[1];
    errmax = elist[maxerr];
    nrmax = 1;
    extrap = false;
    small *= .5;
    erlarg = errsum;
  L90:;
  }

  if (*abserr == oflow) goto L115;
  if (*ier + ierro == 0) goto L110;
  if (ierro == 3) *abserr += correc;
  if (*ier == 0) *ier = 3;
  if (*result == 0. || area == 0.) {
    if (*abserr > errsum) goto L115;
    if (area == 0.) goto L130;
  } else {
    if (*abserr / fabs(*result) > errsum / fabs(area)) goto L115;
  }

L110:
  if (ksgn == -1 && fmax2(fabs(*result), fabs(area)) <= defabs * .01) {
    goto L130;
  }
  if (.01 > *result / area || *result / area > 100. || errsum > fabs(area)) {
    *ier = 5;
  }
  goto L130;

L115:
  *result = 0.;
  for (k = 1; k <= *last; ++k) *result += rlist[k];
  *abserr = errsum;
L130:
  if (*ier > 2)
  L140:
    *neval = *last * 42 - 21;
  return;
}

template <class Float, class integr_fn>
static void rdqk15i(integr_fn f, void *ex, Float *boun, int *inf, Float *a,
                    Float *b, Float *result, Float *abserr, Float *resabs,
                    Float *resasc) {
  static double wg[8] = {0., .129484966168869693270611432679082,
                         0., .27970539148927666790146777142378,
                         0., .381830050505118944950369775488975,
                         0., .417959183673469387755102040816327};
  static double xgk[8] = {
      .991455371120812639206854697526329, .949107912342758524526189684047851,
      .864864423359769072789712788640926, .741531185599394439863864773280788,
      .58608723546769113029414483825873,  .405845151377397166906606412076961,
      .207784955007898467600689403773245, 0.};
  static double wgk[8] = {
      .02293532201052922496373200805897,  .063092092629978553290700663189204,
      .104790010322250183839876322541518, .140653259715525918745189590510238,
      .16900472663926790282658342659855,  .190350578064785409913256402421014,
      .204432940075298892414161999234649, .209482141084727828012999174891714};

  Float absc, dinf, resg, resk, fsum, absc1, absc2, fval1, fval2;
  int j;
  Float hlgth, centr, reskh, uflow;
  Float tabsc1, tabsc2, fc, epmach;
  Float fv1[7], fv2[7], vec[15], vec2[15];
  epmach = DBL_EPSILON;
  uflow = DBL_MIN;
  dinf = (double)imin2(1, *inf);

  centr = (*a + *b) * .5;
  hlgth = (*b - *a) * .5;
  tabsc1 = *boun + dinf * (1. - centr) / centr;
  vec[0] = tabsc1;
  if (*inf == 2) {
    vec2[0] = -tabsc1;
  }
  for (j = 1; j <= 7; ++j) {
    absc = hlgth * xgk[j - 1];
    absc1 = centr - absc;
    absc2 = centr + absc;
    tabsc1 = *boun + dinf * (1. - absc1) / absc1;
    tabsc2 = *boun + dinf * (1. - absc2) / absc2;
    vec[(j << 1) - 1] = tabsc1;
    vec[j * 2] = tabsc2;
    if (*inf == 2) {
      vec2[(j << 1) - 1] = -tabsc1;
      vec2[j * 2] = -tabsc2;
    }
  }
  f(vec, 15, ex);
  if (*inf == 2) f(vec2, 15, ex);
  fval1 = vec[0];
  if (*inf == 2) fval1 += vec2[0];
  fc = fval1 / centr / centr;

  resg = wg[7] * fc;
  resk = wgk[7] * fc;
  *resabs = fabs(resk);
  for (j = 1; j <= 7; ++j) {
    absc = hlgth * xgk[j - 1];
    absc1 = centr - absc;
    absc2 = centr + absc;
    tabsc1 = *boun + dinf * (1. - absc1) / absc1;
    tabsc2 = *boun + dinf * (1. - absc2) / absc2;
    fval1 = vec[(j << 1) - 1];
    fval2 = vec[j * 2];
    if (*inf == 2) {
      fval1 += vec2[(j << 1) - 1];
    }
    if (*inf == 2) {
      fval2 += vec2[j * 2];
    }
    fval1 = fval1 / absc1 / absc1;
    fval2 = fval2 / absc2 / absc2;
    fv1[j - 1] = fval1;
    fv2[j - 1] = fval2;
    fsum = fval1 + fval2;
    resg += wg[j - 1] * fsum;
    resk += wgk[j - 1] * fsum;
    *resabs += wgk[j - 1] * (fabs(fval1) + fabs(fval2));
  }
  reskh = resk * .5;
  *resasc = wgk[7] * fabs(fc - reskh);
  for (j = 1; j <= 7; ++j) {
    *resasc +=
        wgk[j - 1] * (fabs(fv1[j - 1] - reskh) + fabs(fv2[j - 1] - reskh));
  }
  *result = resk * hlgth;
  *resasc *= hlgth;
  *resabs *= hlgth;
  *abserr = fabs((resk - resg) * hlgth);
  if (*resasc != 0. && *abserr != 0.) {
    *abserr = *resasc * fmin2(1., pow(*abserr * 200. / *resasc, 1.5));
  }
  if (*resabs > uflow / (epmach * 50.)) {
    *abserr = fmax2(epmach * 50. * *resabs, *abserr);
  }
  return;
}

template <class Float>
static void rdqelg(int *n, Float *epstab, Float *result, Float *abserr,
                   Float *res3la, int *nres) {
  int i__, indx, ib, ib2, ie, k1, k2, k3, num, newelm, limexp;
  Float delta1, delta2, delta3, e0, e1, e1abs, e2, e3, epmach, epsinf;
  Float oflow, ss, res;
  Float errA, err1, err2, err3, tol1, tol2, tol3;
  --res3la;
  --epstab;

  epmach = DBL_EPSILON;
  oflow = DBL_MAX;
  ++(*nres);
  *abserr = oflow;
  *result = epstab[*n];
  if (*n < 3) {
    goto L100;
  }
  limexp = 50;
  epstab[*n + 2] = epstab[*n];
  newelm = (*n - 1) / 2;
  epstab[*n] = oflow;
  num = *n;
  k1 = *n;
  for (i__ = 1; i__ <= newelm; ++i__) {
    k2 = k1 - 1;
    k3 = k1 - 2;
    res = epstab[k1 + 2];
    e0 = epstab[k3];
    e1 = epstab[k2];
    e2 = res;
    e1abs = fabs(e1);
    delta2 = e2 - e1;
    err2 = fabs(delta2);
    tol2 = fmax2(fabs(e2), e1abs) * epmach;
    delta3 = e1 - e0;
    err3 = fabs(delta3);
    tol3 = fmax2(e1abs, fabs(e0)) * epmach;
    if (err2 <= tol2 && err3 <= tol3) {
      *result = res;
      *abserr = err2 + err3;

      goto L100;
    }

    e3 = epstab[k1];
    epstab[k1] = e1;
    delta1 = e1 - e3;
    err1 = fabs(delta1);
    tol1 = fmax2(e1abs, fabs(e3)) * epmach;

    if (err1 > tol1 && err2 > tol2 && err3 > tol3) {
      ss = 1. / delta1 + 1. / delta2 - 1. / delta3;
      epsinf = fabs(ss * e1);

      if (epsinf > 1e-4) {
        goto L30;
      }
    }

    *n = i__ + i__ - 1;
    goto L50;

  L30:

    res = e1 + 1. / ss;
    epstab[k1] = res;
    k1 += -2;
    errA = err2 + fabs(res - e2) + err3;
    if (errA <= *abserr) {
      *abserr = errA;
      *result = res;
    }
  }

L50:
  if (*n == limexp) {
    *n = (limexp / 2 << 1) - 1;
  }

  if (num / 2 << 1 == num)
    ib = 2;
  else
    ib = 1;
  ie = newelm + 1;
  for (i__ = 1; i__ <= ie; ++i__) {
    ib2 = ib + 2;
    epstab[ib] = epstab[ib2];
    ib = ib2;
  }
  if (num != *n) {
    indx = num - *n + 1;
    for (i__ = 1; i__ <= *n; ++i__) {
      epstab[i__] = epstab[indx];
      ++indx;
    }
  }

  if (*nres >= 4) {
    *abserr = fabs(*result - res3la[3]) + fabs(*result - res3la[2]) +
              fabs(*result - res3la[1]);
    res3la[1] = res3la[2];
    res3la[2] = res3la[3];
    res3la[3] = *result;
  } else {
    res3la[*nres] = *result;
    *abserr = oflow;
  }

L100:
  *abserr = fmax2(*abserr, epmach * 5. * fabs(*result));
  return;
}

template <class Float, class integr_fn>
static void rdqk21(integr_fn f, void *ex, Float *a, Float *b, Float *result,
                   Float *abserr, Float *resabs, Float *resasc) {
  static double wg[5] = {
      .066671344308688137593568809893332, .149451349150580593145776339657697,
      .219086362515982043995534934228163, .269266719309996355091226921569469,
      .295524224714752870173892994651338};
  static double xgk[11] = {.995657163025808080735527280689003,
                           .973906528517171720077964012084452,
                           .930157491355708226001207180059508,
                           .865063366688984510732096688423493,
                           .780817726586416897063717578345042,
                           .679409568299024406234327365114874,
                           .562757134668604683339000099272694,
                           .433395394129247190799265943165784,
                           .294392862701460198131126603103866,
                           .14887433898163121088482600112972,
                           0.};
  static double wgk[11] = {
      .011694638867371874278064396062192, .03255816230796472747881897245939,
      .05475589657435199603138130024458,  .07503967481091995276704314091619,
      .093125454583697605535065465083366, .109387158802297641899210590325805,
      .123491976262065851077958109831074, .134709217311473325928054001771707,
      .142775938577060080797094273138717, .147739104901338491374841515972068,
      .149445554002916905664936468389821};

  Float fv1[10], fv2[10], vec[21];
  Float absc, resg, resk, fsum, fval1, fval2;
  Float hlgth, centr, reskh, uflow;
  Float fc, epmach, dhlgth;
  int j, jtw, jtwm1;
  epmach = DBL_EPSILON;
  uflow = DBL_MIN;

  centr = (*a + *b) * .5;
  hlgth = (*b - *a) * .5;
  dhlgth = fabs(hlgth);

  resg = 0.;
  vec[0] = centr;
  for (j = 1; j <= 5; ++j) {
    jtw = j << 1;
    absc = hlgth * xgk[jtw - 1];
    vec[(j << 1) - 1] = centr - absc;

    vec[j * 2] = centr + absc;
  }
  for (j = 1; j <= 5; ++j) {
    jtwm1 = (j << 1) - 1;
    absc = hlgth * xgk[jtwm1 - 1];
    vec[(j << 1) + 9] = centr - absc;
    vec[(j << 1) + 10] = centr + absc;
  }
  f(vec, 21, ex);
  fc = vec[0];
  resk = wgk[10] * fc;
  *resabs = fabs(resk);
  for (j = 1; j <= 5; ++j) {
    jtw = j << 1;
    absc = hlgth * xgk[jtw - 1];
    fval1 = vec[(j << 1) - 1];
    fval2 = vec[j * 2];
    fv1[jtw - 1] = fval1;
    fv2[jtw - 1] = fval2;
    fsum = fval1 + fval2;
    resg += wg[j - 1] * fsum;
    resk += wgk[jtw - 1] * fsum;
    *resabs += wgk[jtw - 1] * (fabs(fval1) + fabs(fval2));
  }
  for (j = 1; j <= 5; ++j) {
    jtwm1 = (j << 1) - 1;
    absc = hlgth * xgk[jtwm1 - 1];
    fval1 = vec[(j << 1) + 9];
    fval2 = vec[(j << 1) + 10];
    fv1[jtwm1 - 1] = fval1;
    fv2[jtwm1 - 1] = fval2;
    fsum = fval1 + fval2;
    resk += wgk[jtwm1 - 1] * fsum;
    *resabs += wgk[jtwm1 - 1] * (fabs(fval1) + fabs(fval2));
  }
  reskh = resk * .5;
  *resasc = wgk[10] * fabs(fc - reskh);
  for (j = 1; j <= 10; ++j) {
    *resasc +=
        wgk[j - 1] * (fabs(fv1[j - 1] - reskh) + fabs(fv2[j - 1] - reskh));
  }
  *result = resk * hlgth;
  *resabs *= dhlgth;
  *resasc *= dhlgth;
  *abserr = fabs((resk - resg) * hlgth);
  if (*resasc != 0. && *abserr != 0.) {
    *abserr = *resasc * fmin2(1., pow(*abserr * 200. / *resasc, 1.5));
  }
  if (*resabs > uflow / (epmach * 50.)) {
    *abserr = fmax2(epmach * 50. * *resabs, *abserr);
  }
  return;
}

template <class Float>
static void rdqpsrt(int *limit, int *last, int *maxerr, Float *ermax,
                    Float *elist, int *iord, int *nrmax) {
  int i, j, k, ido, jbnd, isucc, jupbn;
  Float errmin, errmax;
  --iord;
  --elist;

  if (*last <= 2) {
    iord[1] = 1;
    iord[2] = 2;
    goto Last;
  }

  errmax = elist[*maxerr];
  if (*nrmax > 1) {
    ido = *nrmax - 1;
    for (i = 1; i <= ido; ++i) {
      isucc = iord[*nrmax - 1];
      if (errmax <= elist[isucc]) break;
      iord[*nrmax] = isucc;
      --(*nrmax);
    }
  }

  if (*last > *limit / 2 + 2)
    jupbn = *limit + 3 - *last;
  else
    jupbn = *last;

  errmin = elist[*last];

  jbnd = jupbn - 1;
  for (i = *nrmax + 1; i <= jbnd; ++i) {
    isucc = iord[i];
    if (errmax >= elist[isucc]) {
      iord[i - 1] = *maxerr;
      for (j = i, k = jbnd; j <= jbnd; j++, k--) {
        isucc = iord[k];
        if (errmin < elist[isucc]) {
          iord[k + 1] = *last;
          goto Last;
        }
        iord[k + 1] = isucc;
      }
      iord[i] = *last;
      goto Last;
    }
    iord[i - 1] = isucc;
  }

  iord[jbnd] = *maxerr;
  iord[jupbn] = *last;

Last:

  *maxerr = iord[*nrmax];
  *ermax = elist[*maxerr];
  return;
}

/** \brief User control parameters for R's integrate

    These control parameters are similar to R's function 'integrate'.
    \param subdivisions The maximum number of subintervals.
    \param reltol Relative accuracy requested.
    \param abstol Absolute accuracy requested.
*/
struct control {
  int subdivisions;
  double reltol;
  double abstol;
  control(int subdivisions_ = 100, double reltol_ = 1e-4,
          double abstol_ = 1e-4);
};

/**
    \brief Interface to R's adaptive integrate routine.

    Takes Integrand (functor) as template parameter. Integrand must be
    a template class where:

    1. Template parameter is the scalar type.
    2. Contains a 'typedef Type Scalar;'
    3. Has an evaluation operator.

    Use this class if multiple integrals must be computed with
    different parameters. Otherwise use the 'integrate' function (see
    below).
*/
template <class Integrand>
struct Integral {
  typedef typename Integrand::Scalar Type;

  struct vectorized_integrand {
    Integrand f;
    vectorized_integrand(Integrand f_) : f(f_) {}
    void operator()(Type *x, int n, void *ex) {
      for (int i = 0; i < n; i++) x[i] = f(x[i]);
    }
  } fn;
  /** \brief Return reference to integrand so the user can change parameters. */
  Integrand &integrand() { return fn.f; }

  Type epsabs, epsrel, result, abserr;
  int neval, ier, limit, lenw, last;
  std::vector<int> iwork;
  std::vector<Type> work;
  void setAccuracy(double epsrel_ = 1e-4, double epsabs_ = 1e-4) {
    epsabs = epsabs_;
    epsrel = epsrel_;
    result = 0;
    abserr = 1e4;
    neval = 0;
    ier = 0;
    last = 0;
  }
  void setWorkspace(int subdivisions = 100) {
    limit = subdivisions;
    lenw = 4 * limit;
    iwork.resize(limit);
    work.resize(lenw);
  }
  Type a, b, bound;
  int inf;
  void setBounds(Type a_, Type b_) {
    int a_finite = (a_ != -INFINITY) && (a_ != INFINITY);
    int b_finite = (b_ != -INFINITY) && (b_ != INFINITY);
    if (a_finite && b_finite) {
      inf = 0;
      a = a_;
      b = b_;
    } else if (a_finite && !b_finite) {
      inf = 1;
      bound = a_;
    } else if (!a_finite && b_finite) {
      inf = -1;
      bound = b_;
    } else {
      inf = 2;
    }
  }
  /** \brief Constructor
      \param f_ Functor integrand
      \param a_ Lower integration limit. Negative infinity allowed.
      \param b_ Upper integration limit. Positive infinity allowed.
      \param c_ Control parameters for accuracy.
  */
  Integral(Integrand f_, Type a_, Type b_, control c = control()) : fn(f_) {
    setAccuracy(c.reltol, c.abstol);
    setWorkspace(c.subdivisions);
    setBounds(a_, b_);
  }
  Type operator()() {
    if (inf)
      Rdqagi(fn, NULL, &bound, &inf, &epsabs, &epsrel, &result, &abserr, &neval,
             &ier, &limit, &lenw, &last, &iwork[0], &work[0]);
    else
      Rdqags(fn, NULL, &a, &b, &epsabs, &epsrel, &result, &abserr, &neval, &ier,
             &limit, &lenw, &last, &iwork[0], &work[0]);
    return result;
  }
};

/** \brief Integrate function over finite or infinite interval
    \param f Univariate integrand (functor)
    \param a Lower integration limit. Default is negative infinity.
    \param a Upper integration limit. Default is positive infinity.
    \param c Optional control parameters.

    Example:
    \code
    template<class Float>
    struct Gauss_t {
      typedef Float Scalar;
      Float a;  // Parameter
      // Evaluate integrand
      Float operator(Float x) () {
        Float ans = exp(- a*x*x);
        return ans;
      }
      // Integrate wrt x
      Float my_integrate() {
        using gauss_kronrod::integrate;
        Float ans = integrate(*this);
        return ans;
      }
    };
    \endcode
*/
template <class Integrand>
typename Integrand::Scalar integrate(Integrand f,
                                     typename Integrand::Scalar a = -INFINITY,
                                     typename Integrand::Scalar b = INFINITY,
                                     control c = control()) {
  Integral<Integrand> I(f, a, b, c);
  return I();
}

/**
   \brief Multivariate integral class.

   Takes Integrand (functor) as template parameter. Integrand must be
   a template class where:

   1. Template parameter is the integrand type.
   2. Contains a 'typedef Type Scalar;'
   3. Has an evaluation operator with void input.

   Use this class if multiple integrals must be computed with
   different parameters. Otherwise use the 'mvIntegrate' function (see
   below).
*/
template <class Integrand>
struct mvIntegral {
  typedef typename Integrand::Scalar Scalar;
  struct evaluator {
    typedef typename Integrand::Scalar Scalar;
    Integrand &f;
    Scalar &x;
    evaluator(Integrand &f_, Scalar &x_) : f(f_), x(x_) {}
    Scalar operator()(const Scalar &x_) {
      x = x_;
      return f();
    }
  } ev;
  control c;
  Integral<evaluator> I;
  mvIntegral(Integrand &f_, Scalar &x_, Scalar a = -INFINITY,
             Scalar b = INFINITY, control c_ = control())
      : ev(f_, x_), c(c_), I(ev, a, b, c_) {}
  Scalar operator()() { return I(); }
  /** \brief With respect to */
  mvIntegral<mvIntegral> wrt(Scalar &x, Scalar a = -INFINITY,
                             Scalar b = INFINITY) {
    return mvIntegral<mvIntegral>(*this, x, a, b, c);
  }
};

template <class Integrand>
struct mvIntegral0 {
  typedef typename Integrand::Scalar Scalar;
  Integrand &f;
  control c;
  mvIntegral0(Integrand &f_, control c_) : f(f_), c(c_) {}
  /** \brief With respect to */
  mvIntegral<Integrand> wrt(Scalar &x, Scalar a = -INFINITY,
                            Scalar b = INFINITY) {
    return mvIntegral<Integrand>(f, x, a, b, c);
  }
};
/** \brief Multivariate integration
    \param f Multivariate integrand (functor)
    \param c Optional control parameters

    Example:
    \code
    template<class Float>
    struct Gauss2D_t {
      typedef Float Scalar;
      Float a, b;  // Parameters
      Float x, y;  // Integration variables
      // Evaluate integrand (u1,u2)
      Float operator() () {
        Float ans = exp(- a*x*x - b*y*y);
        return ans;
      }
      // Integrate wrt (x,y)
      Float my_integrate() {
        using gauss_kronrod::mvIntegrate;
        Float ans = mvIntegrate(*this).
          wrt(x, -INFINITY, INFINITY).
          wrt(y, -INFINITY, INFINITY) ();
        return ans;
      }
    };
    \endcode
*/
template <class Integrand>
mvIntegral0<Integrand> mvIntegrate(Integrand &f, control c = control()) {
  return mvIntegral0<Integrand>(f, c);
}

}  // namespace TMBad
#endif  // HAVE_INTEGRATE_HPP