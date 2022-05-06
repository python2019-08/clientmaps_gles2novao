#pragma once

#define _USE_MATH_DEFINES 1
#include <math.h>
#include <cmath>//#include <limits>
#include <cfloat>
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <array>
#include <md/util/geom.hpp> 
#if defined(__ANDROID__)
#include <android/api-level.h>
#endif
  
namespace clientmap {
//template <typename T>
//using Point = glm::vec<2, T, glm::defaultp>;
 

/**
 * ������������
 */
const double INF = 1E200;	/* ����ֵ */
const double EP = 1E-10;	/* ��Сֵ */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


/**
 * ������Ϣ������������
 */
const double	WGS84_EE = 0.0066943799013;				// ƫ����
const int		WGS84_A = 6378137;						// ���뾶


/**
 * ��������������
 */
 // ����������С
#define D_EPSILON 0.000001
#define F_EPSILON 0.000001f
//#define D_EPSILON std::numeric_limits<double>::epsilon()
//#define F_EPSILON std::numeric_limits<float>::epsilon()

 // �жϸ������Ƿ����
#define D_EQUALS(a, b) (std::abs ((a) - (b)) < D_EPSILON)
#define F_EQUALS(a, b) (std::abs ((a) - (b)) < F_EPSILON)

 // �жϸ������Ƿ���Ч
#define D_ISVALID(n) ((n) == (n) && (n) <= DBL_MAX && (n) >= -DBL_MAX)
#define F_ISVALID(n) ((n) == (n) && (n) <= FLT_MAX && (n) >= -FLT_MAX)

 // �������Ƚ� ��ȷ���0 ǰ�߽ϴ󷵻�1 ���߽ϴ󷵻�-1
#define D_CMP(a, b) (D_EQUALS((a),(b)) ? 0 : ((a) > (b) ? 1 : -1))
#define F_CMP(a, b) (F_EQUALS((a),(b)) ? 0 : ((a) > (b) ? 1 : -1))


/**
 * ������ѧ����
 */
 // ��������
#ifndef ROUND
#define ROUND(d) (((d) > 0.0) ? /*floor*/(int)((d) + 0.5) : /*ceil*/(int)((d) - 0.5))
#endif

// ȡģ���� ���Ÿ��汻����b
#define D_MOD(a, b) fmod((fmod((a), (b)) + (b)), (b))
#define F_MOD(a, b) fmodf((fmodf((a), (b)) + (b)), (b))
#define I_MOD(a, b) ((((a) % (b)) + (b)) % (b))


/**
 * �Ƕ��������
 */
 // �Ƕ���ת������
#ifndef GET_RADIANS
#define GET_RADIANS(degree) (double)((degree) * M_PI / 180.0)
#endif

// ������ת�Ƕ���
#ifndef GET_DEGREES
#define GET_DEGREES(radian) (double)((radian) * 180.0 / M_PI)
#endif

// �Ƕ�ֵת��λ�� ����˳ʱ��н� [0, 360)
#ifndef GET_AZIMUTH
#define GET_AZIMUTH(degree) ((((int32)(degree)) % 360 + 360) % 360)
#endif

// �Ƕ�ֵת��λ�� �������汾 ����˳ʱ��н� [0, 360) ������λС��
// gaojian mark 2019-10-30 GET_AZIMUTH_DOUBLE ���Ⱥܵ� ���������ھ�ȷ�������ۼ�������
#ifndef GET_AZIMUTH_DOUBLE
//#define GET_AZIMUTH_DOUBLE(degree) ((((int32)(floor((double)(degree))) % 360 + 360) % 360) + ((double)(degree)) - (floor((double)(degree))))
#define GET_AZIMUTH_DOUBLE(degree) ((((int64)((degree) * 1E7) % 3600000000 + 3600000000) % 3600000000) * 1.0 / 1E7)
#endif

// ��ȡ���ǶȲ�ֵ ȡ�ӽ�
#ifndef GET_DEGREE_DIFF
#define GET_DEGREE_DIFF(to, from) (GET_AZIMUTH((int32)(to) - (int32)(from) + 180) - 180)
#endif
#ifndef GET_DEGREE_DIFF_D
#define GET_DEGREE_DIFF_D(to, from) (GET_AZIMUTH_DOUBLE((double)(to) - (double)(from) + 180.0) - 180.0)
#endif


/**
 * �߶��������
 */
 // �߶γ���
#define LINEARDIST(x0, y0, x1, y1) (sqrt((double)((x1)-(x0))*(double)((x1)-(x0))+(double)((y1)-(y0))*(double)((y1)-(y0))))

// �߶η�λ�� Y������
#define CALC_AZIMUTH(fromX, fromY, toX, toY) GET_AZIMUTH(GET_DEGREES(atan2((double)((toX) - (fromX)), (double)((fromY) - (toY)))))


/**
 * Webī����
 */
const double kWebMercRange = 20037508.3427892;
#define MercMeter2Rad(x) ((x) / kWebMercRange * M_PI)	// ī������ ת ī���л���
#define MercRad2Meter(x) ((x) / M_PI * kWebMercRange)	// ī���л��� ת ī������


/**
 * ��λת��
 */
 // ���ȵ�λת��
#define M2DM(x) (ROUND((x) * 10.0))			// �ף������ͣ� ת ���ף����ͣ�
#define DM2M(x) ((x) / 10.0)				// ���ף����ͣ� ת �ף������ͣ�
#define M2CM(x) (ROUND((x) * 100.0))		// �ף������ͣ� ת ���ף����ͣ�
#define CM2M(x) ((x) / 100.0)				// ���ף����ͣ� ת �ף������ͣ�

// �ٶȵ�λת��
// ǧ��/Сʱ => ��/��
#define KMPH2MPS(x)	((x)/3.6)
// ��/�� => ǧ��/Сʱ
#define MPS2KMPH(x)	((x)*3.6)

#define CM_CLAMP(x, L, R)     ((x)<(L)? (L) : (x)>(R)? (R) : (x))
    //-----------------------------------------------------

// an integral type (bool, char, char16_t, char32_t, wchar_t, short, int,
//  long, long long, including any signed, unsigned, and cv-qualified variants)
    template <typename T>
    typename std::enable_if_t<std::is_integral<T>::value, T>
        cm_max(T a, T b)// cm_ is abbre of clientmaps
    {
        return std::max(a, b);
    }

