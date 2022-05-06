#include <md/util/logtool.hpp>
#include <md/util/cpp_misc.hpp> 

#include <md/drawengine/transform_state.hpp>
#include "md/drawengine/drawcontext.h"

#include <md/mapengine/scenelayer/scenelayer.hpp>
/*

#include <md/mapengine/scenelayer/scenelayer_bg.hpp>
#include <md/mapengine/scenelayer/scenelayer_tile.hpp>
#include <md/mapengine/scenelayer/scenelayer_overlay.hpp>

*/
 
using namespace clientmap;
 
SceneLayer::SceneLayer(const std::string& aName, 
    VecTileLayerType   aType,
    const glm::dvec4&  aOriginPt)
    : _id(aName), _type(aType),_originPt(aOriginPt)
{

}

SceneLayer::~SceneLayer() = default;
  
double SceneLayer::_calcScaleFromTileZToStyleZ()
{
    //-------
    double tileZoom  = _originPt.z;//tz
    double styleZoom = _originPt.w;//sz
    double diffScale = glm::pow(2, styleZoom - tileZoom);
    double scaleToStyleZ = MercPrj::tileWIDTH / ((double)tileEXTENT) * diffScale;

    return scaleToStyleZ;
}

double SceneLayer::_calcScaleFromStyleZToDrawZ(double aDrawZoom)
{
    // aDrawZoom is cam.getZoom() 
    // styleZoom is _originPt.w;
    double scale = std::pow(2.0, aDrawZoom - _originPt.w);
    return scale;
}


bool SceneLayer::needDraw(const CPaintContext& aPC) 
{
	auto&  camera = const_cast<TransformState&>(aPC._transState);
    // styleZoom is _originPt.w;
	auto diff = glm::abs(_originPt.w - camera.getZoom());

	return  (camera.getZoom() <= 16.0 && diff < 2)||
            (camera.getZoom() >  16.0);
}