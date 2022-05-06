#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <md/util/rapidjson.hpp> 
#include <md/util/constants.hpp> 

#include <md/util/stops_def.hpp> 
 
namespace clientmap {

//////////////////////////////////////////////////////////////////////////////
class GlPainters;
class CPaintContext;
class VectTile;
class Marker;

enum class StyleLayerType : int8_t {
    //------StyleType from tangram£º
    //none,
    //debug,
    //point,
    //polygon,
    //polyline,
    //raster,
    //text,
    //---------------
    unknown = -1,
    background = 0,
    raster,
    face,
    building,
    line,
    symbol,
    LayerTypeCnt
};

class StyleLayer 
{
public:
    StyleLayer(const StyleLayer& ) = delete;
    StyleLayer& operator=(const StyleLayer&) = delete;

    StyleLayer(const std::string& _id, const std::string& aDsLayerId);
    virtual ~StyleLayer();
      
    ////////////////////////////////////
    const std::string&   getID() const;
    const StyleLayerType getType() const;
    const std::string&   getDsLayerID() const;
 
    // Visibility
    bool isVisible() const;
    void setVisibility(bool _b);

    // Zoom range
    float getMinZoom() const;
    float getMaxZoom() const;
    void setMinZoom(float);
    void setMaxZoom(float);

    // parse DrawRule
    virtual  int parse(const JSValue& value) =0; 
 
    // In this function,StyleLayerType has a unique name.
    static  const std::string& typeName(StyleLayerType aTypeIndex);
protected:
    /* StyleLayer example:
    {
    	"id": "landcover_wood",
		"type": "fill", 
		"ds-mainlayer": "landcover",
		"ds-layerid": "landcover_wood",		
		"maxzoom": 14,
		"filter": ["==", "class", "wood"],
		"paint": { }
     }
     "id" :"ds-layerid" -->  n:1  
    */
    /* Unique name for a style instance */
    std::string    mId;  
    StyleLayerType mType; // type
    std::string mDsLayerId; //ds-layerid
    std::string mComments;// description

    float mMinZoom = 0;
    float mMaxZoom = MAX_ZOOM;
    bool  mIsVisible = true;
};
using ShStyleLayer  = std::shared_ptr<StyleLayer >;
using UnqStyleLayer = std::unique_ptr<StyleLayer >;
} //  namespace clientmap
 