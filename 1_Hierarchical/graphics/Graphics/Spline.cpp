#include "Spline.h"

#include <algorithm>
#include <glm.hpp>

int idx = 0;
void inverse(std::vector<std::vector<double>>& M);

Spline::Spline() :t(0.0000), max_t(0.0000), active(false)
{
}


Spline::~Spline()
{
}

// Deletes the whole curve space when Reset button is pressed in the UI.
void Spline::ClearCurvePoints()
{
	if (curvePoints.size() > 0) curvePoints.clear();
	if (controlPoints.size() > 0) 	controlPoints.clear();
}

// Cubic polynomial spline
// This creates a matrix of equations in order to
// calculate a set of coefficients required to find
// the interpolation polynomials for each x, y and z 
// components of the position of every single point
// in the space curve.
std::vector<glm::vec3> Spline::CubicSpline(std::vector<glm::vec3>* controlPoints)
{
	//_segment_list_debug.clear();

	idx = 0;
	segment_list.clear();
	arc_length_table.clear();
	curvePoints.clear();
	coeffs.clear();
	cps = controlPoints;
	
	float maxHeight = (*controlPoints)[0].y + 0.2f; // just set up an initial position in Y axis of the character.
	
	// creates a space curve only if there are at least 2 control points
	if (controlPoints->size() <= 1)
	{
		return curvePoints;
	}

	// build up the equation matrix
	std::vector<std::vector<double>> equationsMatrix;
	int equationsNumber = controlPoints->size() + 2;

	for (int i = 0; i < equationsNumber; i++)
	{
		std::vector<double> vec;
		equationsMatrix.push_back(vec);
	}

	// The following lines create the equations matrix
	// required to compute the coefficients

	// first pattern -> 2 first rows:
	// very first element always 1 and other elements of row#0 are 0:
	equationsMatrix[0].push_back(1.0); //0
	for (int i = 1; i < equationsNumber; i++)
	{
		equationsMatrix[0].push_back(0.0);
	}
	// row#1: 4 first elements always 1 and others will be 0
	equationsMatrix[1].push_back(1.0); //0
	equationsMatrix[1].push_back(1.0); //1
	equationsMatrix[1].push_back(1.0); //2
	equationsMatrix[1].push_back(1.0); //3
	for (int i = 4; i < equationsNumber; i++)
	{
		equationsMatrix[1].push_back(0.0);
	}

	// forward from row#2 to equationsNumber, we have a new pattern
	int pt = 0;
	for (int t = 2; t < equationsNumber - 2; t++)
	{
		equationsMatrix[t].push_back(1.0);//0
		equationsMatrix[t].push_back(t);//1
		equationsMatrix[t].push_back(t*t);//2
		equationsMatrix[t].push_back(t*t*t);//3
		for (int i = 4; i < equationsNumber; i++)
		{
			if (t <= (i - 3))
			{
				equationsMatrix[t].push_back(0);
			}
			else
			{
				equationsMatrix[t].push_back((t - (i - 3))*(t - (i - 3))*(t - (i - 3)));
			}
		}
		pt = t;
	}

	// and finally, filling the free coefficients, using f''(0) and f''(k)
	// also with unrolled recursion, so we can gain a bit more efficiency, 
	// consiering that these last coefficients are only 2
	// f''(0)
	int currentEq = equationsNumber - 2;
	equationsMatrix[currentEq].push_back(0.0);
	equationsMatrix[currentEq].push_back(0.0);
	equationsMatrix[currentEq].push_back(2.0);
	equationsMatrix[currentEq].push_back(0.0); // 6*t, but here, t = 0 = f''(0)
	for (int i = 4; i < equationsNumber; i++)
	{
		if (pt<(i - 3))
		{
			equationsMatrix[currentEq].push_back(0.0);
		}
		else
		{
			equationsMatrix[currentEq].push_back(6 * (pt - (pt - 3)));
		}
	}

	//f''(k)
	currentEq = equationsNumber - 1;
	int t = controlPoints->size() - 1; //k
	equationsMatrix[currentEq].push_back(0.0);
	equationsMatrix[currentEq].push_back(0.0);
	equationsMatrix[currentEq].push_back(2.0);
	equationsMatrix[currentEq].push_back(6 * t);
	for (int i = 4; i < equationsNumber; i++)
	{
		if (t<(i - 3))
		{
			equationsMatrix[currentEq].push_back(0.0);
		}
		else
		{
			equationsMatrix[currentEq].push_back(6 * (t - (i - 3)));
		}
	}

	// now find the coefficients -> easy :)
	inverse(equationsMatrix); // I'm reusing a function I was allwed to use in my MAT500 course.
	
	/*std::vector<double> coeffsX;
	std::vector<double> coeffsZ;*/
	double sumOfProdsX = 0;
	double sumOfProdsZ = 0;
	for (int i = 0; i < equationsNumber; i++)
	{
		for (int j = 0; j < equationsNumber - 2; j++)
		{
			sumOfProdsX = sumOfProdsX + equationsMatrix[i][j] * (*controlPoints)[j].x;
			sumOfProdsZ = sumOfProdsZ + equationsMatrix[i][j] * (*controlPoints)[j].z;
		}
		
		coeffs.push_back(glm::vec3(sumOfProdsX, 0.0f, sumOfProdsZ));
		
		sumOfProdsX = 0;
		sumOfProdsZ = 0;
	}

	// finally...(!!) calculate the curve points!
	double jump = 0.0005;
	std::vector<double> tmpUn;
	for (double u = 0.0000; u < 1.0000; u += jump)
	{
		// generates curve points
		tmpUn.push_back(u);
		
		// I create the GetPointPu function conveniently, because
		// I will need it to get a point according to any u that 
		// I do no know between 0 and 1.
		glm::vec3 point = GetPointPu(u);

		curvePoints.push_back(point);

		// Here is where I create the segments list
		if (u < 1.0000 - jump)
		{
			segment sgmt(u, u + jump);
			segment_list.push_back(sgmt);
		}
	}

	// Here is where I create the arclentgh table
	// Starts from 0.0 | 0.0
	un_s_pair initialG_u(0.0, 0.0); // u | G(u)
	arc_length_table.push_back(initialG_u);
	
	// We pass these first values of the table
	// and build the table with the buildArcLengthTable function
	// using the values in the segment list
	auto i = segment_list.begin();
	while(i != segment_list.end())
		buildArcLengthTable(&i);
	normalizeArcLengthTable();
	


	// this is a debug curve
	/*_segment_list_debug.insert(_segment_list_debug.begin(),
		segment_list.begin(), segment_list.end());*/
	// end debug

	return curvePoints;
}

