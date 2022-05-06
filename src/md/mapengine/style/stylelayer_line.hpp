// clang-format off
#ifndef STYLE_LAYER_LINE_HPP_INCLUDED
#define STYLE_LAYER_LINE_HPP_INCLUDED

#pragma once
#include <md/mapengine/style/stylelayer.hpp>
 

#include <vector>

namespace clientmap {
   
class LineStyle final : public StyleLayer {

public:
	//MNOTE: nested class definition
	class TEvaluated
	{
	public:
		ColorI _color = { 0, 0, 0, 255 };//LineColor
		std::vector<float>   _dasharray{};// LineDasharray, 
		float  _width{ 1 };//LineWidth

	};
	using ShEvaluated = std::shared_ptr<TEvaluated>;

public:
    LineStyle(const std::string& aLayerId,
        const std::string& aDsLayerId);
    ~LineStyle() override;

    int parse(const JSValue& value)override;

    ShEvaluated  evaluate(float aZ);
     
protected:
	//-------------------------------- 
	TStyleExpColor _color = { { 0, 0, 0, 255} };//line-color 
	TStyleStepFs   _dasharray{};// line-dasharray,
	TStyleExpF     _width = 1.0; //line-width       
}; 
} // namespace clientmap
#endif //!STYLE_LAYER_LINE_HPP_INCLUDED
// clang-format on
