// clang-format off
#pragma once  
#include <md/mapengine/scenelayer/scenelayer.hpp> 

namespace clientmap {
class TileID;
typedef std::vector<TileID>  TileIdArray;

class DebugSceneLayer final : public SceneLayer {
public:
    DebugSceneLayer(const std::string& aName ="DebugSceneLayer",  
        const glm::dvec4&  aOriginPt = {});
    ~DebugSceneLayer() override;      
     
    int layout(ShVecTileLayer aVtLayer )override 
    {
        return 0;
    }

    bool draw(
        const GlPainters& aPainter,
        const CPaintContext& aDC )override;
    

protected: 
    bool _drawTileBorders(const GlPainters& aPainter,
        const CPaintContext& aDC );  

    bool _drawMapStatus(const GlPainters& aPainter,
        const CPaintContext& aDC);
    void _test(const GlPainters& aPainter,
        const CPaintContext& aDC);

    void _drawLines(const GlPainters& aPainter,
        const CPaintContext& aDC);

    void _drawBuildings(const GlPainters& aPainter,
        const CPaintContext& aDC);

};
 
} // namespace mapboxmap

// clang-format on
