#pragma once
#include <cstdint>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept> 
   
#include <glm/glm.hpp>
#include <md/util/tile_id.hpp>
#include <md/util/constants.hpp> 
#include <md/util/geo.hpp> 

#include "md/mapengine/scenelayer/scenelayer_obj_def.hpp"

#include "md/mapengine/tile/vec_tile.hpp"
#include "md/mapengine/style/style4map.hpp"
 
 
namespace clientmap {

//===================================================
// named LayoutVTLayer once,used to Layout  ShVecTileLayer, 
// and to store the result.
// Every kind of VecTileLayer has a corresponding kind of SceneLayer.
//===================================================
class SceneLayer {
public:
    SceneLayer(const std::string& aName, 
        VecTileLayerType aType,
        const glm::dvec4&  aOriginPt);
    virtual ~SceneLayer();

    void  updateStyle(std::shared_ptr<void> aEvaledStyle)
    {
        m_evaledStyle = aEvaledStyle;
    }

    virtual int layout(ShVecTileLayer aVtLayer ) = 0;

    virtual bool draw(
        const GlPainters&    aPainter,
        const CPaintContext& aDC)
    {
        return false;
    }
protected:
    // calculate the scale from tileZoom to styleZoom(e.g. layoutZoom).
    double _calcScaleFromTileZToStyleZ();
    // aDrawZoom is cam.getZoom(). 
    double _calcScaleFromStyleZToDrawZ(double aDrawZoom);
    //
	virtual bool needDraw(const CPaintContext& aPC);



    std::string      _id; // layer name
    VecTileLayerType _type = VecTileLayerType::Polygon;// layer type 

    //  _originPt.xy is a 2d bpx coordinate
    //  _originPt.z is TileID.z, e.g. zoomLevel
    //  _originPt.w is layoutZoom(i.e. styleZoom)
	glm::dvec4       _originPt; 

    // 
    std::shared_ptr<void>    m_evaledStyle;
};
using LayoutVTLayer = SceneLayer;
using ShLayoutVTLayer = std::shared_ptr<SceneLayer>;

//////////////////////////////////////////////////////////////////////////////
class StyleLayer; 
class GlPainters;
class CPaintContext; 
    
 
} //  namespace clientmap
 