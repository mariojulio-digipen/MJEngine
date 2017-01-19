#pragma once

#include <vector>
#include <glm.hpp>
#include <map>
#include <glm.hpp>
#include <list>

struct un_s_pair
{
	double un;
	double arc_length;

	un_s_pair(double un, double arc_length) :
		un(un), arc_length(arc_length) {}

};

struct segment
{
	double ua;
	double ub;

	segment(double ua, double ub) :
		ua(ua), ub(ub) {}
};

struct compare_u
{
	bool operator() (double id, const un_s_pair& u)
	{
		return id < u.un;
	}

	bool operator() (const un_s_pair& u, double id)
	{
		return id > u.un;
	}
};

struct compare_s
{
	bool operator() (double id, const un_s_pair& s)
	{
		return id < s.arc_length;
	}

	bool operator() (const un_s_pair& s, double id)
	{
		return id > s.arc_length;
	}
};


class Spline
{
private:

	double t;
	double max_t;
	bool active;
	std::vector<glm::vec3> controlPoints;
	std::vector<glm::vec3> curvePoints; 
	std::vector<glm::vec3> coeffs;
	std::vector<glm::vec3>* cps;

	std::vector<un_s_pair> arc_length_table;
	std::list<segment> segment_list;
	std::vector<segment> _segment_list_debug;

	void buildArcLengthTable(std::list<segment>::iterator* sgmnt_table);
	void normalizeArcLengthTable();

public:
	Spline();
	~Spline();

	// returns a set of points in the curve
	std::vector<glm::vec3> CubicSpline(std::vector<glm::vec3>* controlPoints);
	void ClearCurvePoints();
	double* FindLengthGivenU(double u);

	glm::vec3 GetPointPu(double u);

	std::vector<glm::vec3> GetArcLengthPoints();


	// Retrieve an S at any Uk
	double CurveDistanceAtU(double u);

	// Retrieve an U at any Sk
	double UatCurveDistance(double s);

	double VelocityTime(float deltaTime, double t1, double t2);

	double VelocityTime(float t, float deltaTime, double t1, double t2);
	// distance_time function of the curve using a test cuadratic function
	double DistanceTime(float deltaTime);

	// distance_time function of the curve using ease-in/ease-out,
	// predefined ranges are used.
	double DistanceTimeControlledPeace(float deltaTime, double peace, double t1, double t2);

	// Get Distance according to an specific time t
	double DistanceTime(double t, float deltaTime, double t1, double t2);

	double GetCurrentTime() { return t; }
	
	// reset time t
	void ResetTime() { t = 0.0000; }

	bool IsActive() { return active; }
	void Activate() { active = true; }
	void Deactivate() { active = false; }

};

