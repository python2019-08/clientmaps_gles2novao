// clang-format off
#pragma once  
#include <md/mapengine/scenelayer/scenelayer.hpp> 

namespace clientmap {
  
class FaceSceneLayer final : public SceneLayer {// Polygon Layer
public:
    FaceSceneLayer(const std::string& aName, 
		const glm::dvec4&  aOriginPt);
    ~FaceSceneLayer() override;

    int layout(ShVecTileLayer aVtLayer ) override;

    bool draw(
        const GlPainters& aPainter,
        const CPaintContext& aDC  )override;
private:
    void   debugInf2Log();
    // polygon is layouted(into GL_TRIANGLES triangulation and 
    // GL_LINES outline) by LayoutVT_FaceLayer::layout().
    //
    // use LayoutPart to avoid transferring more than 65535 vertices to GL.
    std::vector<glm::vec2>        m_vertices;

    std::vector<GLTrianglesIndex> m_triangles;// triangles  Index  Array
    std::vector<LayoutPart>       m_triParts;

    std::vector<GLLinesIndex> m_lines;//  lines Index  Array
    std::vector<LayoutPart>   m_lineParts; //  
     
};

using LayoutVT_FaceLayer = FaceSceneLayer;
 
} // namespace mapboxmap

// clang-format on
