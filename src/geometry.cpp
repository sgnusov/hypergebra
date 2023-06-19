#include <cmath>

#include "geometry.h"

const ld EPS = 1e-12;

bool eq(ld a, ld b) {
	return std::abs(a - b) <= EPS;
}

bool gt(ld a, ld b) {
	return a - b > EPS;
}

bool lt(ld a, ld b) {
	return b - a > EPS;
}

ld sinh(ld alpha) {
	return (std::exp(alpha) - std::exp(-alpha)) / 2;
}

ld cosh(ld alpha) {
	return (std::exp(alpha) + std::exp(-alpha)) / 2;
}

Point::Point() : GeometryObject(POINT_CAP | MOVABLE_POINT_CAP, 1) {
	color = {37, 75, 198};
}

Point::Point(const Point& p, const cap_t capability_) : Point() {
	capability &= ~MOVABLE_POINT_CAP;
	capability |= capability_;
	coords[0] = p[0];
	coords[1] = p[1];
	coords[2] = p[2];
}

Point::Point(ld x, ld y, ld z) : Point() {
	coords[0] = x;
	coords[1] = y;
	coords[2] = z;
}

Point::Point(ld x, ld y) : Point(x, y, std::sqrt(1 + x * x + y * y)) {}

Point Point::projectPoint(const Point& p) {
	return *this;
}

ld& Point::operator [] (int i) {
	return coords[i];
}

const ld& Point::operator [] (int i) const {
	return coords[i];
}

ld Point::sq() const {
	return (*this) * (*this);
}

Transformation::Transformation() {
/*
	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j)
			matrix[i][j] = 0;
	for(int i = 0; i < 3; ++i)
		matrix[i][i] = 1;
*/
}

Transformation Transformation::identity() {
	Transformation res;
	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j)
			res[i][j] = 0;
	for(int i = 0; i < 3; ++i)
		res[i][i] = 1;
	return res;
}


ld* Transformation::operator [] (int i) {
	return matrix[i];
}

const ld* Transformation::operator [] (int i) const {
	return matrix[i];
}

Transformation::operator const ld*() const {
	return (ld*)matrix;
}

ld operator * (const Point& u, const Point& v) {
	return u[0] * v[0] + u[1] * v[1] - u[2] * v[2];
}

Point operator ^ (const Point& u, const Point& v) {
	return Point(-u[1] * v[2] + v[1] * u[2], -u[2] * v[0] + v[2] * u[0], u[0] * v[1] - v[0] * u[1]);
}
Point operator + (const Point& u, const Point& v) {
	return Point(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

Point operator - (const Point& u) {
	return Point(-u[0], -u[1], -u[2]);
}

Point operator - (const Point& u, const Point& v) {
	return u + (-v);
}

Point operator * (const Point& u, ld k) {
	return Point(u[0] * k, u[1] * k, u[2] * k);
}

Point operator / (const Point& u, ld k) {
	return u * (1 / k);
}
Point operator * (const Transformation& t, const Point& p) {
	Point res;
	for(int i = 0; i < 3; ++i) {
		res[i] = 0;
		for(int j = 0; j < 3; ++j) {
			res[i] += t[i][j] * p[j];
		}
	}
	return res;
}

Transformation operator * (const Transformation& t, const Transformation& u) {
	Transformation res;
	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j)
			res[i][j] = 0;
	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j)
			for(int k = 0; k < 3; ++k)
				res[i][k] += t[i][j] * u[j][k];
	return res;
}

Line::Line() : GeometryObject(LINE_CAP), coords() {
	color = {172, 172, 172};
}

Line::Line(ld x, ld y, ld z) : Line() {
	coords = Point(x, y, z);
	coords = coords / std::sqrt(coords.sq());
}

Line::Line(ld x, ld y) : Line(x, y, std::sqrt(x * x + y * y - 1)) {}

Line::Line(const Point& p, const Point& q) : Line(p ^ q) {};

Point Line::projectPoint(const Point& p) {
	Point q = p - coords * (p * coords);
	q = q / std::sqrt(-q.sq());
	return q;
}

ld& Line::operator [] (int i) {
	return coords[i];
}

const ld& Line::operator [] (int i) const {
	return coords[i];
}

Line::operator Point() const {
	return coords;
}

Point::operator Line() const {
	return Line(coords[0], coords[1], coords[2]);
}

Point Line::getPoint() {
	if(eq(coords[2], 0)) {
		return Point(0, 0, 1);
	}
	Point p = coords ^ Point(-coords[1], coords[0], 0);
	return p / std::sqrt(-p.sq());
}

Point Line::getTangent(Point p) {
	Point res = coords ^ p;
	return res / std::sqrt(res.sq());
}

std::ostream& operator << (std::ostream& out, const Point& p) {
	out << p[0] << ' ' << p[1] << ' ' << p[2];
	return out;
}

PointOnLine::PointOnLine(Point p, Line l) : Point(l.projectPoint(p), MOVABLE_POINT_CAP | POINT_ON_LINE_CAP), l(l) {}

/*
void PointOnLine::render() {
	p.render();
}

Point PointOnLine::projectPoint(const Point& p_) {
	return p.projectPoint(p_);
}
*/

FixedPoint::FixedPoint(const Point& p) : Point(p, FIXED_POINT_CAP) {
	color = {86, 86, 86};
}
