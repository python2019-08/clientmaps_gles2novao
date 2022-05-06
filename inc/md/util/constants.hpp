#pragma once
 
#define _USE_MATH_DEFINES 1
#include <math.h>
#include <cmath>
#include <string>
#include <vector>

#include <md/util/chrono.hpp> 

#ifndef _MATH_DEFINES_DEFINED
#define _MATH_DEFINES_DEFINED
// Definitions of useful mathematical constants
//
// Define _USE_MATH_DEFINES before including <math.h> to expose these macro
// definitions for common math constants.  These are placed under an #ifdef
// since these commonly-defined names are not part of the C or C++ standards
#define M_E        2.71828182845904523536   // e
#define M_LOG2E    1.44269504088896340736   // log2(e)
#define M_LOG10E   0.434294481903251827651  // log10(e)
#define M_LN2      0.693147180559945309417  // ln(2)
#define M_LN10     2.30258509299404568402   // ln(10)
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4
#define M_1_PI     0.318309886183790671538  // 1/pi
#define M_2_PI     0.636619772367581343076  // 2/pi
#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#define M_SQRT2    1.41421356237309504880   // sqrt(2)
#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)
#endif
namespace clientmap {
 
constexpr int32_t tileEXTENT = 4096;


 

constexpr double DEG2RAD = M_PI / 180.0;
constexpr double RAD2DEG = 180.0 / M_PI;
constexpr double M2PI = M_PI * 2;
constexpr double LATITUDE_MAX = 85.051128779806604;
constexpr double LONGITUDE_MAX = 180;
constexpr double DEGREES_MAX = 360;
constexpr double PITCH_MIN = 0.0;
constexpr double PITCH_MAX = 83.1* DEG2RAD;
constexpr double MIN_ZOOM = 0.0;
constexpr double MAX_ZOOM = 22.0;
constexpr uint8_t MIN_ZOOM_I = 0;
constexpr uint8_t MAX_ZOOM_I = 22;
constexpr uint8_t BASE_ZOOM = MAX_ZOOM_I;
// ONE_EM constant used to go between "em" units used in style 
// spec and "points" used internally for layout.
constexpr float ONE_EM = 24.0f;

constexpr uint8_t DEFAULT_PREFETCH_ZOOM_DELTA = 4;

constexpr uint64_t DEFAULT_MAX_CACHE_SIZE = 50 * 1024 * 1024;

// Default ImageManager's cache size for images added via onStyleImageMissing API.
// Average sprite size with 1.0 pixel ratio is ~2kB, 8kB for pixel ratio of 2.0.
constexpr std::size_t DEFAULT_ON_DEMAND_IMAGES_CACHE_SIZE = 100 * 8192;

constexpr Duration DEFAULT_TRANSITION_DURATION = Milliseconds(300);
constexpr Seconds  CLOCK_SKEW_RETRY_TIMEOUT { 30 };
   
constexpr uint32_t DEFAULT_MAXIMUM_CONCURRENT_REQUESTS = 20;

constexpr uint8_t TERRAIN_RGB_MAXZOOM = 15; 
 
} // namespace MdU//namespace md