double* Spline::FindLengthGivenU(double u)
{
	if (arc_length_table.size() == 0)
		return NULL;
	
	auto ptr = std::lower_bound(arc_length_table.begin(), arc_length_table.end(), u, compare_u());
	return &((*ptr).arc_length);
}

std::vector<glm::vec3> Spline::GetArcLengthPoints()
{
	std::vector<glm::vec3> vct;

	for (size_t i = 0; i < arc_length_table.size(); i++)
	{
		glm::vec3 pt = GetPointPu(arc_length_table[i].un);
		vct.push_back(pt);
	}

	return vct;
}

// Makes the arclength table have values between 0 and 1
void Spline::normalizeArcLengthTable()
{
	double max_length = arc_length_table.back().arc_length;
	auto i = arc_length_table.begin();
	while (i != arc_length_table.end())
	{
		i->arc_length = i->arc_length / max_length;
		i++;
	}
}

// Creates an arclength table using a linked list (std::list)
void Spline::buildArcLengthTable(std::list<segment>::iterator* i)
{
	// Let's assume I'm in an interval ua -> ub.
	// 1. Find the um:
	//int ua_idx = 0; // next ua_idx = (ua_idx + 1)
	//int ub_idx = 1; // next ub_idx = (ub_idx + 1)
	double ua = (*i)->ua;
	double ub = (*i)->ub;
	double um = (ua + ub) * 0.5;

	// 2. Find the linear distance as follows:
	// P(ua)------P(um)------P(ub) //
	// Then, I need: 
	// distance(P(ua)------P(um))
	// distance(P(um)------P(ub))
	// distance(P(ua)------P(ub))
	// So first, find P(un):
	glm::vec3 p_ua = GetPointPu(ua);
	glm::vec3 p_um = GetPointPu(um);
	glm::vec3 p_ub = GetPointPu(ub);
	// now distances:
	double A = glm::distance(p_ua, p_um);
	double B = glm::distance(p_um, p_ub);
	double C = glm::distance(p_ua, p_ub);
	// ah! remember defining a tolerance:
	double tol = 0.000000001;

	// 3. Evaluate if the sum of two distances sum = ua_um and um_ub 
	// is above to ua_ub plus the tolerance then:
	// segment_list.add(ua_um) (this is first)
	// segment_list.add(um_ub) (this goes second)
	// ----- in order! -----
	// so in summary: 
	// if sum - ua_ub > tol => add to segments table ua_um & um_ub, in that order
	// if sum - ua_ub <= tol => add to arclength table 
	// um and s(um) = G(ua) plus distance from P(ua) to P(um)
	// also, add to the table -> 
	// ub and arclength from P(um) to P(ub)
	auto to_delete = (*i);
	double d = A + B - C;
	if (d > tol)
	{
		segment ua_um(ua, um);
		segment um_ub(um, ub);
		auto tmp = (*i);
		std::vector<segment> to_insert = { ua_um, um_ub };
		segment_list.insert((*i), to_insert.begin(), to_insert.end());
		// remove ua, ub
		(*i) = segment_list.erase(to_delete);
		
		--(*i);
		--(*i); 
		// I just go backwards two elements in the linked list (Std::list)
		// because I added two elements (and they will always be two to add)
		// so that I do not need to create a recursive function.
	}
	else
	{
		// record um, sm; where sm = G(Ua) + A
		double G_ua = arc_length_table.back().arc_length;
		double sm = G_ua + A;
		un_s_pair usm_pair(um, sm);
		arc_length_table.push_back(usm_pair);

		// record ub, sb; where sb = G(Um) + B
		double G_um = arc_length_table.back().arc_length;
		double sb = G_um + B;
		un_s_pair usb_pair(ub, sb);
		arc_length_table.push_back(usb_pair);
		(*i) = segment_list.erase(to_delete);
		
	}

	
}

