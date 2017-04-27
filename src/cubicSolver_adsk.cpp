//-
// ==========================================================================
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// From: https://github.com/autodesk-adn/Maya-devkit/blob/master/win/devkit/animEngine/animEngine.c
// ==========================================================================
//+
#include "cubicSolver_adsk.h"

#include <math.h>

EtValue sMachineTolerance;

EtInt
dbl_gt (EtValue *a, EtValue *b)
{
	return (*a > *b ? 1 : 0);
}

EtVoid
dbl_mult (EtValue *a, EtValue *b, EtValue *atimesb)
{
	EtValue product = (*a) * (*b);
	*atimesb = product;
}

EtVoid
dbl_add (EtValue *a, EtValue *b, EtValue *aplusb)
{
	EtValue sum = (*a) + (*b);
	*aplusb = sum;
}

EtVoid
init_tolerance ()
/*
// Description:
//		Init the machine tolerance, sMachineTolerance, is defined to be the
//		double that satisfies:
//		(1)  sMachineTolerance = 2^(k), for some integer k;
//		(2*) (1.0 + sMachineTolerance > 1.0) is TRUE;
//		(3*) (1.0 + sMachineTolerance/2.0 == 1.0) is TRUE.
//		(*)  When NO floating point optimization is used.
//
//		To foil floating point optimizers, sMachineTolerance must be
//		computed using dbl_mult(), dbl_add() and dbl_gt().
*/
{
    EtValue one, half, sum;
    one  = 1.0;
    half = 0.5;
    sMachineTolerance = 1.0;
    do {
		dbl_mult (&sMachineTolerance, &half, &sMachineTolerance);
		dbl_add (&sMachineTolerance, &one, &sum);
    } while (dbl_gt (&sum, &one));
	sMachineTolerance = (EtValue)2.0 * sMachineTolerance;
}

/*
// Description:
//   Find the zeros of a polynomial function on an interval
//   input:
//       Poly                 array of coefficients of polynomial
//       deg                  degree of polynomial
//       a, b                 interval of definition a < b
//       a_closed             include a in interval (TRUE or FALSE)
//       b_closed             include b in interval (TRUE or FALSE)
//   output: 
//       polyzero             number of roots 
//                            -1 indicates Poly == 0.0
//       Roots                zeroes of the polynomial on the interval
//   process:
//       Find all zeroes of the function on the open interval by 
//       recursively finding all of the zeroes of the derivative
//       to isolate the zeroes of the function.  Return all of the 
//       zeroes found adding the end points if the corresponding side
//       of the interval is closed and the value of the function 
//       is indeed 0 there.
//   restrictions:
//       The polynomial p is simply an array of deg+1 doubles.
//       p[0] is the constant term and p[deg] is the coef 
//       of t^deg.
//       The array roots should be dimensioned to deg+2. If the number
//       of roots returned is greater than deg, suspect numerical
//       instabilities caused by some nearly flat portion of Poly.
*/
EtInt
polyZeroes (EtValue Poly[], EtInt deg, EtValue a, EtInt a_closed, EtValue b, EtInt b_closed, EtValue Roots[])
{
	EtInt i, left_ok, right_ok, nr, ndr, skip;
	EtValue e, f, s, pe, ps, tol, *p, p_x[22], *d, d_x[22], *dr, dr_x[22];
	AG_POLYNOMIAL ply;

	e = pe = 0.0;  
	f = 0.0;

	for (i = 0 ; i < deg + 1; ++i) {
		f += (EtValue)fabs((double)Poly[i]);
	}
	tol = (fabs(a) + fabs(b))*(deg+1)*sMachineTolerance;

	/* Zero polynomial to tolerance? */
	if (f <= tol)  return(-1);

	p = p_x;  d = d_x;  dr = dr_x;
	for (i = 0 ; i < deg + 1; ++i) {
		p[i] = 1.0/f * Poly[i];
	}

	/* determine true degree */
	while ( fabs(p[deg]) < tol) deg--;

	/* Identically zero poly already caught so constant fn != 0 */
	nr = 0;
	if (deg == 0) return (nr);

	/* check for linear case */
	if (deg == 1) {
		Roots[0] = -p[0] / p[1];
		left_ok  = (a_closed) ? (a<Roots[0]+tol) : (a<Roots[0]-tol);
		right_ok = (b_closed) ? (b>Roots[0]-tol) : (b>Roots[0]+tol);
		nr = (left_ok && right_ok) ? 1 : 0;
		if (nr) {
			if (a_closed && Roots[0]<a) Roots[0] = a;
			else if (b_closed && Roots[0]>b) Roots[0] = b;
		}
		return (nr);
	}
	/* handle non-linear case */
	else {
		ply.p = p;  ply.deg = deg;

		/* compute derivative */
		for (i=1; i<=deg; i++) d[i-1] = i*p[i];

		/* find roots of derivative */
		ndr = polyZeroes ( d, deg-1, a, 0, b, 0, dr );
		if (ndr == -1) return (0);

		/* find roots between roots of the derivative */
		for (i=skip=0; i<=ndr; i++) {
			if (nr>deg) return (nr);
			if (i==0) {
				s=a; ps = ag_horner1( p, deg, s);
				if ( fabs(ps)<=tol && a_closed) Roots[nr++]=a;
			}
			else { s=e; ps=pe; }
			if (i==ndr) { e = b; skip = 0;}
			else e=dr[i];
			pe = ag_horner1( p, deg, e );
			if (skip) skip = 0;
			else {
				if ( fabs(pe) < tol ) {
					if (i!=ndr || b_closed) {
						Roots[nr++] = e;
						skip = 1;
					}
				}
				else if ((ps<0 && pe>0)||(ps>0 && pe<0)) {
					Roots[nr++] = ag_zeroin(s, e, 0.0, &ply );
					if ((nr>1) && Roots[nr-2]>=Roots[nr-1]-tol) { 
						Roots[nr-2] = (Roots[nr-2]+Roots[nr-1]) * 0.5;
						nr--;
					}
				}
			}
		}
	}

	return (nr);
} 

