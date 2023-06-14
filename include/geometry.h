#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "object.h"

using ld = long double;

class Line;

#include <iostream>

class Point : public GeometryObject {
	ld coords[3];

public:
	Point();
	Point(ld x, ld y, ld z);
	Point(ld x, ld y);

	void render();
	Point projectPoint(const Point& p);

	ld& operator [] (int i);
	const ld& operator [] (int i) const;

	operator Line() const;

	ld sq();
};

std::ostream& operator << (std::ostream& out, const Point& p);

ld operator * (const Point& u, const Point& v);

Point operator ^ (const Point& u, const Point& v);

Point operator + (const Point& u, const Point& v);

Point operator - (const Point& u);

Point operator - (const Point& u, const Point& v);

Point operator * (const Point& u, ld k);

Point operator / (const Point& u, ld k);


class Transformation {
	ld matrix[3][3];

public:
	Transformation();
	ld* operator [] (int i);
	const ld* operator [] (int i) const;
	operator const ld*() const;
	static Transformation identity();
};

Point operator * (const Transformation& t, const Point& p);

Transformation operator * (const Transformation& t, const Transformation& u);



class Line : public GeometryObject {
	Point coords;

public:
	Line();
	Line(ld x, ld y, ld z);
	Line(ld x, ld y);
	Line(const Point& p, const Point& q);

	void render();
	Point projectPoint(const Point& p);

	ld& operator [] (int i);
	const ld& operator [] (int i) const;

	operator Point() const;

	Point getPoint();
	Point getTangent(Point p);
};


#endif
