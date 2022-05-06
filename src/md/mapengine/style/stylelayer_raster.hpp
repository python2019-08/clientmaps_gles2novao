// clang-format off
#pragma once 
#include <md/util/color.hpp>
#include <md/mapengine/style/stylelayer.hpp> 

namespace clientmap {
 
class RasterStyle final : public StyleLayer {
public:
	class TEvaluated
	{
	public:
		float   _opacity = 1.0;
		float   _fadeDuration{ 300 };
	};
	using ShEvaluated = std::shared_ptr<TEvaluated>;

public:
    RasterStyle(const std::string& aLayerId,
        const std::string& aDsLayerId);
    ~RasterStyle() override;

    int parse(const JSValue& value)override;

    ShEvaluated  evaluate(float aZ);

protected: 
	//--------------------------------
	// raster-fade-duration
	// raster-opacity 
	TStyleExpF _opacity = 1.0;
	TStyleExpF _fadeDuration{ 300 };
};
using ShRasterStyleLayer = std::shared_ptr<RasterStyle >;
using UniqRasterStyleLayer = std::unique_ptr<RasterStyle >;
} // namespace mapboxmap

// clang-format on
