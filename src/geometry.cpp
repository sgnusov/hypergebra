#include <cmath>

#include "geometry.h"
#include "common.h"

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

Point Point::projectPoint(const Point& p) const {
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

Point Point::norm() const {
	return (*this) / std::sqrt(-sq());
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

ld* Transformation::operator [] (int i) {
	return matrix[i];
}

const ld* Transformation::operator [] (int i) const {
	return matrix[i];
}

Transformation::operator const ld*() const {
	return (ld*)matrix;
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

void Transformation::correct() {
	for(int i = 0; i < 3; ++i)
		for(int j = 0; j <= i; ++j) {
			ld mod = 0;
			for(int k = 0; k < 3; ++k)
	 			mod += matrix[k][i] * matrix[k][j] * (k == 2 ? -1 : 1);
			if(i == j) mod = 1 - std::sqrt((i == 2 ? -1 : 1) / mod);
			for(int k = 0; k < 3; ++k) matrix[k][i] -= mod * matrix[k][j];
		}
}

Transformation Transformation::rotateToPoint(const Point& p) {
	Transformation res = Transformation::identity();
	ld r = std::sqrt(p[0] * p[0] + p[1] * p[1]);
	if(r >= 1e-12) {
		res[0][0] =  p[0] / r; res[0][1] = p[1] / r;
		res[1][0] = -p[1] / r; res[1][1] = p[0] / r;
	}
	return res;
};

Transformation Transformation::reverseRotateToPoint(const Point& p) {
	Transformation res = Transformation::identity();
	ld r = std::sqrt(p[0] * p[0] + p[1] * p[1]);
	if(r >= 1e-12) {
		res[0][0] =  p[0] / r; res[0][1] = -p[1] / r;
		res[1][0] =  p[1] / r; res[1][1] =  p[0] / r;
	}
	return res;
}

Transformation Transformation::moveToXPoint(const Point& p) {
	Transformation res = Transformation::identity();
	res[0][0] =  p[2]; res[0][2] = -p[0];
	res[2][0] = -p[0]; res[2][2] =  p[2];
	return res;
}

Transformation Transformation::reverseMoveToXPoint(const Point& p) {
	Transformation res = Transformation::identity();
	res[0][0] =  p[2]; res[0][2] =  p[0];
	res[2][0] =  p[0]; res[2][2] =  p[2];
	return res;
}

Transformation Transformation::moveToPoint(const Point& p) {
	Point q = Transformation::rotateToPoint(p) * p;
	return Transformation::reverseRotateToPoint(p) * Transformation::moveToXPoint(q) * Transformation::rotateToPoint(p);
}

Transformation Transformation::reverseMoveToPoint(const Point& p) {
	Point q = Transformation::rotateToPoint(p) * p;
	return Transformation::reverseRotateToPoint(p) * Transformation::reverseMoveToXPoint(q) * Transformation::rotateToPoint(p);
}

Transformation Transformation::movePointToPoint(const Point& p, const Point& q) {
	Point p_ = Transformation::moveToPoint(q) * p;
	return Transformation::reverseMoveToPoint(q) * Transformation::moveToPoint(p_) * Transformation::moveToPoint(q);
}

Transformation operator ~ (const Transformation& t) {
	ld det = 0;
	for(int i = 0; i < 3; ++i)
		det += t[0][i] * t[1][(i + 1) % 3] * t[2][(i + 2) % 3];
	for(int i = 0; i < 3; ++i)
		det -= t[0][i] * t[1][(i + 2) % 3] * t[2][(i + 1) % 3];

	assert(std::fabsl(det) > EPS);

	Transformation res;
	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j) {
			res[j][i] = (t[(i + 1) % 3][(j + 1) % 3] * t[(i + 2) % 3][(j + 2) % 3]
				- t[(i + 1) % 3][(j + 2) % 3] * t[(i + 2) % 3][(j + 1) % 3]) / det;
		}
	return res;
}

std::ostream& operator << (std::ostream& out, const Transformation& t) {
	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 3; ++j) {
			out << t[i][j] << ' ';
		}
		out << '\n';
	}
	return out;
}

bool operator == (const Point& u, const Point& v) {
	return u[0] == v[0] && u[1] == v[1] && u[2] == v[2];
}

bool operator != (const Point& u, const Point& v) {
	return !(u == v);
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

Point Line::projectPoint(const Point& p) const {
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

PointOnLine::PointOnLine(const Point& p, const Line& l) : Point(l.projectPoint(p), MOVABLE_POINT_CAP | POINT_ON_LINE_CAP), l(l) {}

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


Circle::Circle() : GeometryObject(CIRCLE_CAP), center(), c(0) {
	color = {172, 172, 172};
}


Circle::Circle(const Point& center_, const Point& p) : Circle() {
	center = center_;
	c = -center * p;
}

Circle::Circle(const Point& center_, ld c_) : Circle() {
	center = center_;
	c = c_;
}

// TODO: Fix division by zero
Point Circle::projectPoint(const Point& p) const {
	Point center1 = center * c;
	Point v = p * (-c / (p * center)) - center1;
	ld mod = std::sqrt((c * c - 1) / v.sq());
	return center1 + v * mod;
}

Point Circle::getCenter() const {
	return center;
}

ld Circle::getC() const {
	return c;
}

PointOnCircle::PointOnCircle(const Point& p, const Circle& c) : Point(c.projectPoint(p), MOVABLE_POINT_CAP | POINT_ON_CIRCLE_CAP), c(c) {}


