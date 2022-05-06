// clang-format off
 
#include <md/util/cpp_misc.hpp> 
#include <md/mapengine/style/style_value.hpp>  
 
#include "md/drawengine/glpainters/glpainters.h" 
#include "md/drawengine/drawcontext.h"
#include <md/mapengine/tile/vec_tile.hpp> 

#include <md/drawengine/transform_state.hpp>
#include <md/mapengine/style/stylelayer_raster.hpp>

#include <md/mapengine/scenelayer/scenelayer_overlay.hpp>


namespace clientmap {
 
///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////
OverlaySceneLayer::OverlaySceneLayer(const std::string& aName,  
	const glm::dvec4&  aOriginPt)
    : SceneLayer(aName, VecTileLayerType::Overlay, aOriginPt)
{

}
  
OverlaySceneLayer ::~OverlaySceneLayer () = default;    

bool OverlaySceneLayer ::draw(const GlPainters& aPainter,
    const CPaintContext& aDC 
    )
{  
    bool ret = false;
    auto& cam= const_cast<TransformState&>(aDC._transState);

    //TRasterDrawRule::TEvaluated  eval;
    //if (mStyle)
    //{
    //     auto styleL = std::static_pointer_cast<RasterStyle>(mStyle);
    //     eval = styleL->evaluate( cam.getZoom() );
    //}

    GlPainters& painter = const_cast<GlPainters&>(aPainter);
     
    return ret;
}
 

void OverlaySceneLayer::_drawSth(
    const GlPainters& aPainter,
    const CPaintContext& aDC )
{
    GlPainters& painter=const_cast<GlPainters&>(aPainter);
    //painter.bgDrawGrid(aDC, {0,33,233,255});     
}

} // namespace md

// clang-format on