    template <typename T>
    typename std::enable_if_t<std::is_floating_point<T>::value, T>
        cm_max(T a, T b)
    {
        return ::fmax(a, b);
    }

    // an arithmetic type  is an integral type or a floating-point type)
    template <typename T, typename... Ts>
    typename std::enable_if_t<std::is_arithmetic<T>::value, T>
        cm_max(T a, T b, Ts... args)
    {
        return cm_max(a, cm_max(b, args...));
    }

    template <typename T>
    typename std::enable_if_t<std::is_integral<T>::value, T>
        cm_min(T a, T b)
    {
        return std::min(a, b);
    }
    // It inherits from integral_constant as being either true_type or 
    // false_type, depending on whether T is a floating point type:
    // fundamental floating point types: float, double,long double
    template <typename T>
    typename std::enable_if_t<std::is_floating_point<T>::value, T>
        cm_min(T a, T b)
    {
        return ::fmin(a, b);
    }

    template <typename T, typename... Ts>
    typename std::enable_if_t<std::is_arithmetic<T>::value, T>
        cm_min(T a, T b, Ts... args)
    {
        return cm_min(a, cm_min(b, args...));
    }


    //-----------------------------------------------------
    template<typename T>
    T cm_clamp(T value, T min_, T max_)
    {
        return cm_max(min_, cm_min(max_, value));
    }

    //------------------------------------------------------

// Constrains n to the given range (including min, excluding max) via modular
// arithmetic.// ���������� �淶��
    template <typename T>
    T cm_wrap(T value, T min, T max)
    {
        if (value >= min && value < max)
        {
            return value;
        }
        else if (value == max) {
            return min;
        }

        const T delta = max - min;
        const T wrapped = min + std::fmod(value - min, delta);
        return value < min ? wrapped + delta : wrapped;
    }
 


    //------------------------------------------------------
    // Computes the log2(x) rounded up to the next integer.
    // (== number of bits required to store x)
    uint32_t ceil_log2(uint64_t x);

