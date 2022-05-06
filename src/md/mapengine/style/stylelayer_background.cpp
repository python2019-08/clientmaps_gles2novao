// clang-format off
 
#include <md/util/cpp_misc.hpp>
 
#include <md/mapengine/style/style_value.hpp>   
#include <md/mapengine/style/stylelayer_background.hpp>


namespace clientmap {
 
///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////
BackgroundStyle::BackgroundStyle(const std::string& aLayerId,
    const std::string& aDsLayerId)
    : StyleLayer(aLayerId, aDsLayerId)
{
    mType =  StyleLayerType::background;
}
  
BackgroundStyle::~BackgroundStyle() = default;
   
 
 
int
BackgroundStyle::parse(const JSValue& aVal)
{      
    int ret = 0;
    if (!JsonTool::isObject(&aVal)) {
        std::string err = "layer must be an object";
        return -3;
    }

    auto bgPatternValue = JsonTool::objectMember(&aVal, "background-pattern");
    if (bgPatternValue)
    {
        TStyleStepStr str;
        ret = StyleConverter<TStyleStepStr>{}(*bgPatternValue, str);
        if (ret >= 0)
        {
            _pattern = std::move(str);
        }
    }

    auto bgClrValue = JsonTool::objectMember(&aVal, "background-color");
    if (bgClrValue)
    {
        TStyleExpColor  clr;
        ret = StyleConverter<TStyleExpColor>{}(*bgClrValue, clr);
        if (ret >= 0)
        {
            _color = std::move(clr);
        }
    }

    auto opaValue = JsonTool::objectMember(&aVal, "background-opacity");
    if (opaValue)
    {
        TStyleExpF  opa;
        ret = StyleConverter<TStyleExpF>{}(*opaValue, opa);
        if (ret >= 0)
        {
            _opacity = std::move(opa);
        }
    }

    return ret; 
} 

BackgroundStyle::ShEvaluated  BackgroundStyle::evaluate(float aZ)
{
	ShEvaluated ret = std::make_shared<TEvaluated>();
	ret->_opacity = this->_opacity.evaluate(aZ);
	ret->_color = this->_color.evaluate(aZ);
	ret->_pattern = this->_pattern.evaluate(aZ);;

	return ret;
}
   

} // namespace md

// clang-format on
