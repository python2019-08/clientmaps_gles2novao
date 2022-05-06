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

#include <md/mapengine/scenelayer/scenelayer_building.hpp>

using namespace clientmap;

//================================================= 
//--------------------------
// polygon triangulation (step 2/3)for earcut 
namespace mapbox {
    namespace util {

        template <>
        struct nth<0, InTilePt> {
            inline static auto get(const InTilePt& t) {
                return t.x;
            };
        };
        template <>
        struct nth<1, InTilePt> {
            inline static auto get(const InTilePt& t) {
                return t.y;
            };
        };

    } // namespace util
} // namespace mapbox
 


 
///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////
BuildingSceneLayer ::BuildingSceneLayer(const std::string& aName,  
	const glm::dvec4&  aOriginPt)
    : SceneLayer(aName, VecTileLayerType::Building, aOriginPt)
{

}
  
BuildingSceneLayer ::~BuildingSceneLayer () = default;    


//--------------------------
int BuildingSceneLayer::layout(ShVecTileLayer aVtLayer )
{
    int ret = 0;

    do
    {
        if (!aVtLayer || 
			aVtLayer->_type != VecTileLayerType::Building)
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


                //----------------------
                // building  roof 
                //----------------------
                layoutRoof(poly, totalVertices, ft->_buildingHeight );
                layoutSide(poly, totalVertices, ft->_buildingHeight);
            }
        }
    } while (false);

    return ret;
}

float BuildingSceneLayer::adjuestBuildingHeight(float aBuildingH,
    float aScaleToStyleZ)
{
    double bdH = aBuildingH;// 3 meters per story
    bdH = bdH <= 0 ? 1 : bdH;
    bdH *= aScaleToStyleZ *50;
    bdH = bdH < 1 ? 4 : bdH;// unit: meter
    double styleZoom = _originPt.w;//sz
    bdH /= MercPrj::metersPerPxAtZoom(styleZoom);

    return bdH;
}
void BuildingSceneLayer::layoutRoof(InTilePolygon& aPoly,
    uint32_t aPolyTotalVertices,
    float aBuildingHeight // building story count??
    )
{
    double scaleToStyleZ = _calcScaleFromTileZToStyleZ();
    const double texWidth = 128;
    const double texHeight = 128;


    double bdH = adjuestBuildingHeight(aBuildingHeight, scaleToStyleZ);
   
    // TODO: meter to px
    std::size_t startVertices = m_roofVertices.size(); 
    for (const auto& ring : aPoly)
    { 
        std::size_t nVert = ring.size();
        if (nVert == 0)
            continue;

        for (std::size_t i = 0; i < nVert; i++)
        {
            m_roofVertices.emplace_back(ring[i].x* scaleToStyleZ,
                ring[i].y* scaleToStyleZ, 
                bdH,
                ring[i].x * scaleToStyleZ / texWidth,
                aBuildingHeight * scaleToStyleZ / texHeight);
        }
    }


    //----------------------
    std::vector<uint32_t> indices = ::mapbox::earcut(aPoly);
    std::size_t nIndicies = indices.size();
    assert(nIndicies % 3 == 0);

    const uint16_t  cUint16Max = std::numeric_limits<uint16_t>::max();
    if (m_roofTriParts.empty() ||
        m_roofTriParts.back()._vertexLen + aPolyTotalVertices > cUint16Max)
    {
        m_roofTriParts.emplace_back(startVertices, m_roofTris.size());
    }
    //----
    auto& triPart = m_roofTriParts.back();//trianglePart
    assert(triPart._vertexLen <= cUint16Max);
    uint16_t triangleIndex = triPart._vertexLen;

    for (std::size_t i = 0; i < nIndicies; i += 3)
    {
        m_roofTris.emplace_back(
            triangleIndex + indices[i],
            triangleIndex + indices[i + 1],
            triangleIndex + indices[i + 2]);
    }

    triPart._vertexLen += aPolyTotalVertices;
    triPart._indexLen += nIndicies;
}


void BuildingSceneLayer::layoutSide(InTilePolygon& aPoly,
    uint32_t aPolyTotalVertices,
    float    aBuildingHeight )
{
    double scaleToStyleZ = _calcScaleFromTileZToStyleZ();
    const uint16_t cUint16Max = std::numeric_limits<uint16_t>::max();
    const double texWidth = 256;
    const double texHeight = 256;

    double bdH = adjuestBuildingHeight(aBuildingHeight, scaleToStyleZ);

    std::size_t totalVertices = aPolyTotalVertices;
    TriStrip3DPart  newPart;
    for (const auto& ring : aPoly)
    { 
        std::size_t nVert = ring.size();
        if (nVert == 0)
            continue;

        for (std::size_t i = 0; i < nVert; i++)
        {
            if (i == 0)
            {
                newPart.emplace_back(ring[i].x * scaleToStyleZ,
                    ring[i].y * scaleToStyleZ,
                    0,
                    ring[i].x * scaleToStyleZ / texWidth,
                    0.0);
            }
            newPart.emplace_back( ring[i].x* scaleToStyleZ,
                ring[i].y * scaleToStyleZ, 
                0,
                ring[i].x * scaleToStyleZ / texWidth,
                0.0);
            newPart.emplace_back(ring[i].x * scaleToStyleZ,
                ring[i].y * scaleToStyleZ, 
                bdH, // aBuildingHeight * scaleToStyleZ,
                ring[i].x * scaleToStyleZ / texWidth,
                aBuildingHeight * scaleToStyleZ / texHeight);
            if (i == nVert - 1) 
            {
                newPart.emplace_back(ring[i].x * scaleToStyleZ,
                    ring[i].y * scaleToStyleZ,
                    bdH, // aBuildingHeight * scaleToStyleZ,
                    ring[i].x * scaleToStyleZ / texWidth,
                    aBuildingHeight * scaleToStyleZ / texHeight);
            }
        }
    }//for (const auto& ring : aPoly)
     
    // to store newPart into m_triStripParts
    if (m_sideParts.empty() ||
        m_sideParts.back().size() + totalVertices * 2 > cUint16Max)
    {
        m_sideParts.emplace_back(newPart);
    }
    else {
        TriStrip3DPart& curPart = m_sideParts.back();
        curPart.insert(curPart.end(), newPart.begin(), newPart.end());
    }
 
}

