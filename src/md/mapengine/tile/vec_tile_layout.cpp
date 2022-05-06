
#include <md/util/math_tools.hpp>
#include <md/util/merc_prj.hpp>

#include <md/drawengine/transform_state.hpp>
#include "md/drawengine/drawcontext.h"

#include "md/mapengine/style/style4map.hpp"   
#include <md/mapengine/scenelayer/scenelayer_face.hpp>
#include <md/mapengine/scenelayer/scenelayer_building.hpp>
#include <md/mapengine/scenelayer/scenelayer_line.hpp>

#include <md/mapengine/tile/vec_tile_layout.hpp>
 

using namespace clientmap;
 
//=================================================

ShLayoutVT  LayoutVecTile::doLayout(ShVectTile  aVT,
	float        aLayoutZoom,
	ShStyle4Map  aMapStyle)
{
	ShLayoutVT  pLayoutVT = nullptr;

	do
	{
		int ret = 0;
		if (!aVT ||  aVT->m_Id2layer.empty() ||
            !aMapStyle || aMapStyle->_vectLayers.empty() ||
            aLayoutZoom < 0 || aLayoutZoom > MAX_ZOOM
            )
        {
			ret = -1;
			break;
		}

        TileIDEx  tlIdEx{ aVT->m_tileId .x, aVT->m_tileId.y,
                          aVT->m_tileId.z,  (uint8_t)aLayoutZoom };
		pLayoutVT = std::make_shared<LayoutVecTile>(tlIdEx);
		if (!pLayoutVT) {
			ret = -2;
			break;
		}



        float layoutZ = aLayoutZoom; // int part of aLayoutZoom
        std::modf(aLayoutZoom, &layoutZ);
		for (auto& styleLayer : aMapStyle->_vectLayers)
		{
            // whether need layout this styleLayer?
            if (!styleLayer->isVisible()||
                aLayoutZoom < styleLayer->getMinZoom() ||
                aLayoutZoom > styleLayer->getMaxZoom()
                ) 
            {
                continue;
            }

            // get the vtLayer from aVT,and create corresponding 
            // pLVTLayer if not exists.
            auto& styleLayerId = styleLayer->getID();
            auto&    dsLayerId = styleLayer->getDsLayerID();
            if (aVT->m_Id2layer.count(dsLayerId) == 0)
            {
                continue;
            } 
			ShVecTileLayer     vtLayer = aVT->m_Id2layer[dsLayerId];
			ShLayoutVTLayer  pLVTLayer = getLayoutTileLayer(pLayoutVT,
                styleLayerId, vtLayer->_type );
            if (!pLVTLayer) {
                continue;
            }


            // pLVTLayer->updateStyle
			if (vtLayer->_type == FeatureType::Polygon)
			{ 
                auto fcStyle = std::dynamic_pointer_cast<FaceStyle>(styleLayer);
                pLVTLayer->updateStyle( fcStyle->evaluate(layoutZ) );
			}
			else if (vtLayer->_type == FeatureType::LineString)
			{ 
                auto lnStyle = std::dynamic_pointer_cast<LineStyle>(styleLayer);                 
                pLVTLayer->updateStyle(lnStyle->evaluate(layoutZ));                 
			}
            else if (vtLayer->_type == FeatureType::Building)
            {
                auto bdStyle = std::dynamic_pointer_cast<BuildingStyle>(styleLayer);
                pLVTLayer->updateStyle(bdStyle->evaluate(layoutZ));
            }
			else if (vtLayer->_type == FeatureType::Point)
			{
                assert(0);
			}


            // pLVTLayer->layout( )
            pLVTLayer->layout(vtLayer );
		}


	} while (false);

	return pLayoutVT;
}
 

ShLayoutVTLayer 
LayoutVecTile::getLayoutTileLayer(ShLayoutVT  aLVT,
	const std::string& aLayerId, 
	VecTileLayerType   aType )
{
    // South-West corner of the tile in 2D 
	Pt2D tileOri = MercPrj::tileOriginInBpx( aLVT->tileIdEx().tileId() );
    glm::dvec4  tileOrigin{ tileOri.x,  tileOri.y, 
              (double)aLVT->tileIdEx().z,
              (double)aLVT->tileIdEx().w };


    ShLayoutVTLayer  pLVTLayer = nullptr;
	if (aLVT->m_Id2layer.count(aLayerId) == 0)
	{        
        if (aType == VecTileLayerType::Polygon)
        { 
            pLVTLayer = std::make_shared<LayoutVT_FaceLayer>(aLayerId, tileOrigin);
        }
        if (aType == VecTileLayerType::Building)
        {
            pLVTLayer = std::make_shared<LayoutVT_BuildingLayer>(aLayerId,tileOrigin);
        }
        else if (aType == VecTileLayerType::LineString)
        {
            pLVTLayer = std::make_shared<LayoutVT_lineLayer>(aLayerId,tileOrigin);
        }
        else if (aType == VecTileLayerType::Point)
        {
            assert(0);
        }

        if(pLVTLayer){ 
		    aLVT->m_Id2layer[aLayerId] = pLVTLayer;
        }
	}
 
	return pLVTLayer;
}

LayoutVecTile::LayoutVecTile(const TileIDEx& aID)
    :m_layoutTileId(aID)
{

}

LayoutVecTile::~LayoutVecTile() = default;


bool LayoutVecTile::needDraw(const CPaintContext& aPC)
{
    auto&  camera = const_cast<TransformState&>(aPC._transState);
    auto diff = glm::abs(m_layoutTileId.w - camera.getZoom());

    return  (camera.getZoom() <= 16.0 && diff < 2) ||
        (camera.getZoom() > 16.0);
}

int LayoutVecTile::draw(
    const GlPainters& aPainter,
    const CPaintContext& aDC,
    const std::string& aStyleLayerId)
{
    int ret = 0;
    do
    {
        if (!needDraw(aDC))
        {
            ret = 1;// not draw
            break;
        }



        if (m_Id2layer.count(aStyleLayerId) ==0){
            ret = 1;// not draw
            break;
        }
        ShLayoutVTLayer  pLVTLayer= m_Id2layer.at(aStyleLayerId);
        if (!pLVTLayer) {
            break;
        }


        pLVTLayer->draw(aPainter, aDC);

    } while (false);

    return ret;
}