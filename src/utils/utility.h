#pragma once
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <bit>
#include <stdio.h>
#define MAX(a,b) (a > b ? a : b)
#define deltaLerp(a, b, t, delta) (powf(t, delta) * (a)) + ((1.0f - powf(t, delta)) * (b))
typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef long i32;
typedef unsigned long u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;