/*
	CUBIC SOLVER

	Finds all the roots of a Cubic using Newton-Raphson method
*/
#include "cubicSolver.h"

#include <math.h>
#include <iostream>

#define MAXIT 100

int cubic_solver(float a, float b, float c, float d, float* roots)
{
	if (equals(a,0,1E-6f)) {
		if (equals(b, 0, 1E-6f)) {
			// linear
			roots[0] = -d / c;
			return 1;
		}
		else {
			// quadratic
			return quadratic_solver(b, c, d, roots[0], roots[1]);
		}
	}

	// oh no it's really a cubic...

	// bracket roots (intervals where there could be a root)
	float intervals[3][2] = { 0 };
	int nIntervals = cubic_bracket_roots(a,b,c,d, intervals);
	
	// find roots
	int ri = 0;
	for (int i = 0; i < nIntervals; ++i) {
		float x1 = intervals[i][0];
		float x2 = intervals[i][1];
		if (equals(x1, x2, 1E-6f)) continue; //skip 0,0 intervals
		if (!cubic_sign_change(a, b, c, d, x1, x2)) continue; //skip interval with no sign change
		roots[ri] = cubic_root_newton(a, b, c, d, x1, x2, 1E-6f);
		std::cout << "cubic root(" << i << "):" << roots[ri] << std::endl;
		ri++;
	}

	return ri; //nRootsFound
}

int cubic_bracket_roots(float a, float b, float c, float d, float intervals[3][2])
{
	const float min = -4.0f;
	const float max = 4.0f;

	float r1, r2;
	float qa = 3 * a;
	float qb = 2 * b;
	float qc = c;
	int nRoots = quadratic_solver(qa, qb, qc, r1, r2);
	std::cout << "(" << nRoots << ")" << r1 << "," << r2 << std::endl;

	float r1_df = cubic_derived2(a, b, r1);
	float r2_df = cubic_derived2(a, b, r2);
	std::cout << r1_df << "," << r2_df << std::endl;

	float ifx = cubic_inflection(a, b);
	std::cout << ifx << std::endl;
		

	int nIntervals = 0;

	if (nRoots == 2) {
		if (r1 > r2) {
			float tmp = r1;
			r1 = r2;
			r2 = tmp;
		}

		if (r1 < min) {
			r1 = min;
		}
		if (r2 < min) {
			r2 = min;
		}
		if (equals(r1 - r2, min, 1E-6f)) {
			std::cout << "min == min ?!" << std::endl;
			return 0;
		}

		if (r1 > max) {
			r1 = max;
		}
		if (r2 > max) {
			r2 = max;
		}
		if (equals(r1 - r2, max, 1E-6f)) {
			std::cout << "max == max ?!" << std::endl;
			return 0;
		}

		intervals[0][0] = min;
		intervals[0][1] = r1;
		intervals[1][0] = r1;
		intervals[1][1] = r2;
		intervals[2][0] = r2;
		intervals[2][1] = max;

		nIntervals = 3;
	}
	else if (nRoots == 1) {
		if (!equals(r1, ifx, 1E-6f)) {
			std::cout << "inflection points arent equals" << std::endl;
		}
		if (ifx < min) {
			ifx = min;
		}
		if (ifx > max) {
			r2 = min;
		}
		intervals[0][0] = min;
		intervals[0][1] = ifx;
		intervals[1][0] = ifx;
		intervals[1][1] = max;
		nIntervals = 2;
	}
	else {
		if (ifx < min) {
			ifx = min;
		}
		if (ifx > max) {
			r2 = min;
		}
		intervals[0][0] = min;
		intervals[0][1] = ifx;
		intervals[1][0] = ifx;
		intervals[1][1] = max;
		nIntervals = 2;
	}

	return nIntervals;
}

/*
	Solve a Quadratic equation
*/
int quadratic_solver(float a, float b, float c, float& root1, float& root2)
{
	if (equals(a, 0, 1E-6f)) {
		// solve linear equation (Bt + C = 0) -> t = -C/B
		root1 = -c / b;
		return 1; // 1 root
	}
	// solve quadratic equation (At^2 + Bt + C = 0)
	if (b*b < 4 * a*c) {
		return 0; //complex numbers = 0 roots
	}
	float root = sqrt(b*b - 4 * a*c);
	root1 = (-b + root) / (2 * a);
	root2 = (-b - root) / (2 * a);
	if (equals(root, 0, 1E-12f)) {
		return 1; // 1 root
	}
	else {
		return 2; // 2 roots
	}
}

