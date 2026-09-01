#ifndef BASE_MATH_H
#define BASE_MATH_H

////////////////////////////////
// NOTE: Basic Constants
//
#define PI     3.14159265358979324f
#define E      2.71828182845904523f
#define TAU    6.28318530717958648f
#define LOG2E  1.44269504088896339f
#define LOG10E 0.43429448190325182f

#define deg_to_rad(x) (F32)((x) * (PI / 180.0f))
#define rad_to_deg(x) (F32)((180.0f / PI) * x)

#define min(a, b)      (((a) < (b)) ? (a) : (b))
#define max(a, b)      (((a) > (b)) ? (a) : (b))
#define clamp(x, a, b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))
#define abs(x)         ((x) < 0 ? -(x) : (x))
#define square(x)      ((x) * (x))
#define lerp(a, b, t)  ((a) + ((b) - (a)) * (t))

#endif // BASE_MATH_H
