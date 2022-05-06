#include <assert.h>
#include "md/util/cpp_misc.hpp"  
#include "md/util/math_tools.hpp" 
#include "md/util/merc_prj.hpp"
#include "md/util/msg_que_worker.h"


#include "md/drawengine/glpainters/glpainters.h" 
#include "md/drawengine/drawcontext.h"   
#include "md/drawengine/transform_state.hpp"
#include "md/drawengine/drawengine.h"
#include "md/mapengine/marker/marker.hpp" 

#include "md/mapengine/tile/vec_tile.hpp"
#include "md/mapengine/tile/vec_tile_source.hpp" 
#include "md/mapengine/tile/raster_tile_source.hpp"
#include "md/mapengine/tile/tile_db_worker.h"
#include  "md/mapengine/tile/response.hpp"
#include "md/mapengine/style/style4map.hpp"  
 
#include <md/mapengine/scenelayer/scenelayer_bg.hpp> 
#include <md/mapengine/scenelayer/scenelayer_debug.hpp>
#include <md/mapengine/scenelayer/scenelayer_overlay.hpp>
 
#include "md/md_global.h" 

#include "md/mapengine/mapengine.h" 

namespace clientmap {
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
CMapEngine::CMapEngine(CDrawEngine& aDE)
    :m_DE(aDE)
{
	m_msgQ = std::make_shared<CMsgQWorker>(1024);

	m_msgQ->regMsgFn((uint32_t)EMviewEvent::kViewChange,
		std::bind(&CMapEngine::_onViewChange, this, std::placeholders::_1)
	);
	m_msgQ->regMsgFn((uint32_t)EMviewEvent::kDataRcv,
		std::bind(&CMapEngine::_onDataRecv, this, std::placeholders::_1)
	);

	//-------------
	m_vectTS = std::make_shared<VecTileSource>();
	m_rastTS = std::make_shared<RasterTileSource>();
	CTileDbWorker::inst().setMsgQWeakPtr(m_msgQ);

    //--create BgSceneLayer 
    mBgSceneLayer  = std::make_unique<BgSceneLayer>();
    mDbgSceneLayer = std::make_unique<DebugSceneLayer>();
}

CMapEngine::~CMapEngine()
{
    done();
}
 

bool CMapEngine::init( )
{
    bool ret = true;  

    return ret;
}

void CMapEngine::done()
{
 
}
 
void CMapEngine::setStyle(const std::string&  aStyleName)
{ 
    m_curStyleName = aStyleName;
    //
    std::string stylePath = CMdGlobal::unique.m_assetPath + "/" + aStyleName;
    ShStyle4Map pMStyle = Style4Map::loadLocalStyle(stylePath);
    if (pMStyle)
    {
		std::unique_lock<std::mutex> lock(m_mapStyleCS);
		m_mapStyle = pMStyle;
		// 
        mBgSceneLayer->setStyle(m_mapStyle->_bgLayer); 


    } 
}

void CMapEngine::setStyleString(const std::string&  aStyleRawData)
{
	ShStyle4Map pMStyle = Style4Map::loadStr(aStyleRawData);
	if (pMStyle)
	{
		std::unique_lock<std::mutex> lock(m_mapStyleCS);
		m_mapStyle = pMStyle;
		// 
		mBgSceneLayer->setStyle(m_mapStyle->_bgLayer);
	}
}

ShStyle4Map CMapEngine::getStyle()
{
    ShStyle4Map pMStyle;
    {
        std::unique_lock<std::mutex> lock(m_mapStyleCS);
        pMStyle = m_mapStyle;
    }
    return pMStyle;
}

bool CMapEngine::onBeginDrawFrame(const GlPainters& aPainter,
    const CPaintContext& aPC)
{ 
    auto& cam = const_cast<TransformState&>(aPC._transState);
    
    int64_t curMS = clientmap::currentMillisec();

    //mapController.lock.lock();
    //this->getGpsView().update(curMS);
    //mapController.update(curMS); 
    //mapController.lock.unlock();

    std::shared_ptr<TileIdArray> tidArr = std::make_shared<TileIdArray>();
    cam.getCoveredTiles(*tidArr);

	m_msgQ->postMsg((uint32_t)EMviewEvent::kViewChange,
        tidArr, 0, cam.getZoom(), clientmap::currentMillisec());


    return true;
}

bool CMapEngine::onBeginDrawSelectionFrame(const GlPainters& aPainter,
    const CPaintContext& aPC)
{
    return false;
}

bool CMapEngine::onDrawFrame(const GlPainters& aPainter,
    const CPaintContext& aPC)
{      
    mBgSceneLayer->draw(aPainter, aPC); 

    // other styleLayers draw...
    bool ret = false; 
	auto& cam = const_cast<TransformState&>(aPC._transState);
	GlPainters& painter = const_cast<GlPainters&>(aPainter);

	std::shared_ptr<Style4Map>  mapStyles;
	{
		std::unique_lock<std::mutex> lock(m_mapStyleCS);
		mapStyles = m_mapStyle;
	}

	if (mapStyles)
	{
		if (mapStyles->_rasterLayer) {

		}

		for (auto& style : mapStyles->_vectLayers)
		{
			if (!style->isVisible() ) {
				continue;
			}

            if (cam.getZoom() < style->getMinZoom() ||
                cam.getZoom() > style->getMaxZoom())
            {
                continue;
            }

            // for debug,start
            //if (style->getID() != "road-service-link-track-case")
            //{
            //    continue;
            //}
            // for debug,end

			m_vectTS->drawStyleLayer(aPainter, aPC, style);
		}
	}

    mDbgSceneLayer->draw(aPainter, aPC);
     
    return ret;
}

/* Perform any unsetup needed after drawing each frame */
bool CMapEngine::onEndDrawFrame(const GlPainters& aPainter, 
                             const CPaintContext& aPC)
{
    return true;
}

  

}//namespace clientmap 