// Returns the distance traveled at a specific u, just in case...
double Spline::CurveDistanceAtU(double u)
{
	std::vector<un_s_pair>::iterator begin = arc_length_table.begin();
	std::vector<un_s_pair>::iterator end = arc_length_table.end();

	// binary search with the compare_u function, so I get the upper bound (ptr)
	// and with a pointer to it, get the lower bound also (ptr_prv).
	auto ptr = std::upper_bound(begin, end, u, compare_u());
	auto ptr_prv = ptr - 1;

	double u_k_1 = ((*ptr_prv).un);
	double s_k_1 = ((*ptr_prv).arc_length);
	double u_k = ((*ptr).un);
	double s_k = ((*ptr).arc_length);

	double l = (u - u_k_1) / (u_k - u_k_1);
	double s_gu = (1 - l)*s_k_1 + l*s_k;
	return s_gu;
}

// Returns the value of u at a specific distance traveled s
double Spline::UatCurveDistance(double s)
{
	std::vector<un_s_pair>::iterator begin = arc_length_table.begin();
	std::vector<un_s_pair>::iterator end = arc_length_table.end();

	// binary search with the compare_s function, so I get the upper bound (ptr)
	// and with a pointer to it, get the lower bound also (ptr_prv).
	auto ptr = std::upper_bound(begin, end, s, compare_s());
	auto ptr_prv = ptr - 1;

	double u_k_1 = ((*ptr_prv).un);
	double s_k_1 = ((*ptr_prv).arc_length);
	double u_k = ((*ptr).un);
	double s_k = ((*ptr).arc_length);

	double l = (s - s_k_1) / (s_k - s_k_1);
	double u = (1 - l)*u_k_1 + l*u_k;

	return u;
}

