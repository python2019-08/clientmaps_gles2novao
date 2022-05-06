// clang-format off

#include <md/util/cpp_misc.hpp> 
 
#include "md/drawengine/drawcontext.h"
#include <md/mapengine/style/style_value.hpp>

#include <md/mapengine/style/stylelayer_building.hpp>

namespace clientmap {
///////////////////////////////////////////////////
 

///////////////////////////////////////////////////
BuildingStyle::BuildingStyle(const std::string& aLayerId,
    const std::string& aDsLayerId)
    : StyleLayer(aLayerId, aDsLayerId)
{
    mType =  StyleLayerType::building;
}
 
BuildingStyle::~BuildingStyle() = default;
 /*
{
    "id": "building",
    "type": "building",
    "source": "mapbox",
    "source-layer": "building",
    "paint": {
        "side-pattern":"building-side.png",
        "side-color": {
            "base": 1,
            "stops": [
                [ 15.5, "#f2eae2" ],
                [ 16, "#dfdbd7" ]
            ]
        },
        "side-opacity": {
            "base": 1,
            "stops": [
                [ 15, 0 ],
                [ 16, 1 ]
            ]
        },


        "top-color": "#f2eae2",
        "top-opacity": {
            "base": 1,
            "stops": [
                [ 15, 0 ],
                [ 16, 1 ]
            ]
        },
        "top-outline-color": "#dfdbd7"
    }
},
 */
int
BuildingStyle::parse(const JSValue& aVal)
{
    int ret = 0;
    if (!JsonTool::isObject(&aVal)) {
        std::string err = "layer must be an object";
        return -3;
    }

    auto sidePatternValue = JsonTool::objectMember(&aVal, "side-pattern");
    if (sidePatternValue)
    {
        TStyleStepStr str;
        ret = StyleConverter<TStyleStepStr>{}(*sidePatternValue, str);
        if (ret >= 0)
        {
            _sidePattern = std::move(str);
        }
    }

    auto sideClrValue = JsonTool::objectMember(&aVal, "side-color");
    if (sideClrValue)
    {
        TStyleExpColor  sideClr;
        ret = StyleConverter<TStyleExpColor>{}(*sideClrValue, sideClr);
        if (ret >= 0)
        {
            _sideColor = std::move(sideClr);
        }
    }

    auto sideOpValue = JsonTool::objectMember(&aVal, "side-opacity");
    if (sideOpValue)
    {
        TStyleExpF  sideOpa;
        ret = StyleConverter<TStyleExpF>{}(*sideOpValue, sideOpa);
        if (ret >= 0)
        {
            _sideOpacity = std::move(sideOpa);
        }
    }

    auto topColorValue = JsonTool::objectMember(&aVal, "top-color");
    if (topColorValue)
    {
        TStyleExpColor  clr;
        ret = StyleConverter<TStyleExpColor>{}(*topColorValue, clr);
        if (ret >= 0)
        {
            _topColor = std::move(clr);
        }
    }

    auto topOpaValue = JsonTool::objectMember(&aVal, "top-opacity");
    if (topOpaValue)
    {
        TStyleExpF  opa;
        ret = StyleConverter<TStyleExpF>{}(*topOpaValue, opa);
        if (ret >= 0)
        {
            _topOpacity = std::move(opa);
        }
    }
#ifdef DRAW_BUILDING_OUTLINE
    auto topOutlineClrValue = JsonTool::objectMember(&aVal, "top-outline-color");
    if (topOutlineClrValue)
    {
        TStyleExpColor  clr;
        ret = StyleConverter<TStyleExpColor>{}(*topOutlineClrValue, clr);
        if (ret >= 0)
        {
            _topOutlineColor = std::move(clr);
        }
    }
#endif

    return ret;
}

BuildingStyle::ShEvaluated  BuildingStyle::evaluate(float aZ)
{
	ShEvaluated ret = std::make_shared<TEvaluated>();
	ret->_sidePattern = this->_sidePattern.evaluate(aZ);

	ret->_sideColor = this->_sideColor.evaluate(aZ);
	ret->_sideOpacity = this->_sideOpacity.evaluate(aZ);

	ret->_topColor = this->_topColor.evaluate(aZ);
	ret->_topOpacity = this->_topOpacity.evaluate(aZ);
#ifdef DRAW_BUILDING_OUTLINE
	ret->_topOutlineColor = this->_topOutlineColor.evaluate(aZ);
#endif

	return ret;
} 
} // namespace clientmap

// clang-format on
