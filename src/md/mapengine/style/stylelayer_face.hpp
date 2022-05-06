// clang-format off

// This file is generated. Do not edit.

#pragma once
#include "md/util/color.hpp"
#include <md/mapengine/style/stylelayer.hpp> 
 

namespace clientmap {
 
class FaceStyle final : public StyleLayer {

public:
	//MNOTE: nested class definition
	class TEvaluated
	{
	public:
		bool              _antialias{ true };
		std::string       _pattern{};

		ColorI            _color{ 0, 0, 0, 255 }; 
		ColorI            _outlineColor{ };

		std::array<float, 2>   _offset = { {0, 0} };
	};
	using ShEvaluated = std::shared_ptr<TEvaluated>;


public:
    FaceStyle(const std::string& aLayerId,
        const std::string& aDsLayerId);
    ~FaceStyle() override;

    int parse(const JSValue& value)override;

	
	TEvaluated&  defaultVal();
	ShEvaluated  evaluate(float aZ);      
protected:
	//--------------------------------
	TStyleStepB    _antialias = { true };//  FillAntialias,
	TStyleStepStr  _pattern = {};//	FillPattern,

	TStyleExpColor _color = { {0, 0, 0, 255} };// FillColor, 
	TStyleExpColor _outlineColor = { };// FillOutlineColor

	TStyleExp2F    _offset = { { {0, 0} } };
};
 
} // namespace clientmap

// clang-format on