// this is a test function, disregard it.
double Spline::DistanceTime(float deltaTime)
{
	max_t = 10.0000; // let's try with 10 secs
	double jump = 0.01;
	t = t + jump;
	//t = (t / max_t);// * deltaTime;

	if (t >= 1)
		t = 0.0000;

	double s = (2 - t)*t;

	return s;
}

// this is the distance_time function
// return s for every t along the space curve
double Spline::DistanceTimeControlledPeace(float deltaTime, double peace, double t1, double t2)
{
	double jump = 0.001*peace;
	t = t + jump;

	double vc = 2 / (1-t1+t2);
	double s = 0.0000;

	if (t >= 0.0000 && t < t1)
		s = (vc / (2 * t1)) * t*t;
	else if (t >= t1 && t < t2)
		s = vc*(t - t1*0.5);
	else if (t >= t2)
		s = (vc*(t - t2) / (2 * (1 - t2)))*(2 - t - t2) + vc*(t2 - t1*0.5);

	if (t >= 1)
		t = 1;
	return s;
}

// this is another version of the distance_time function
// any t in the space curve must be provided
double Spline::DistanceTime(double t, float deltaTime, double t1, double t2)
{
	double vc = 2 / (1 - t1 + t2);
	double s = 0.0000;

	if (t >= 0.0000 && t < t1)
		s = (vc / (2 * t1)) * t*t;
	else if (t >= t1 && t < t2)
		s = vc*(t - t1*0.5);
	else if (t >= t2)
		s = (vc*(t - t2) / (2 * (1 - t2)))*(2 - t - t2) + vc*(t2 - t1*0.5);

	return s;
}

// disregard
double Spline::VelocityTime(float deltaTime, double t1, double t2)
{
	return 0.0;
}

// returns the velocity at a particular t of the space curve
double Spline::VelocityTime(float t, float deltaTime, double t1, double t2)
{
	double vc = 2 / (1 - t1 + t2);
	double vt;
	if (t >= 0.0000 && t < t1)
		vt = (vc / t1) * t;
	else if (t >= t1 && t < t2)
		vt = vc;
	else if (t >= t2 && t <= 1.0000)
		vt = (vc / (1 - t2)) * (1 - t);

	return vt;
}

// This is the P(u) function
// Returns a point in the curve, according to any u
// using the coefficients found during the cubic 
// spline creation.
glm::vec3 Spline::GetPointPu(double u)
{
	double pointX = 0;
	double pointZ = 0;
	int equationsNumber = cps->size() + 2;
	float maxHeight = (*cps)[0].y + 0.2f;
	double tf = u * (cps->size() - 1);

	pointX = coeffs[0].x + coeffs[1].x * tf + coeffs[2].x * tf*tf + coeffs[3].x * tf*tf*tf;
	pointZ = coeffs[0].z + coeffs[1].z * tf + coeffs[2].z * tf*tf + coeffs[3].z * tf*tf*tf;
	for (int i = 4; i < equationsNumber; ++i)
	{
		if (tf>(i - 3))
		{
			pointX = pointX + coeffs[i].x * (tf - (i - 3))*(tf - (i - 3))*(tf - (i - 3));
			pointZ = pointZ + coeffs[i].z * (tf - (i - 3))*(tf - (i - 3))*(tf - (i - 3));
		}
	}

	glm::vec3 point = glm::vec3(static_cast<float>(pointX), maxHeight, static_cast<float>(pointZ));
	return point;
}

