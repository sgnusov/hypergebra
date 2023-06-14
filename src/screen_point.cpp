#include "screen_point.h"

ScreenPoint::ScreenPoint(ld x, ld y) {
	coords[0] = x;
	coords[1] = y;
}

ld& ScreenPoint::operator [] (int i) {
	return coords[i];
}

const ld& ScreenPoint::operator [] (int i) const {
	return coords[i];
}

ld ScreenPoint::sq() {
	return coords[0] * coords[0] + coords[1] * coords[1];
}

ScreenPoint operator - (const ScreenPoint& u, const ScreenPoint& v) {
	return ScreenPoint(u[0] - v[0], u[1] - v[1]);
}
