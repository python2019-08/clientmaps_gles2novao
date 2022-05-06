// clang-format off
#include <mapbox/earcut.hpp> // polygon triangulation (step 1/3)

#include <md/util/cpp_misc.hpp> 
#include "md/util/merc_prj.hpp"
#include "md/util/logtool.hpp"
#include <md/mapengine/style/style_value.hpp>  

#include "md/drawengine/glpainters/glpainters.h" 
#include "md/drawengine/drawcontext.h"
#include <md/mapengine/tile/vec_tile.hpp> 

#include <md/drawengine/transform_state.hpp>
#include <md/mapengine/style/stylelayer_raster.hpp>

#include <md/mapengine/scenelayer/scenelayer_face.hpp>

using namespace clientmap;

//================================================= 
//--------------------------
// polygon triangulation (step 2/3)for earcut 
namespace mapbox {
namespace util {

template <>
struct nth<0, InTilePt> {
    inline static auto get(const InTilePt &t) {
        return t.x;
    };
};
template <>
struct nth<1, InTilePt> {
    inline static auto get(const InTilePt &t) {
        return t.y;
    };
};

} // namespace util
} // namespace mapbox


 
///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////
FaceSceneLayer ::FaceSceneLayer(const std::string& aName,  
	const glm::dvec4&  aOriginPt)
    : SceneLayer(aName, VecTileLayerType::Polygon, aOriginPt)
{

}
  
FaceSceneLayer ::~FaceSceneLayer () = default;    


//--------------------------
int FaceSceneLayer::layout(ShVecTileLayer aVtLayer )
{
    int ret = 0;

    do
    {
        if (!aVtLayer || 
			aVtLayer->_type != VecTileLayerType::Polygon)
		{
            ret = -1;
            break;
        }


        const uint16_t  cUint16Max = std::numeric_limits<uint16_t>::max();
        for (auto& ft : aVtLayer->_features)
        {
            for (auto& poly : ft->_polygons)
            {
                //--Optimize polygons with many interior rings for 
                //earcut tesselation.
                // limitHoles(poly, 500);
                bool bPolyTooManyPoints = false;
                std::size_t totalVertices = 0;
                for (const auto& ring : poly)
                {
                    totalVertices += ring.size();
                    if (totalVertices > cUint16Max) {
                        bPolyTooManyPoints = true;
                        break;
                    }
                }
                if (bPolyTooManyPoints) {
                    continue;// abandon this polygon
                }

                std::size_t startVertices = m_vertices.size();

                for (const auto& ring : poly)
                {
                    std::size_t nVert = ring.size();
                    if (nVert == 0)
                        continue;

                    if (m_lineParts.empty() ||
                        m_lineParts.back()._vertexLen + nVert > cUint16Max)
                    {
                        m_lineParts.emplace_back(
                            m_vertices.size(), // vert offset                           
                            m_lines.size()     // index offset
                        );
                    }

                    auto& linePart = m_lineParts.back();
                    assert(linePart._vertexLen <= cUint16Max);
                    uint16_t lineIdx = linePart._vertexLen;

                    m_vertices.emplace_back(ring[0].x, ring[0].y);
                    m_lines.emplace_back(lineIdx + nVert - 1, lineIdx);
                    for (std::size_t i = 1; i < nVert; i++)
                    {
                        m_vertices.emplace_back(ring[i].x, ring[i].y);
                        m_lines.emplace_back(lineIdx + i - 1, lineIdx + i);
                    }

                    linePart._vertexLen += nVert;
                    linePart._indexLen += nVert * 2;
                }

                std::vector<uint32_t> indices = ::mapbox::earcut(poly);

                std::size_t nIndicies = indices.size();
                assert(nIndicies % 3 == 0);

                if (m_triParts.empty() ||
                    m_triParts.back()._vertexLen + totalVertices > cUint16Max)
                {
                    m_triParts.emplace_back(startVertices, m_triangles.size());
                }

                auto& triPart = m_triParts.back();//trianglePart
                assert(triPart._vertexLen <= cUint16Max);
                uint16_t triangleIndex = triPart._vertexLen;

                for (std::size_t i = 0; i < nIndicies; i += 3)
                {
                    m_triangles.emplace_back(
                        triangleIndex + indices[i],
                        triangleIndex + indices[i + 1],
                        triangleIndex + indices[i + 2]);
                }

                triPart._vertexLen += totalVertices;
                triPart._indexLen += nIndicies;
            }
        }
    } while (false);

    return ret;
}
void   FaceSceneLayer::debugInf2Log()
{
    auto   style = std::static_pointer_cast<FaceStyle::TEvaluated>(m_evaledStyle);
    Log::Debug("FaceSceneLayer =%s , clr(%d,%d,%d,%d)\n",
        this->_id.c_str(),  
        style->_color.r, style->_color.g, style->_color.b, style->_color.a);

}
bool FaceSceneLayer::draw(const GlPainters& aPainter,
    const CPaintContext& aPC 
    )
{   
    bool ret = false;
	//return ret;
	if (!m_evaledStyle)
	{
		return ret;
	}
     

	GlPainters& painter = const_cast<GlPainters&>(aPainter);
    auto&       cam     = const_cast<TransformState&>(aPC._transState);
    auto pStyle = std::static_pointer_cast<FaceStyle::TEvaluated>(m_evaledStyle);
	if (pStyle->_color.a == 0 ) {
		return ret;
	}
	if (!needDraw(aPC)) {
		return ret;
	}
    debugInf2Log();

    ColorI  finalClr = pStyle->_color; 
	Pt2D  originPt{ _originPt.x, _originPt.y };// Base coordinate

	double  tbw = MercPrj::tileBpxWidth(_originPt.z); 
	double scale = tbw / tileEXTENT;


	int partCnt = (int)m_triParts.size();
	for (int i = 0; i < partCnt; i++)
	{
		LayoutPart& curPart = m_triParts[i];

		if (curPart._vertexOffset >= m_vertices.size() ||
			curPart._indexOffset >= m_triangles.size() ||
			(curPart._vertexOffset+ curPart._vertexLen > m_vertices.size() )||
			(curPart._indexOffset + curPart._indexLen  > m_triangles.size()*3)
			) 
		{
			continue; // if curPart is not valid,skip it
		}
		 

		painter.faceDrawPolygon(aPC, finalClr, originPt, scale,
			(float*)&(m_vertices[curPart._vertexOffset]),
			(int16_t*)&(m_triangles[curPart._indexOffset]), 
			curPart._indexLen); //curPart._vertexLen;		  
	}
     
    return ret;
}
  

// clang-format on
