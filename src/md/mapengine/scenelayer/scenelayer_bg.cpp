// clang-format off
 
#include <md/util/cpp_misc.hpp> 
#include <md/mapengine/style/style_value.hpp>  

#include "md/drawengine/glpainters/glpainters.h" 
#include "md/drawengine/drawcontext.h"
#include <md/mapengine/tile/vec_tile.hpp> 

#include <md/drawengine/transform_state.hpp>
#include <md/mapengine/style/stylelayer_background.hpp>

#include <md/mapengine/scenelayer/scenelayer_bg.hpp>


namespace clientmap {
 
///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////
BgSceneLayer::BgSceneLayer(const std::string& aName,  
    const glm::dvec4&  aOriginPt)
    : SceneLayer(aName, FeatureType::Unknown, aOriginPt)
{

}
  
BgSceneLayer::~BgSceneLayer() = default;    

bool BgSceneLayer::draw(const GlPainters& aPainter,
    const CPaintContext& aDC 
    )
{  
    bool ret = false;
    auto& cam= const_cast<TransformState&>(aDC._transState);
      
    GlPainters& painter = const_cast<GlPainters&>(aPainter);
    
    auto bgStyle = std::dynamic_pointer_cast<BackgroundStyle>(mBgStyle);
    auto evalStyle = bgStyle->evaluate(cam.getZoom() );
    ColorI bgC{ 167,217,252,255 };// sea blue 
    if (evalStyle || evalStyle->_color.a != 0) {
        bgC= evalStyle->_color;
    } 
    painter.bgDrawClear(aDC, bgC);


#if 0
    _drawGrid( painter, aDC);
#endif

    return ret;
}
  
void BgSceneLayer::_drawGrid(
    const GlPainters& aPainter,
    const CPaintContext& aDC )
{
    GlPainters& painter=const_cast<GlPainters&>(aPainter);
    painter.bgDrawGrid(aDC, {0,33,233,255});     
}

} // namespace md

// clang-format on
