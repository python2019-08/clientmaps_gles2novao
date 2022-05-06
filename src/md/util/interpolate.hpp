#pragma once

 
#include <md/util/color.hpp> 
#include <md/util/unitbezier.hpp>
 
#include <array>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>

 
namespace clientmap {

float interpolationFactor(float base, 
                          float aRangeMin, float aRangeMax, 
                          float z);
 
class CubicBezierInterpolator
{
public:
	CubicBezierInterpolator(double x1_, double y1_,
		double x2_, double y2_)
		: ub(x1_, y1_, x2_, y2_)
	{

	}

	double getCubicBezierFactor(double aRangeMin, double aRangeMax,
		const double input) const
	{
        float  f = clientmap::interpolationFactor(1.0,
            static_cast<float>(aRangeMin),
            static_cast<float>(aRangeMax),
            input);

		return ub.solve(f, 1e-6);
	}

	bool operator==(const CubicBezierInterpolator &rhs) const
	{
		return ub == rhs.ub;
	}

	 UnitBezier ub;
};
 
template <class T, class Enabled = void>
struct Interpolator;

template <typename T>
T interpolate(const T& a, const T& b, const double t)
{
    return Interpolator<T>()(a, b, t);
}


template <class T, class Enabled>
struct Interpolator 
{
    T operator()(const T& a, const T& b, const double t) const 
	{
        return a * (1.0 - t) + b * t;
    }
};
 

template <class T, std::size_t N>
struct Interpolator< std::array<T, N> > 
{
private:
    using Array = std::array<T, N>;

    template <std::size_t... I>
    Array operator()(const Array& a, 
	                 const Array& b, 
					 const double t, 
					 std::index_sequence<I...>) 
	{
        return {{ interpolate(a[I], b[I], t)... }};
    }

public:
    Array operator()(const Array& a, const Array& b, const double t) 
	{
        return operator()(a, b, t, std::make_index_sequence<N>());
    }
};

 
template<class T>
struct Interpolator<std::vector<T>,
    std::enable_if_t<std::is_floating_point<T>::value>
    >
{
    std::vector<T> operator()(const std::vector<T>& a,
                              const std::vector<T>& b,
                              const double          t) const
    {
        assert(a.size() == b.size());
        if (a.empty())
        {
            return {};
        }

        std::vector<T> result;
        for (std::size_t i = 0; i < a.size(); i++)
        {
            T item = interpolate<T>( a[i], b[i], t);
            result.push_back(item);
        }
        return result;
    }
};
 
 
 
template <>
struct Interpolator<ColorI> 
{
public:
    ColorI operator()(const ColorI& a0,
        const ColorI& b0,
        const double t
        )
	{
        ColorF a = a0.toColorF();
        ColorF b = b0.toColorF();
        ColorF c{
            interpolate(a.r, b.r, t),
            interpolate(a.g, b.g, t),
            interpolate(a.b, b.b, t),
            interpolate(a.a, b.a, t)
        };
        return c.toColor();
    }
};

 
#if 0
template <>
struct Interpolator<StyleRotation> 
{
public:
    StyleRotation operator()(const StyleRotation& a, 
        const StyleRotation& b, 
        const double t) 
    {
        assert(a.period() == b.period());
        auto period = a.period();
        auto aAngle = std::fmod(a.getAngle(), period);
        auto bAngle = std::fmod(b.getAngle(), period);

        if (aAngle - bAngle > period * 0.5) {
            return {std::fmod(aAngle * (1.0 - t) + (bAngle + period) * t, period)};
        }

        if (aAngle - bAngle < period * -0.5) {
            return {std::fmod((aAngle + period) * (1.0 - t) + bAngle * t, period)};
        }

        return {aAngle * (1.0 - t) + bAngle * t};
    }
};
#endif

struct Uninterpolated 
{
    template <class T>
    T operator()(const T& a, const T&, const double) const 
	{
        return a;
    }
};

template <>
struct Interpolator<bool> : Uninterpolated 
{
};

template <class T>
struct Interpolator<T, 
                   typename std::enable_if_t<std::is_enum<T>::value>>
    : Uninterpolated 
	{
	};

template <>
struct Interpolator<std::string>
    : Uninterpolated 
{
};

template <class T>
struct Interpolator<std::vector<T> >
    : Uninterpolated 
{

};

template <class T>
struct Interpolatable
    : std::conditional_t<
      !std::is_base_of<Uninterpolated, Interpolator<T>>::value,
      std::true_type,
      std::false_type> 
{

};

  
} // namespace MdU
 
