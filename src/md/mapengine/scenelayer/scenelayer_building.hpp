// clang-format off
#pragma once  
#include <md/mapengine/scenelayer/scenelayer.hpp> 

namespace clientmap {
  
class BuildingSceneLayer final : public SceneLayer { 
public:
    BuildingSceneLayer(const std::string& aName, 
		const glm::dvec4&  aOriginPt);
    ~BuildingSceneLayer() override;

    int layout(ShVecTileLayer aVtLayer ) override;

    bool draw(
        const GlPainters& aPainter,
        const CPaintContext& aDC  )override;
private:
    float adjuestBuildingHeight( float aBuildingH,
        float aScaleToStyleZ);
    void layoutRoof(InTilePolygon& aPoly,
        uint32_t aPolyTotalVertices,
        float aBuildingHeight);
    void layoutSide(InTilePolygon& aPoly,
        uint32_t aPolyTotalVertices,
        float aBuildingHeight );

    bool drawRoof(
        const GlPainters& aPainter,
        const CPaintContext& aDC);
    bool drawSide(
        const GlPainters& aPainter,
        const CPaintContext& aDC);

    void   debugInf2Log();
    // polygon is layouted(into GL_TRIANGLES triangulation ) by layout().
    //
    //-------building roof--------
    // use LayoutPart to avoid transferring more than 65535 vertices to GL.
    std::vector<XyzUv>            m_roofVertices;
    std::vector<GLTrianglesIndex> m_roofTris;// triangles  Index  Array
    std::vector<LayoutPart>       m_roofTriParts;

    std::vector<TriStrip3DPart>   m_sideParts; 
};

using LayoutVT_BuildingLayer = BuildingSceneLayer;
 
} // namespace mapboxmap

// clang-format on
