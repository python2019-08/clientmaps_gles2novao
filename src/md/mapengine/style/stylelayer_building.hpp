// clang-format off

// This file is generated. Do not edit.

#pragma once
#include "md/util/color.hpp"
#include <md/mapengine/style/stylelayer.hpp> 
 

namespace clientmap {
 
class BuildingStyle final : public StyleLayer {
public:
	class TEvaluated
	{
	public:
		std::string       _sidePattern{};
		ColorI    _sideColor{ 255, 255, 255, 255 };
		float     _sideOpacity{ 1.0 };

		ColorI    _topColor{ 255, 255, 255, 255 };
		float     _topOpacity = 1.0;
#ifdef DRAW_BUILDING_OUTLINE
		ColorI    _topOutlineColor = { 255, 255, 255, 255 };
#endif
	};
	using ShEvaluated = std::shared_ptr<TEvaluated>;


public:
    BuildingStyle(const std::string& aLayerId,
        const std::string& aDsLayerId);
    ~BuildingStyle() override;

    int parse(const JSValue& value)override;
 
    ShEvaluated  evaluate(float aZ);

protected:
	//--------------------------------
	TStyleStepStr  _sidePattern = {};//	 ,

	TStyleExpColor _sideColor = { {255, 255, 255, 255} };// 
	TStyleExpF     _sideOpacity = 1.0;// 

	TStyleExpColor _topColor{ {255, 255, 255, 255} };
	TStyleExpF     _topOpacity = 1.0;
#ifdef DRAW_BUILDING_OUTLINE
	TStyleExpColor _topOutlineColor = { };// not used.
#endif
};
 
} // namespace clientmap

// clang-format on
