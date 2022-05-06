// clang-format off
#pragma once  
#include <md/mapengine/scenelayer/scenelayer.hpp> 

namespace clientmap { 

class LineSceneLayer final : public SceneLayer {
public:
    LineSceneLayer(const std::string& aName,  
		const glm::dvec4&  aOriginPt);
    ~LineSceneLayer() override;
    
    int layout(ShVecTileLayer aVtLayer ) override;

    bool draw(
        const GlPainters& aPainter,
        const CPaintContext& aDC  )override;
private:
    void   debugInf2Log();
    double _calcLineHalfWidth();
    double _calcDashArrayLen(); 
    

    int _calcLineHeadCap(glm::dvec2* p0,
        glm::dvec2& p1, // start point
        glm::dvec2& p2, // end   point
        double  aLineR,//line radius ,namely line Thickness
        glm::vec2  aLineCap[10],
        glm::vec2  aLineCapTexSTs[10]
    );
    int _calcLineTailCap(glm::dvec2& aP1, // start point
        glm::dvec2& aP2, // end   point
        glm::dvec2* aP3,
        double  aLineR,//line radius ,namely line Thickness
        glm::vec2  aLineCap[10],
        glm::vec2  aLineCapTexSTs[10]
    );
    int _calcLineJoin(glm::dvec2& p0,// previous point
        glm::dvec2& p1, // current point
        glm::dvec2& p2, // next point
        double  aLineR,//line radius ,namely line Thickness
        glm::vec2  aLineJoin[10],
        glm::vec2  aLineJoinTexSTs[10]
    );
    void _extrudeAAPath(std::vector<glm::vec2>&  aPath,
        int      aDrawBegin,
        int      aDrawEnd,
        float    aLineWidth,
        std::vector<glm::vec2>& aTriVertices,
        std::vector<glm::vec2>& aTriAATexSTs
    );

    void _extrudeAAPath_round(std::vector<glm::i16vec2>&  aPath,
        double    aLineHalfWidth,
        TriStrip2DPart& aTriStripVerts);
    void _extrudeAAPath_roundHead(
        std::vector<glm::i16vec2>& aPath,
        double    aLineHalfWidth,
        TriStrip2DPart& aTriStripVerts
    );

	void _extrudeAAPath_butt(std::vector<glm::i16vec2>&  aPath,
        double         aLineHalfWidth,
        double         aDashArrLen,
        TriStrip2DPart& aTriStripVerts);
private:    
    // polyline is layouted(into GL_TRIANGLE_STRIP) by LayoutVT_lineLayer::layout.
    // TriStripPart MUST NOT own more than 65535 vertices.
    std::vector< TriStrip2DPart >  m_triStripParts; //  
};

using LayoutVT_lineLayer = LineSceneLayer;

} // namespace mapboxmap

// clang-format on
