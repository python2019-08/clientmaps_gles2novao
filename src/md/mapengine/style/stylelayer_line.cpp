// clang-format off

#include <md/util/cpp_misc.hpp>
  
 
#include "md/drawengine/drawcontext.h"
#include <md/mapengine/style/style_value.hpp>
#include <md/mapengine/style/stylelayer_line.hpp> 

namespace clientmap {
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
LineStyle::LineStyle(const std::string& aLayerId,
    const std::string& aDsLayerId)
    : StyleLayer(aLayerId, aDsLayerId)
{
    mType = StyleLayerType::line;
}
 
LineStyle::~LineStyle() = default;
 
/*
{

    "visibility": "visible",
    "filter": [
        "all",
        [ "==", "structure", "tunnel" ],
        [ "==", "class", "motorway_link" ]
    ],
    "type": "line",
    "source": "mapbox",
    "id": "tunnel_motorway_link",
    "paint": { 
        "line-color": "#fc8", 
        "line-dasharray": [ 0.5, 0.25 ],
        "line-width": {
            "base": 1.2,
            "stops": [
                [ 12.5, 0 ],
                [ 13, 1.5 ],
                [ 14, 2.5 ],
                [ 20, 11.5]
            ]
        }
    },
    "source-layer": "road"
},
*/
int
LineStyle::parse(const JSValue& aVal)
{
    int ret = 0;
    if (!JsonTool::isObject(&aVal)) {
        std::string err = "layer must be an object";
        return -3;
    }        


    auto lcValue = JsonTool::objectMember(&aVal, "line-color");
    if (lcValue)
    {
        TStyleExpColor  clr;
        ret = StyleConverter<TStyleExpColor>{}(*lcValue, clr);
        if (ret >= 0)
        {
            _color = std::move(clr);
        }
    }
     

    auto dasharrValue = JsonTool::objectMember(&aVal, "line-dasharray");
    if (dasharrValue)
    {
        TStyleStepFs  dasharr;
        ret = StyleConverter<TStyleStepFs>{}(*dasharrValue, dasharr);
        if (ret >= 0)
        {
            _dasharray = std::move(dasharr);
        }
    }

    auto lWValue = JsonTool::objectMember(&aVal, "line-width");
    if (lWValue)
    {
        TStyleExpF  w;
        ret = StyleConverter<TStyleExpF>{}(*lWValue, w);
        if (ret >= 0)
        {
            _width = std::move(w);
        }
    }

    return ret;
} 

LineStyle::ShEvaluated  LineStyle::evaluate(float aZ)
{
	ShEvaluated ret = std::make_shared<TEvaluated>();
	ret->_color = this->_color.evaluate(aZ); 
	ret->_dasharray = this->_dasharray.evaluate(aZ);
	ret->_width = this->_width.evaluate(aZ);

	return ret;
}
 
} // namespace clientmap

// clang-format on
