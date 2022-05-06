// clang-format off

#include <md/util/cpp_misc.hpp> 
 
#include "md/drawengine/drawcontext.h"
#include <md/mapengine/style/style_value.hpp>

#include <md/mapengine/style/stylelayer_face.hpp>

namespace clientmap { 
///////////////////////////////////////////////////
FaceStyle::FaceStyle(const std::string& aLayerId,
    const std::string& aDsLayerId)
    : StyleLayer(aLayerId, aDsLayerId)
{
    mType = StyleLayerType::face;
}
 
FaceStyle::~FaceStyle() = default;
 
/*
{
    "id": "water",
    "type": "face",
    "is-visible": true,
    "source": "composite",
    "source-layer": "water",
    "paint": {
        "fill-color": {
            "base": 1,
            "stops": [
                [15, "200 , 23 , 82"],
                [16, "230 , 37 , 84"]
            ]
        }, 
        "fill-outline-color": "35,6,79",
         
        "fill-pattern": "wave",
        "fill-offset": {
            "base": 1.2,
            "stops": [
                [7, [0, 0]],
                [16, [-1, -1]]
            ]
        },
        "fill-offset-anchor": "viewport"

    }
},
*/

int
FaceStyle::parse(const JSValue& aVal)
{
    int ret = 0;
    if (!JsonTool::isObject(&aVal)) {
        std::string err = "layer must be an object";
        return -3;
    }

    auto patternValue = JsonTool::objectMember(&aVal, "fill-pattern");
    if (patternValue)
    {
        TStyleStepStr  pattern;
        ret = StyleConverter<TStyleStepStr>{}(*patternValue, pattern);
        if (ret >= 0)
        {
            _pattern = std::move(pattern);
        }
    }

    auto fcValue = JsonTool::objectMember(&aVal, "fill-color");
    if (fcValue)
    {
        TStyleExpColor  clr;
        ret = StyleConverter<TStyleExpColor>{}(*fcValue, clr);
        if (ret >= 0)
        {
            _color = std::move(clr);
        }
    }
     

    auto outlineColorValue = JsonTool::objectMember(&aVal, "fill-outline-color");
    if (outlineColorValue)
    {
        TStyleExpColor  clr;
        ret = StyleConverter<TStyleExpColor>{}(*outlineColorValue, clr);
        if (ret >= 0)
        {
            _outlineColor = std::move(clr);
        }
    }
      

    auto offsetValue = JsonTool::objectMember(&aVal, "fill-offset");
    if (offsetValue)
    {
        TStyleExp2F  offset;
        ret = StyleConverter<TStyleExp2F>{}(*offsetValue, offset);
        if (ret >= 0)
        {
            _offset = std::move(offset);
        }
    }

    return ret;
}

FaceStyle::TEvaluated&   FaceStyle::defaultVal()
{
	static std::unique_ptr<TEvaluated> ret = nullptr; // static unique_ptr
	if (!ret)
	{
		ret = std::make_unique<TEvaluated>();

		ret->_antialias = { true };
		ret->_pattern = {};

		ret->_color = { 0, 0, 0, 255 }; 
		ret->_outlineColor = { };

		ret->_offset = { {0, 0} };
	}

	return (*ret);
}

FaceStyle::ShEvaluated  FaceStyle::evaluate(float aZ)
{
	ShEvaluated ret = std::make_shared<TEvaluated>();
	if (ret)
	{
		ret->_antialias = this->_antialias.evaluate(aZ);
		ret->_pattern = this->_pattern.evaluate(aZ);

		ret->_color = this->_color.evaluate(aZ); 
		ret->_outlineColor = this->_outlineColor.evaluate(aZ);

		ret->_offset = this->_offset.evaluate(aZ);
	}

	return ret;
}
 
} // namespace clientmap

// clang-format on