/*
Newton-Raphson root finding
function = Cubic

Sample page from NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING (ISBN 0-521-43108-5)
Copyright (C) 1988-1992 by Cambridge University Press. Programs Copyright (C) 1988-1992 by Numerical Recipes Software.
Permission is granted for internet users to make one paper copy for their own personal use. Further reproduction, or any copying of machinereadable
files (including this one) to any server computer, is strictly prohibited. To order Numerical Recipes books or CDROMs, visit website
http://www.nr.com or call 1-800-872-7423 (North America only), or send email to directcustserv@cambridge.org (outside North America).

Adapted by Keven Villeneuve
*/
/*
Using a combination of Newton-Raphson and bisection, find the root of a function bracketed
between x1 and x2. The root, returned as the function value rtsafe, will be refined until
its accuracy is known within ±xacc. funcd is a user-supplied routine that returns both the
function value and the first derivative of the function.
*/
float cubic_root_newton(float a, float b, float c, float d, float x1, float x2, float xacc)
{
	int j;
	float df, dx, dxold, f, fh, fl;
	float temp, xh, xl, rts;

	fl = cubic(a, b, c, d, x1);
	df = cubic_derived(a, b, c, x1);
	fh = cubic(a, b, c, d, x2);
	df = cubic_derived(a, b, c, x2);
	if ((fl > 0.0 && fh > 0.0) || (fl < 0.0 && fh < 0.0))
		std::cout << "Root must be bracketed in rtsafe" << std::endl;
	if (fl == 0.0) return x1;
	if (fh == 0.0) return x2;
	if (fl < 0.0) { //Orient the search so that f(xl) < 0.
		xl = x1;
		xh = x2;
	}
	else {
		xh = x1;
		xl = x2;
	}
	rts = 0.5f*(x1 + x2); //Initialize the guess for root,
	dxold = (float)fabs(x2 - x1); //the “stepsize before last,”
	dx = dxold; //and the last step.
	f = cubic(a, b, c, d, rts);
	df = cubic_derived(a, b, c, rts);
	for (j = 1; j <= MAXIT; j++) { //Loop over allowed iterations.
		if ((((rts - xh)*df - f)*((rts - xl)*df - f) > 0.0) //Bisect if Newton out of range,
			|| (fabs(2.0*f) > fabs(dxold*df))) { //or not decreasing fast enough.
			dxold = dx;
			dx = 0.5f*(xh - xl);
			rts = xl + dx;
			if (xl == rts) return rts; //Change in root is negligible.
		}
		else { //Newton step acceptable. Take it.
			dxold = dx;
			dx = f / df;
			temp = rts;
			rts -= dx;
			if (temp == rts) return rts;
		}
		if (fabs(dx) < xacc) return rts; //Convergence criterion.
		f = cubic(a, b, c, d, rts);
		df = cubic_derived(a, b, c, rts);
		if (f < 0.0) //Maintain the bracket on the root.
			xl = rts;
		else
			xh = rts;
	}
	std::cout << "Maximum number of iterations exceeded in rtsafe" << std::endl;
	return 0.0; //Never get here.
}

/*
	Detect a sign change from x1 to x2
*/
bool cubic_sign_change(float a, float b, float c, float d, float x1, float x2)
{
	return cubic(a, b, c, d, x1) * cubic(a, b, c, d, x2) < 0;
}

bool equals(float a, float b, float epsillon)
{
	return fabs(a - b) < epsillon;
}

float cubic(float a, float b, float c, float d, float x)
{
	return a*x*x*x + b*x*x + c*x + d;
}

/*
	Derivative of a Cubic is a Quadratic
*/
float cubic_derived(float a, float b, float c, float x)
{
	return 3 * a*x*x + 2 * b*x + c;
}

/*
	Second derivative of a Cubic is Linear
*/
float cubic_derived2(float a, float b, float x)
{
	return 6 * a*x + 2 * b;
}

/*
	Cubic always has an inflection point
*/
float cubic_inflection(float a, float b)
{
	return -b / (3 * a);
}