/*
//   Evaluate a polynomial in array form ( value only )
//   input:
//      P               array 
//      deg             degree
//      s               parameter
//   output:
//      ag_horner1      evaluated polynomial
//   process: 
//      ans = sum (i from 0 to deg) of P[i]*s^i
//   restrictions: 
//      deg >= 0           
*/
EtValue
ag_horner1 (EtValue P[], EtInt deg, EtValue s)
{
	EtValue h = P[deg];
	while (--deg >= 0) h = (s * h) + P[deg];
	return (h);
}


/*
//	Description:
//   Compute parameter value at zero of a function between limits
//   input:
//       a, b            real interval
//       f               real valued function of t and pars
//       tol             tolerance
//       pars            pointer to a structure
//   output:
//       ag_zeroin       zero of function
//   process:
//       Call ag_zeroin2 to find the zeroes of the function f(t, pars).
//       t is restricted to the interval [a, b].
//       pars is passed in as a pointer to a structure which contains
//       parameters for the function f.
//   restrictions:
//       f(a) and f(b) are of opposite sign.
//       Note that since pars comes at the end of both the
//         call to ag_zeroin and to f, it is an optional parameter.
//       If you already have values for fa,fb use ag_zeroin2 directly
*/
EtValue
ag_zeroin (EtValue a, EtValue b, EtValue tol, AG_POLYNOMIAL *pars)
{
	EtValue fa, fb;

	fa = ag_horner1 (pars->p, pars->deg, a);
	if (fabs(fa) < sMachineTolerance) return(a);

	fb = ag_horner1 (pars->p, pars->deg, b);
	if (fabs(fb) < sMachineTolerance) return(b);

	return (ag_zeroin2 (a, b, fa, fb, tol, pars));
} 

/*
//	Description
//   Compute parameter value at zero of a function between limits
//       with function values at limits
//   input:
//       a, b      real interval
//       fa, fb    double values of f at a, b
//       f         real valued function of t and pars
//       tol       tolerance
//       pars      pointer to a structure
//   output:
//       ag_zeroin2   a <= zero of function <= b
//   process:
//       We find the zeroes of the function f(t, pars).  t is
//       restricted to the interval [a, b].  pars is passed in as
//       a pointer to a structure which contains parameters
//       for the function f.
//   restrictions:
//       fa and fb are of opposite sign.
//       Note that since pars comes at the end of both the
//       call to ag_zeroin and to f, it is an optional parameter.
*/
EtValue
ag_zeroin2 (EtValue a, EtValue b, EtValue fa, EtValue fb, EtValue tol, AG_POLYNOMIAL *pars)
{
	EtInt test;
	EtValue c, d, e, fc, del, m, machtol, p, q, r, s;

	/* initialization */
	machtol = sMachineTolerance;

	/* start iteration */
label1:
	c = a;  fc = fa;  d = b-a;  e = d;
label2:
	if (fabs(fc) < fabs(fb)) {
		a = b;   b = c;   c = a;   fa = fb;   fb = fc;   fc = fa;
	}

	/* convergence test */
	del = 2.0 * machtol * fabs(b) + 0.5*tol;
	m = 0.5 * (c - b);
	test = ((fabs(m) > del) && (fb != 0.0));
	if (test) {
		if ((fabs(e) < del) || (fabs(fa) <= fabs(fb))) {
			/* bisection */
			d = m;  e= d;
		}
		else {
			s = fb / fa;
			if (a == c) {
				/* linear interpolation */
				p = 2.0*m*s;    q = 1.0 - s;
			}
			else {
				/* inverse quadratic interpolation */
				q = fa/fc;
				r = fb/fc;
				p = s*(2.0*m*q*(q-r)-(b-a)*(r-1.0));
				q = (q-1.0)*(r-1.0)*(s-1.0);
			}
			/* adjust the sign */
			if (p > 0.0) q = -q;  else p = -p;
			/* check if interpolation is acceptable */
			s = e;   e = d;
			if ((2.0*p < 3.0*m*q-fabs(del*q))&&(p < fabs(0.5*s*q))) {
				d = p/q;
			}
			else {
				d = m;	e = d;
			}
		}
		/* complete step */
		a = b;	fa = fb;
		if ( fabs(d) > del )   b += d;
		else if (m > 0.0) b += del;  else b -= del;
		fb = ag_horner1 (pars->p, pars->deg, b);
		if (fb*(fc/fabs(fc)) > 0.0 ) {
			goto label1;
		}
		else {
			goto label2;
		}
	}
	return (b);
}