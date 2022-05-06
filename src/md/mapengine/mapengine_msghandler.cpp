// clang-format off
 
#include <md/util/cpp_misc.hpp>  
//
 
// 
#include <md/util/math_tools.hpp>
#include <md/util/merc_prj.hpp>
#include "md/util/msg_que_worker.h"

#include "md/drawengine/glpainters/glpainters.h" 
#include "md/drawengine/drawcontext.h"   
#include <md/drawengine/transform_state.hpp>
#include "md/drawengine/drawengine.h"

#include "md/mapengine/tile/vec_tile.hpp"
#include "md/mapengine/tile/vec_tile_source.hpp" 
#include "md/mapengine/tile/raster_tile_source.hpp"
#include "md/mapengine/tile/tile_db_worker.h"
#include  "md/mapengine/tile/response.hpp"
#include "md/mapengine/style/style4map.hpp"   

#include "md/mapengine/mapengine.h" 



namespace clientmap {
 
void CMapEngine::_onViewChange(const STMsg& aEvt) {
	if (!aEvt._ptrParam) {
		return;
	}

	// convert tileid array to tileid set
	auto newTiles = std::static_pointer_cast<std::vector<TileID>>(aEvt._ptrParam);
	m_curTiles.clear();
	for (auto &tile : *newTiles) {
		m_curTiles.insert(tile);
	}
	// 
	m_curZoom = aEvt._fParam;
	// get mapStyle
	std::shared_ptr<Style4Map>  mapStyle;
	{
		std::unique_lock<std::mutex> lock(m_mapStyleCS);
		mapStyle = m_mapStyle;
	}

	// apply to every tileSource
	bool needDraw = false;
	needDraw = m_rastTS->onViewChange(m_curTiles, m_curZoom);
	needDraw = m_vectTS->onViewChange(m_curTiles, m_curZoom,
		mapStyle) || needDraw;
	if (needDraw) {
		m_DE.requestDrawing();
	}
}

void CMapEngine::_onDataRecv(const STMsg& aEvt)
{
	do
	{
		// convert to CTileResponse type variable "resp"
		auto resp = std::static_pointer_cast<CTileResponse>(aEvt._ptrParam);
		if (!resp) {
			return;
		}

		//   
		ETileType tileType = (ETileType)resp->_TTId.w;
		
		// get shared_ptr  mapStyle
		std::shared_ptr<Style4Map>  mapStyle;
		{
			std::unique_lock<std::mutex> lock(m_mapStyleCS);
			mapStyle = m_mapStyle;
		}

		// processing "resp->_data"
		if (tileType == ETileType::eVector) { 
			if (mapStyle) {
				m_vectTS->onDataRecv(resp->_data, mapStyle);
			}			
		}
		else if (tileType == ETileType::eRaster)
		{
			//m_rastTS->onDataRecv();
		}

		resp->_data.reset();

	} while (false);


	m_DE.requestDrawing();
}



} // namespace md

// clang-format on
