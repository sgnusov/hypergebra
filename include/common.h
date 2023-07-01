#ifndef COMMON_H_
#define COMMON_H_

#include <cmath>

using ld = long double;

extern const ld EPS;

bool eq(ld a, ld b);

bool gt(ld a, ld b);

bool lt(ld a, ld b);

ld sinh(ld alpha);

ld cosh(ld alpha);

#endif
