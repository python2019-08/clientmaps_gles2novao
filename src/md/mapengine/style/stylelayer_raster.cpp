// clang-format off 
#include <md/util/cpp_misc.hpp>
 
#include <md/mapengine/style/style_value.hpp>  
 
#include "md/drawengine/glpainters/glpaint_background.h"
#include "md/drawengine/glpainters/glpainters.h" 
#include "md/drawengine/drawcontext.h"
#include <md/mapengine/tile/vec_tile.hpp>


#include <md/mapengine/style/stylelayer_raster.hpp>


using namespace clientmap; 
///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////
RasterStyle::RasterStyle(const std::string& aLayerId,
    const std::string& aDsLayerId)
    : StyleLayer(aLayerId, aDsLayerId)
{
    mType = StyleLayerType::raster;
}
  
RasterStyle::~RasterStyle() = default;
 

int RasterStyle::parse(const JSValue& aVal)
{  
    int ret = 0;
    if (!JsonTool::isObject(&aVal)) {
        std::string err = "layer must be an object";
        return -3;
    }

    auto durValue = JsonTool::objectMember(&aVal, "raster-fade-duration");
    if (durValue)
    {
        TStyleExpF  dur;
        ret = StyleConverter<TStyleExpF>{}(*durValue, dur);
        if (ret >= 0)
        {
            _fadeDuration = std::move(dur);
        }
    }

    auto caseOpaValue = JsonTool::objectMember(&aVal, "raster-opacity");
    if (caseOpaValue)
    {
        TStyleExpF  opa;
        ret = StyleConverter<TStyleExpF>{}(*caseOpaValue, opa);
        if (ret >= 0)
        {
            _opacity = std::move(opa);
        }
    }
   
    return -4;
}  

RasterStyle::ShEvaluated  RasterStyle::evaluate(float aZ)
{
	ShEvaluated ret = std::make_shared<TEvaluated>();
	ret->_opacity = this->_opacity.evaluate(aZ);
	ret->_fadeDuration = this->_fadeDuration.evaluate(aZ);
	return ret;
}

// clang-format on
