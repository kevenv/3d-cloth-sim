#ifndef CUBIC_SOLVER_H
#define CUBIC_SOLVER_H

/*
	Solve a Cubic equation
		ax^3 + bx^2 + cx + d = 0

	return: nb of roots found
*/
int cubic_solver(float a, float b, float c, float d, float* roots);

int cubic_bracket_roots(float a, float b, float c, float d, float intervals[3][2]);
int quadratic_solver(float a, float b, float c, float& root1, float& root2);
float cubic_root_newton(float a, float b, float c, float d, float x1, float x2, float xacc);
bool equals(float a, float b, float epsillon);
bool cubic_sign_change(float a, float b, float c, float d, float x1, float x2);
float cubic(float a, float b, float c, float d, float x);
float cubic_derived(float a, float b, float c, float x);
float cubic_derived2(float a, float b, float x);
float cubic_inflection(float a, float b);

#endif