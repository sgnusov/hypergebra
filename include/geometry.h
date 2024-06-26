#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "object.h"

using ld = long double;

class Line;

#include <iostream>

const cap_t POINT_CAP = 0x2;
const cap_t MOVABLE_POINT_CAP = 0x4;
const cap_t LINE_CAP = 0x8;
const cap_t POINT_ON_LINE_CAP = 0x10;
const cap_t FIXED_POINT_CAP = 0x20;
const cap_t CIRCLE_CAP = 0x40;
const cap_t POINT_ON_CIRCLE_CAP = 0x80;

class Point : public GeometryObject {
	ld coords[3];

protected:
	Point(const Point& p, cap_t capability);

public:
	Point();
	Point(ld x, ld y, ld z);
	Point(ld x, ld y);

	void render();
	Point projectPoint(const Point& p) const;

	ld& operator [] (int i);
	const ld& operator [] (int i) const;

	operator Line() const;

	ld sq() const;
	Point norm() const;
};

std::ostream& operator << (std::ostream& out, const Point& p);

bool operator == (const Point& u, const Point& v);

bool operator != (const Point& u, const Point& v);

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

	void correct();

	static Transformation identity();
	static Transformation rotateToPoint(const Point& p);
	static Transformation reverseRotateToPoint(const Point& p);
	static Transformation moveToXPoint(const Point& p);
	static Transformation reverseMoveToXPoint(const Point& p);
	static Transformation moveToPoint(const Point& p);
	static Transformation reverseMoveToPoint(const Point& p);
	static Transformation movePointToPoint(const Point& p, const Point& q);
};

Transformation operator ~ (const Transformation& t);

std::ostream& operator << (std::ostream& out, const Transformation& t);

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
	Point projectPoint(const Point& p) const;

	ld& operator [] (int i);
	const ld& operator [] (int i) const;

	operator Point() const;

	Point getPoint();
	Point getTangent(Point p);
};

class PointOnLine : public Point {
	Line l;

public:
	PointOnLine(const Point& p, const Line& l);
};

class FixedPoint : public Point {
public:
	FixedPoint(const Point& p);
};


class Circle : public GeometryObject {
	Point center;
	ld c;

public:
	Circle();
	Circle(const Point& center, const Point& p);
	Circle(const Point& center, ld c);

	void render();
	Point projectPoint(const Point& p) const;

	Point getCenter() const;
	ld getC() const;
};

class PointOnCircle : public Point {
	Circle c;

public:
	PointOnCircle(const Point& p, const Circle& c);
};

#endif
