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
#ifndef CUBIC_SOLVER_ADSK_H
#define CUBIC_SOLVER_ADSK_H

typedef float EtValue;		/* type used for key values (in internal units)	*/
typedef int EtInt;			/* natural int representation					*/
typedef void EtVoid;		/* void											*/

typedef struct ag_polynomial {
	EtValue *p;
	EtInt deg;
} AG_POLYNOMIAL;

/*
// statics for sMachineTolerance computation
*/


EtInt
dbl_gt (EtValue *a, EtValue *b);
EtVoid
dbl_mult (EtValue *a, EtValue *b, EtValue *atimesb);
EtVoid
dbl_add (EtValue *a, EtValue *b, EtValue *aplusb);

EtVoid
init_tolerance ();

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
polyZeroes (EtValue Poly[], EtInt deg, EtValue a, EtInt a_closed, EtValue b, EtInt b_closed, EtValue Roots[]);

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
ag_horner1 (EtValue P[], EtInt deg, EtValue s);

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
ag_zeroin (EtValue a, EtValue b, EtValue tol, AG_POLYNOMIAL *pars);

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
ag_zeroin2 (EtValue a, EtValue b, EtValue fa, EtValue fb, EtValue tol, AG_POLYNOMIAL *pars);

#endif