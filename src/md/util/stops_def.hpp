#pragma once
#include <stdint.h> 
#include <map>
#include "md/util/color.hpp"


namespace clientmap{ 
template<class T>
class ExponentialStops
{
public:
    using Stops = std::map<float, T>;
        
    Stops stops;
    float base = 1.0f; 
        
    ExponentialStops()
    {
            
    }
        
    ExponentialStops(T aV)
    {
        stops[0] = aV;
    }
        
    ExponentialStops(Stops stops_, float base_ = 1.0f)
    : stops(std::move(stops_))  ,base(base_)
    {
    }
        
    bool isUndefined() const
    {
        return stops.empty();
    }
        
    T  evaluate(float z) const;
         
    friend bool
    operator==(const ExponentialStops &lhs, const ExponentialStops &rhs)
    {
        return lhs.stops == rhs.stops &&
                lhs.base == rhs.base;
    }
};
	 

/*
GNOTYE: Produces discrete, stepped results by evaluating
a piecewise-constant function defined by pairs of input and
output values ("stops"). The input may be any numeric.

Stop inputs must be numeric literals in strictly ascending order.
Returns the output value of the stop just less than the input,
or the first input if the input is less than the first stop.
*/
template<class T>
class IntervalStops
{
public:
	using Stops = std::map<float, T>;
	Stops stops;

	// todolist MNOTE:
	//TransitionOptions  _options;

	//  IntervalStops() = default;
	IntervalStops()
	{
		//printf("\n");
	}

	IntervalStops(T aV)
	{
		stops[0] = aV;
	}

	IntervalStops(Stops stops_)
		: stops(std::move(stops_))
	{
	}

	bool  isUndefined() const
	{
		return stops.empty();
	}

    T  evaluate(float z) const;

	friend bool
		operator==(const IntervalStops &lhs, const IntervalStops &rhs)
	{
		return lhs.stops == rhs.stops;
	}
};

 
typedef ExponentialStops<float>     TStyleExpF;
typedef ExponentialStops<ColorI>    TStyleExpColor;
typedef ExponentialStops< std::array<float, 2> > TStyleExp2F;
typedef ExponentialStops< std::array<float, 4> > TStyleExp4F;
typedef ExponentialStops< std::array<double, 3>  > TStyleExp3D;


// MNOTE: Uninterpolated types
typedef IntervalStops<float>        TStyleStepF;
typedef IntervalStops<std::string>  TStyleStepStr;
typedef IntervalStops<std::vector<std::string> > TStyleStepStrs;
typedef IntervalStops<std::vector<float> > TStyleStepFs;
typedef IntervalStops<bool>       TStyleStepB;
typedef IntervalStops<uint8_t>    TStyleStepE; // enum
//
//typedef IntervalStops<style::LineCapType>  TStyleStepLineCap;// enum            
//typedef IntervalStops<style::LineJoinType> TStyleStepLineJoin;// enum
//typedef IntervalStops<style::AlignmentType> TStyleStepAlign;// enum
//typedef IntervalStops< style::AlignmentType> TStyleStepAlignment;// enum
//typedef IntervalStops< style::IconTextFitType> TStyleStepIconTextFit;// enum
//typedef IntervalStops< style::SymbolPlacementType> TStyleStepSymbolPlacement;//enum
//typedef IntervalStops< style::SymbolZOrderType> TStyleStepSymbolZOrder;//enum
//typedef IntervalStops< style::TextJustifyType> TStyleStepTextJustify;//enum
//typedef IntervalStops< style::TextTransformType> TStyleStepTextTransform;//enum
//typedef IntervalStops< std::vector<style::TextVariableAnchorType> >
//                TStyleStepTextVariableAnchors;//enum
//typedef IntervalStops< std::vector<style::TextWritingModeType> >
//                TStyleStepTextWritingModes;//enum


} // namespace clientmap
