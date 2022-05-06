#ifndef _MAPSTU_MAP_ENGINE_H__
#define _MAPSTU_MAP_ENGINE_H__
#include <cstdint>
#include <string>
#include <memory>
#include "md/drawengine/drawengine.h"

namespace clientmap {
class  CDrawEngine;

class StyleLayer;
class Style4Map;
using ShStyle4Map = std::shared_ptr<Style4Map >;

class BgSceneLayer; 
class DebugSceneLayer;

class VectTile;
class VecTileSource;
class RasterTileSource;

struct STMsg;
class  CMsgQWorker;

class CMapEngine :public DrawSceneBase
{
public:
    CMapEngine( CDrawEngine& aDE);
    ~CMapEngine();

    bool init( );
    void done();

    void setStyle(const std::string&  aStyleName);
    void setStyleString(const std::string&  aStyleString);
    ShStyle4Map getStyle();
	//-----------------------------
	// virtual functions of DrawSceneBase 
	//----------------------------- 
    bool onBeginDrawFrame(const GlPainters& aPainter,
        const CPaintContext& aPC)override;
    bool onBeginDrawSelectionFrame(const GlPainters& aPainter,
        const CPaintContext& aPC)override;
    bool onDrawFrame(const GlPainters& aPainter,
        const CPaintContext& aPC)override;
    bool onEndDrawFrame(const GlPainters& aPainter,
        const CPaintContext& aPC)override;
private:  
	//-----------------------------
	//---- msg handling functions    
	//-----------------------------
	void _onViewChange(const STMsg& aEvt);
	void _onDataRecv(const STMsg& aEvt);
private: 
    //  ³õÊ¼²ÎÊý
    CDrawEngine&  m_DE;
	 
    // 
    std::string m_curStyleName;
    // 
	std::mutex    m_mapStyleCS;
    ShStyle4Map   m_mapStyle;

    std::unique_ptr<BgSceneLayer >      mBgSceneLayer;
    std::unique_ptr<DebugSceneLayer >   mDbgSceneLayer;
	//--------------- 
	//std::unique_ptr<MapMsgHandler >     mMsgHandler;
	std::shared_ptr<CMsgQWorker>   m_msgQ;

	std::shared_ptr<VecTileSource >    m_vectTS;
	std::shared_ptr<RasterTileSource > m_rastTS;

	std::set<TileID>    m_curTiles;
	float               m_curZoom = 14;
	 

};

}//namespace clientmap 
#endif// _MAPSTU_MAP_ENGINE_H__