void   BuildingSceneLayer::debugInf2Log()
{
    auto   style = std::static_pointer_cast<BuildingStyle::TEvaluated>(m_evaledStyle);
    Log::Debug("BuildingSceneLayer =%s ,top-clr(%d,%d,%d,%d)side-clr(%d,%d,%d,%d)\n",
        this->_id.c_str(),  
        style->_topColor.r, style->_topColor.g, 
        style->_topColor.b, style->_topColor.a,
        style->_sideColor.r, style->_sideColor.g,
        style->_sideColor.b, style->_sideColor.a);

}

bool BuildingSceneLayer::drawRoof(
    const GlPainters& aPainter,
    const CPaintContext& aPC)
{
    bool ret = false;

    if (!m_evaledStyle)
    {
        return ret;
    }

    auto pStyle = std::static_pointer_cast<BuildingStyle::TEvaluated>(m_evaledStyle);
    if (pStyle->_topColor.a == 0) {
        return ret;
    }

    GlPainters& painter = const_cast<GlPainters&>(aPainter);
    auto& cam = const_cast<TransformState&>(aPC._transState);
        

    ColorI  finalClr = pStyle->_topColor;
    Pt2D    originPt{ _originPt.x, _originPt.y };// Base coordinate
    double scale = _calcScaleFromStyleZToDrawZ(cam.getZoom());

    int partCnt = (int)m_roofTriParts.size();
    for (int i = 0; i < partCnt; i++)
    {
        LayoutPart& curPart = m_roofTriParts[i];

        if (curPart._vertexOffset >= m_roofVertices.size() ||
            curPart._indexOffset >= m_roofTris.size() ||
            (curPart._vertexOffset + curPart._vertexLen > m_roofVertices.size()) ||
            (curPart._indexOffset + curPart._indexLen > m_roofTris.size() * 3)
            )
        {
            continue; // if curPart is not valid,skip it
        }


        painter.drawBuildingRoof(aPC, finalClr, originPt, scale,
            (float*)&(m_roofVertices[curPart._vertexOffset]),
            (int16_t*)&(m_roofTris[curPart._indexOffset]),
            curPart._indexLen); //curPart._vertexLen;		  
    }

    return ret;
}

bool BuildingSceneLayer::drawSide(
    const GlPainters& aPainter,
    const CPaintContext& aPC)
{
    bool ret = false;

    GlPainters& painter = const_cast<GlPainters&>(aPainter);
    auto& cam = const_cast<TransformState&>(aPC._transState);

    auto pStyle = std::static_pointer_cast<BuildingStyle::TEvaluated>(m_evaledStyle);
    if (pStyle->_sideColor.a == 0) {
        return ret;
    }

    if (!needDraw(aPC)) {
        return ret;
    }

    //----for debug,start
    //if (_id != "road-service-link-track-case")
    //{
    //    return ret;
    //}

    debugInf2Log();
    //----for debug,end 
    Pt2D  originPt{ _originPt.x, _originPt.y };// Base coordinate     
    double scale = _calcScaleFromStyleZToDrawZ(cam.getZoom());
  
    int partCnt = (int)m_sideParts.size();
    for (int i = 0; i < partCnt; i++)
    {
        TriStrip3DPart& curPart = m_sideParts[i];
        if (curPart.size() < 4) {
            continue; // if curPart is not valid,skip it
        }


        painter.drawBuildingSide(aPC, pStyle->_sideColor,
            originPt, scale,
            (float*)curPart.data(), curPart.size());
 
    }
   
    return ret;
}

bool BuildingSceneLayer::draw(const GlPainters& aPainter,
    const CPaintContext& aPC 
    )
{   
    bool ret = false;
 
    do 
    {
        if (!m_evaledStyle)
        {
            break;
        }

        GlPainters& painter = const_cast<GlPainters&>(aPainter);
        auto& cam = const_cast<TransformState&>(aPC._transState);
        if (!needDraw(aPC)) {
            break;
        }
        debugInf2Log();

        ////----draw 2D building, only draw roof
        if (cam.getRotateX() <= 5)
        {
            drawRoof(aPainter, aPC);
            break;
        }
        //----draw 3D
        ret = drawRoof(aPainter, aPC);
 

        ret = drawSide(aPainter, aPC);

    } while (false);
     
    return ret;
}
  

// clang-format on
