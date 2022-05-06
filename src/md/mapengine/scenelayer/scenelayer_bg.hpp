// clang-format off
#pragma once  
#include <md/mapengine/scenelayer/scenelayer.hpp> 

namespace clientmap {
class TileID;
typedef std::vector<TileID>  TileIdArray;

class BgSceneLayer final : public SceneLayer {
public:
    BgSceneLayer(const std::string& aName= "bgScene", 
        const glm::dvec4&  aOriginPt = {});
    ~BgSceneLayer() override;      

    void setStyle(ShStyleLayer aBgStyle)
    {
        mBgStyle = aBgStyle;
    }

    int layout(ShVecTileLayer aVtLayer )override 
    {
        return 0;
    }

    bool draw( const GlPainters& aPainter, 
        const CPaintContext& aDC )override;
    

protected:
    void _drawGrid(const GlPainters& aPainter, const CPaintContext& aDC);
     

    ShStyleLayer mBgStyle;
};
 
} // namespace mapboxmap

// clang-format on
