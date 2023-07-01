#include "common.h"

const ld EPS = 1e-6;

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
