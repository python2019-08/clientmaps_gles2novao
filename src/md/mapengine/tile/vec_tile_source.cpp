#include <glm/glm.hpp>
#include <md/util/math_tools.hpp>
#include <md/util/merc_prj.hpp>
#include <md/util/logtool.hpp>
 
#include <md/mapengine/tile/vec_tile.hpp>
#include <md/mapengine/tile/vec_tile_layout.hpp>
#include "md/mapengine/tile/tile_db_worker.h"

#include <md/drawengine/transform_state.hpp>
#include "md/drawengine/glpainters/glpainters.h" 
#include "md/drawengine/drawcontext.h"    
#include "md/drawengine/drawengine.h"

#include <md/mapengine/tile/vec_tile_source.hpp>
 

namespace clientmap {

VecTileSource::VecTileSource( )
{ 
}

VecTileSource::~VecTileSource() = default;

uint8_t zoomToLayoutZ(double aZoom)
{
    //uint8_t layoutZ = static_cast<uint8_t>(aZoom * 10.0);
    //layoutZ &= ~(0x3);
    //layoutZ = glm::clamp<uint8_t>(layoutZ, 0, MAX_ZOOM_I*10);

    uint8_t layoutZ = static_cast<uint8_t>(aZoom); 
    layoutZ = glm::clamp<uint8_t>(layoutZ, 0, MAX_ZOOM_I);
    return  layoutZ;
}

bool VecTileSource::onViewChange(const TileIdSet & aCurTiles,
    double aZoom , 
    std::shared_ptr<Style4Map> aMapStyle
    )
{ 
	mCurZoom = aZoom;
    TypedTileIdSet requestingTiles;
    for (auto &tlId : aCurTiles) 
    { 
        if (auto vt = m_dataCache.get(tlId))
        {
            uint8_t   layoutZ = zoomToLayoutZ(aZoom);             
            TileIDEx  layoutId( tlId.x , tlId.y, tlId.z , layoutZ);
            if (m_layoutCache.get(layoutId)) {
                continue;
            }
            else { 
				auto pLvt= LayoutVecTile::doLayout(vt, aZoom, aMapStyle);
				if (pLvt) {
					m_layoutCache.put(layoutId, pLvt);
				}
                continue;
            }
        } 

        TypedTileId  tt_Id{ tlId.x,tlId.y,tlId.z, 
                           (uint8_t)(ETileType::eVector) };
        if (requestingTiles.count(tt_Id)) {
            continue;
        }
        requestingTiles.insert(tt_Id);
    }
 
    CTileDbWorker::inst().queryTiles(requestingTiles, 0);
 

    return _updateDrawTile(aCurTiles, aZoom);
} 

void VecTileSource::onDataRecv(ShVoid aTileData,
	std::shared_ptr<Style4Map> aMapStyle
)
{
    ShVectTile vt = std::static_pointer_cast<VectTile>(aTileData);
 
    auto &tlId = vt->m_tileId;     
    m_dataCache.put(tlId, vt);

	// 
	uint8_t   layoutZ = zoomToLayoutZ(mCurZoom);
	TileIDEx  layoutId(tlId.x, tlId.y, tlId.z, layoutZ);
    if (!m_layoutCache.get(layoutId)) 
	{  // 
		auto pLvt = LayoutVecTile::doLayout(vt, mCurZoom, aMapStyle);
		if (pLvt) {
			m_layoutCache.put(layoutId, pLvt);
		}
    } 
    return;
}

bool VecTileSource::twoDrawTileArray_isDiff(std::shared_ptr<ShLayoutVTArray> aL,
	std::shared_ptr<ShLayoutVTArray> aR)
{
	bool ret = false;
	do
	{
		if (!aL && !aR) {
			ret = false;
			break;
		}

		if (!aL || !aR) {
			ret = true;
			break;
		}

		if (aL->size() != aR->size()) {
			ret = true;
			break;
		}

		uint32_t cnt = aR->size();
		for (uint32_t i = 0; i < cnt; i++)
		{
			if (aL->at(i)->tileIdEx() != aR->at(i)->tileIdEx())
			{
				ret = true;
				break;
			}
		}
		ret = false;

	} while (false);
	return ret;
}


bool VecTileSource::_updateDrawTile(const TileIdSet& aCurTiles, double aZoom)
{
    bool ret = false;
 
    auto newDrawTiles = std::make_shared<ShLayoutVTArray>( );
     
    for (auto &tlId : aCurTiles)
    {
        uint8_t   layoutZ = zoomToLayoutZ(aZoom);
        TileIDEx  layoutId(tlId.x, tlId.y, tlId.z, layoutZ);
        if (auto rr = m_layoutCache.get(layoutId) ) {
			newDrawTiles->push_back(rr);
        } 
    }    

    if (newDrawTiles->empty()) {
        return false;
    }

    m_swapCS.lock();
    auto oldDrawTiles = m_layoutToDraw;
    m_swapCS.unlock();

    ret = twoDrawTileArray_isDiff(oldDrawTiles, newDrawTiles);
    if (ret) {
        std::unique_lock<std::mutex> lock(m_swapCS);
        m_layoutToDraw = newDrawTiles;
    } 

    return ret;
}

bool VecTileSource::drawStyleLayer(
	const GlPainters& aPainter,
	const CPaintContext& aDC,
	ShStyleLayer aStyleLayer)
{
	bool ret = true;
    auto& cam = const_cast<TransformState&>(aDC._transState);

	std::shared_ptr<ShLayoutVTArray> drawableTiles;
	{
		std::unique_lock<std::mutex> lock(m_swapCS);
		drawableTiles =m_layoutToDraw;
	}

	if (drawableTiles) 
	{
        
        Pt3D mapCt= cam.getMapCenter();
        const std::string& styleLayerID = aStyleLayer->getID();
        //if (styleLayerID != "building")
        //{
        //    Log::Debug("styleLayerID ==  building\n");
        //    return false;// gm-debug
        //}

		for (size_t i = 0; i < drawableTiles->size(); i++)
		{
			auto& pLVT = drawableTiles->at(i);
            //----debug code,start
			//if (pLVT->tileIdEx().z != 3 ||
			//	pLVT->tileIdEx().x != 6 ||
			//	pLVT->tileIdEx().y != 5  
			//	)
			//{
			//	continue;
			//}
        #if 0
            TileID tlId = pLVT->tileIdEx().tileId();
            Box2D tileBBox = MercPrj::tileBoundsInBpx(tlId);
            if (!tileBBox.contains(mapCt.x, mapCt.y))
            {
                continue;
            }
        #endif
            //----debug code,end
            pLVT->draw(aPainter, aDC, styleLayerID );
		}
         
	}

	return ret;
}

} // namespace clientmap
