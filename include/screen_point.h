#ifndef SCREEN_POINT_H_
#define SCREEN_POINT_H_

#include <ostream>

using ld = long double;

class ScreenPoint {
	ld coords[2];

public:
	ScreenPoint(ld x, ld y);

	ld& operator [] (int i);
	const ld& operator [] (int i) const;

	ld sq();
};

ScreenPoint operator - (const ScreenPoint& u, const ScreenPoint& v);

std::ostream& operator << (std::ostream& out, const ScreenPoint& p);

#endif