    template <typename T>
    T log2(T x)
    {
        // log2() is producing wrong results on ARMv5 binaries
        // running on ARMv7+ CPUs.
#if defined(__ANDROID__)
        return ::log(x) / M_LN2;
#else
        return ::log2(x);
#endif
    }
    //-----------------------------------------------------
    //#include <md/util/geometry.hpp> 
 
    // Find the angle of the two vectors, solving the formula for the cross product
    // a x b = |a||b|sin(��) for ��.
    template <typename T = double, typename S>
    T cm_angleBetween(const SPoint2D<S>& a, const SPoint2D<S>& b)
    {
        return std::atan2((a.x * b.y - a.y * b.x), a.x * b.x + a.y * b.y);
    }

    template <typename T = double, typename S>
    T cm_angleTo(const SPoint2D<S>& a, const SPoint2D<S>& b)
    {
        return std::atan2(a.y - b.y, a.x - b.x);
    }

    // Reflect an angle around 0 degrees
    template <typename T>
    std::array<T, 2> cm_flip(const std::array<T, 2>& c)
    {
        return { {
            static_cast<T>(2 * M_PI - c[0]),
            static_cast<T>(2 * M_PI - c[1])
        } };
    }

    template <typename T, typename S1, typename S2>
    SPoint2D<T> cm_normal(const S1& a, const S2& b)
    {
        T dx = b.x - a.x;
        T dy = b.y - a.y;
        T c = std::sqrt(dx * dx + dy * dy);
        return { dx / c, dy / c };
    }

    template <typename T>
    T cm_perp(const T& a)
    {
        return T(-a.y, a.x);
    }

    template <typename T, typename S1, typename S2>
    T cm_dist(const S1& a, const S2& b)
    {
        T dx = b.x - a.x;
        T dy = b.y - a.y;
        T c = std::sqrt(dx * dx + dy * dy);
        return c;
    }

    template <typename T, typename S1, typename S2>
    T cm_distSqr(const S1& a, const S2& b)
    {
        T dx = b.x - a.x;
        T dy = b.y - a.y;
        T c = dx * dx + dy * dy;
        return c;
    }

    template <typename T>
    T cm_round(const T& a)
    {
        return T(::round(a.x), ::round(a.y));
    }

    template <typename T>
    T cm_length(T a, T b)
    {
        return std::sqrt(a * a + b * b);
    }

    // Take the magnitude of vector a.
    template <typename T = double, typename S>
    T cm_mag(const S& a)
    {
        return std::sqrt(a.x * a.x + a.y * a.y);
    }

    template <typename T = double, typename S>
    S cm_unit(const S& a)
    {
        auto magnitude = cm_mag<T>(a);
        if (magnitude == 0)
        {
            return a;
        }
        return a * (1 / magnitude);
    }

    template <typename T, typename S = double>
    T cm_rotate(const T& a, S angle)
    {
        S cos = std::cos(angle);
        S sin = std::sin(angle);
        S x = cos * a.x - sin * a.y;
        S y = sin * a.x + cos * a.y;
        return T(x, y);
    }

    template <typename T>
    SPoint2D<T> cm_matrixMultiply(const std::array<T, 4>& m,
        const SPoint2D<T>& p)
    {
        return SPoint2D<T>(m[0] * p.x + m[1] * p.y, m[2] * p.x + m[3] * p.y);
    }

    template <typename T>
    T cm_smoothstep(T edge0, T edge1, T x)
    {
        T t = cm_clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
        return t * t * (T(3) - T(2) * t);
    }

    template <typename T>
    inline T cm_division(const T dividend, const T divisor, const T nan)
    {
        if (divisor == 0)
        {
            if (dividend == 0)
            {
                return nan;
            }
            else {
                return ::copysign(std::numeric_limits<T>::infinity(), dividend);
            }
        }
        else {
            return dividend / divisor;
        }
    }


} // namespace MdU


// log2 is not available on Android before API 18.
#if defined(__ANDROID__) && defined(__GNUC__) && \
    defined(__ANDROID_API__) && __ANDROID_API__ < 18

template <typename T>
typename std::enable_if_t<std::is_floating_point<T>::value, T>
log2(T x)
{
    return ::log(x) / M_LN2;
}

#endif