// Returns the inverse of matrix 'M'.
void inverse(std::vector<std::vector<double>>& M)
{
	// I use Guassian Elimination to calculate the inverse:
	// (1) 'augment' the matrix (left) by the identity (on the right)
	// (2) Turn the matrix on the left into the identity by elemetry row ops
	// (3) The matrix on the right is the inverse (was the identity matrix)
	// There are 3 elemtary row ops: (I combine b and c in my code)
	// (a) Swap 2 rows
	// (b) Multiply a row by a scalar
	// (c) Add 2 rows

	//if the matrix isn't square: exit (error)
	if (M.size() != M[0].size()) { return; }

	//create the identity matrix (I), and a copy (C) of the original
	int i = 0, ii = 0, j = 0, dim = M.size();
	double e = 0;
	std::vector<std::vector<double>> I;
	std::vector<std::vector<double>> C;

	I.resize(dim);
	C.resize(dim);
	for (i = 0; i < dim; ++i)
	{
		I[i].resize(dim);
		C[i].resize(dim);
	}

	for (i = 0; i<dim; i++)
	{
		for (j = 0; j<dim; j++)
		{
			//if we're on the diagonal, put a 1 (for identity)
			if (i == j) { I[i][j] = 1; }
			else { I[i][j] = 0; }

			// Also, make the copy of the original
			C[i][j] = M[i][j];
		}
	}

	// Perform elementary row operations
	for (i = 0; i<dim; i++)
	{
		// get the element e on the diagonal
		e = C[i][i];

		// if we have a 0 on the diagonal (we'll need to swap with a lower row)
		if (e == 0)
		{
			//look through every row below the i'th row
			for (ii = i + 1; ii<dim; ii++)
			{
				//if the ii'th row has a non-0 in the i'th col
				if (C[ii][i] != 0)
				{
					//it would make the diagonal have a non-0 so swap it
					for (j = 0; j<dim; j++)
					{
						e = C[i][j];       //temp store i'th row
						C[i][j] = C[ii][j];//replace i'th row by ii'th
						C[ii][j] = e;      //repace ii'th by temp
						e = I[i][j];       //temp store i'th row
						I[i][j] = I[ii][j];//replace i'th row by ii'th
						I[ii][j] = e;      //repace ii'th by temp
					}
					//don't bother checking other rows since we've swapped
					break;
				}
			}
			//get the new diagonal
			e = C[i][i];
			//if it's still 0, not invertable (error)
			if (e == 0) { return; }
		}

		// Scale this row down by e (so we have a 1 on the diagonal)
		for (j = 0; j<dim; j++)
		{
			C[i][j] = C[i][j] / e; //apply to original matrix
			I[i][j] = I[i][j] / e; //apply to identity
		}

		// Subtract this row (scaled appropriately for each row) from ALL of
		// the other rows so that there will be 0's in this column in the
		// rows above and below this one
		for (ii = 0; ii<dim; ii++)
		{
			// Only apply to other rows (we want a 1 on the diagonal)
			if (ii == i) { continue; }

			// We want to change this element to 0
			e = C[ii][i];

			// Subtract (the row above(or below) scaled by e) from (the
			// current row) but start at the i'th column and assume all the
			// stuff left of diagonal is 0 (which it should be if we made this
			// algorithm correctly)
			for (j = 0; j<dim; j++)
			{
				C[ii][j] -= e*C[i][j]; //apply to original matrix
				I[ii][j] -= e*I[i][j]; //apply to identity
			}
		}
	}

	//we've done all operations, C should be the identity
	//matrix I should be the inverse:
	for (i = 0; i<dim; i++)
	{
		for (j = 0; j<dim; j++)
		{
			M[i][j] = I[i][j];
		}
	}
}
