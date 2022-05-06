// clang-format off
#pragma once 
#include <md/util/color.hpp>
#include <md/mapengine/style/stylelayer.hpp> 

namespace clientmap { 

class BackgroundStyle final : public StyleLayer {
public:
	class TEvaluated
	{
	public:
		float        _opacity = 1.0;
		ColorI       _color{ 167,217,252,255 };
		std::string  _pattern{};
	};
	using ShEvaluated = std::shared_ptr<TEvaluated>;

public:
    BackgroundStyle(const std::string& aLayerId,
        const std::string& aDsLayerId);
    ~BackgroundStyle() override;

    int parse(const JSValue& value)override;

    ShEvaluated  evaluate(float aZ);
protected: 
	//--------------------------------
	// BackgroundOpacity
	// BackgroundColor
	// TStyleStepStr BackgroundPattern
	TStyleExpF     _opacity = 1.0;
	TStyleExpColor _color{ { 167,217,252,255 } };// sea blue
	TStyleStepStr  _pattern{};
};
using ShBgStyleLayer = std::shared_ptr<BackgroundStyle >;
using UniqBgStyleLayer = std::unique_ptr<BackgroundStyle >;
} // namespace mapboxmap

// clang-format on
