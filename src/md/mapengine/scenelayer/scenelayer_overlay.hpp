// clang-format off
#pragma once  
#include <md/mapengine/scenelayer/scenelayer.hpp> 

namespace clientmap {
 
class OverlaySceneLayer final : public SceneLayer {
public:
    OverlaySceneLayer(const std::string& aName,  
		const glm::dvec4&  aOriginPt = {});
    ~OverlaySceneLayer () override;      

    int layout(ShVecTileLayer aVtLayer )override
    {
        return 0;
    }

    bool draw(
        const GlPainters& aPainter,
        const CPaintContext& aDC  )override;
    

protected:
    void _drawSth(const GlPainters& aPainter, const CPaintContext& aDC);
     
};
 
} // namespace mapboxmap

// clang-format on
