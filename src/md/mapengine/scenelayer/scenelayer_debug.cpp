// clang-format off
 
#include <md/util/cpp_misc.hpp> 
#include "md/util/merc_prj.hpp"
#include "md/util/logtool.hpp"

#include <md/mapengine/style/style_value.hpp>  

#include "md/drawengine/glpainters/glpainters.h" 
#include "md/drawengine/drawcontext.h"
#include <md/mapengine/tile/vec_tile.hpp> 

#include <md/drawengine/transform_state.hpp>
#include <md/mapengine/style/stylelayer_background.hpp>

#include <md/mapengine/scenelayer/scenelayer_debug.hpp>


namespace clientmap {
 
///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////
DebugSceneLayer::DebugSceneLayer(const std::string& aName,  
    const glm::dvec4&  aOriginPt)
    : SceneLayer(aName, VecTileLayerType::Unknown, aOriginPt)
{

}
  
DebugSceneLayer::~DebugSceneLayer() = default;    

bool DebugSceneLayer::draw(const GlPainters& aPainter,
    const CPaintContext& aDC 
    )
{  
    bool  ret = false;
    auto& cam = const_cast<TransformState&>(aDC._transState);  
    GlPainters& painter = const_cast<GlPainters&>(aPainter);
     
    _test(painter, aDC);
    // dbgDrawBound
    painter.dbgDrawBound(aDC, { 233,0,0,255 });
     
    _drawMapStatus(painter, aDC);

    _drawTileBorders(painter, aDC);

    _drawLines(painter, aDC);

    _drawBuildings(painter, aDC);
    return ret;
}
bool DebugSceneLayer::_drawMapStatus(const GlPainters& aPainter,
    const CPaintContext& aDC)
{
    auto& cam = const_cast<TransformState&>(aDC._transState);
    GlPainters& painter = const_cast<GlPainters&>(aPainter);

    ColorI  c = { 2,2,255,255 };
    {
        // draw view center LonLat
        char tmp[256] = { 0 };
        Pt3D mct = cam.getMapCenter();
        LonLat ctLL = MercPrj::LngLatFromPx({ mct.x,mct.y }, BASE_ZOOM);
        snprintf(tmp, sizeof(tmp), "%f,%f", ctLL.mLon, ctLL.mLat);
        ScreenPt scrnPt = cam.getViewCenter();
        painter.dbgDraw2dText(aDC, scrnPt.x, scrnPt.y, c, tmp);

        // 
        TileIdArray  tiles;
        cam.getCoveredTiles(tiles);
        double z = cam.getZoom();
        snprintf(tmp, sizeof(tmp), "zoom=%f;ct=%f,%f,tileCnt=%d", 
            z, ctLL.mLon, ctLL.mLat,
            tiles.size() );
        painter.dbgDraw2dText(aDC, 20, 20, c, tmp);
    }

    return true;
}

bool DebugSceneLayer::_drawTileBorders(const GlPainters& aPainter,
    const CPaintContext&  aDC )
{
    auto& cam = const_cast<TransformState&>(aDC._transState);
    GlPainters& painter = const_cast<GlPainters&>(aPainter);
 
    ColorI  c = { 255,2,2,255 }; 

    TileIdArray  tiles;
    cam.getCoveredTiles(tiles);
    for (auto& TLId : tiles)
    {         
        //---- draw tile border
        painter.dbgDrawTileBorder( aDC, TLId, c);
        
        //---- draw label at lb
        Pt2D tileOrigin = MercPrj::tileOriginInBpx(TLId);
        ScreenPt scrnPt= cam.screenFromBpx({ tileOrigin.x,tileOrigin.y,0 });
        char tmp[256] = { 0 };
        snprintf(tmp, sizeof(tmp), "%d-%d-%d", TLId.z, TLId.x, TLId.y);
        painter.dbgDraw2dText(aDC, scrnPt.x, scrnPt.y, c, tmp);

        //---- draw label at ct
        Pt2D tileCt = MercPrj::tileCenterInBpx(TLId);
        scrnPt = cam.screenFromBpx({ tileCt.x,tileCt.y,0 }); 
        painter.dbgDraw2dText(aDC, scrnPt.x, scrnPt.y, c, tmp);
       
        //  GDEBUG: for debug
        //break; 
    }
       
    return true;
}

void DebugSceneLayer::_test(const GlPainters& aPainter,
    const CPaintContext& aDC)
{
    auto& cam = const_cast<TransformState&>(aDC._transState);
    GlPainters& painter = const_cast<GlPainters&>(aPainter);

    Pt2D t01=MercPrj::tileXYFromLngLat({ -LONGITUDE_MAX ,-LATITUDE_MAX },3);

    Pt2D t02 = MercPrj::tileXYFromLngLat({ LONGITUDE_MAX ,LATITUDE_MAX }, 3);
    //Pt2D tileOrigin = MercPrj::tileOriginInBpx(TLId);
    //ScreenPt scrnPt = cam.screenFromBpx({ tileOrigin.x,tileOrigin.y,0 })

    Pt2D p01 = MercPrj::pxFromLngLat({ -LONGITUDE_MAX ,-LATITUDE_MAX }, BASE_ZOOM);
    Pt2D p02 = MercPrj::pxFromLngLat({ LONGITUDE_MAX ,LATITUDE_MAX }, BASE_ZOOM);
    TileCoord t01_p01 = MercPrj::tileCoord4bpx(p01, 3);
    TileCoord t01_p02 = MercPrj::tileCoord4bpx(p02, 3);
    Log::Debug("bpx: p01(%lf,%lf) p02(%lf,%lf)\n", p01.x, p01.y, p02.x, p02.y);

    double vw = cam.viewWidth();
    double vh = cam.viewHeight();
    bool bIntersect = true;
    Pt3D tl=  cam.bpxFromScreen(0,   0, bIntersect);
    Pt3D tr = cam.bpxFromScreen(vw,  0, bIntersect);
    Pt3D bl = cam.bpxFromScreen(0,  vh, bIntersect);
    Pt3D br = cam.bpxFromScreen(vw, vh, bIntersect);
    Log::Debug("bpx: tl(%lf,%lf) tr(%lf,%lf)\n",tl.x, tl.y, tr.x, tr.y);
    Log::Debug("bpx: bl(%lf,%lf) br(%lf,%lf)\n",bl.x, bl.y, br.x, br.y);

    LonLat LL_tl = MercPrj::LngLatFromPx({ tl.x,tl.y }, BASE_ZOOM);
    LonLat LL_tr = MercPrj::LngLatFromPx({ tr.x,tr.y }, BASE_ZOOM);
    LonLat LL_bl = MercPrj::LngLatFromPx({ bl.x,bl.y }, BASE_ZOOM);
    LonLat LL_br = MercPrj::LngLatFromPx({ br.x,br.y }, BASE_ZOOM);
    Log::Debug("LonLat: tl(%lf,%lf) tr(%lf,%lf)\n", 
        LL_tl.mLon, LL_tl.mLat, LL_tr.mLon, LL_tr.mLat);
    Log::Debug("LonLat: bl(%lf,%lf) br(%lf,%lf)\n", 
        LL_bl.mLon, LL_bl.mLat, LL_br.mLon, LL_br.mLat);
    Log::Debug("DebugSceneLayer::_test\n");
}
 
void DebugSceneLayer::_drawLines(const GlPainters& aPainter,
    const CPaintContext& aDC)
{
    auto& cam = const_cast<TransformState&>(aDC._transState);
    GlPainters& painter = const_cast<GlPainters&>(aPainter);


    ColorI  c = { 255,2,2,255 };
    std::vector<Pt2Df>  pts;
    pts.emplace_back(0,0);
    pts.emplace_back(5, 299);
    painter.dbgDrawSolidLineInPx(aDC,c,12.0,
        (float*)pts.data(), pts.size() );

    std::vector<Pt2Df>  pts1{ Pt2Df{15, 0}, {20, 299} }; 
    painter.dbgDrawSolidLineInPx(aDC, c, 10.0,
        (float*)pts1.data(), pts1.size());

    std::vector<Pt2Df>  pts2{ Pt2Df{25, 0}, {30, 299} }; 
    painter.dbgDrawSolidLineInPx(aDC, c, 7.0,
        (float*)pts2.data(), pts2.size());

    pts.clear();
    pts.emplace_back(0, 0);
    pts.emplace_back(5, -299);
    painter.dbgDrawThinSolidLineInPx(aDC, c, 12.0,
        (float*)pts.data(), pts.size());

}

void DebugSceneLayer::_drawBuildings(const GlPainters& aPainter,
    const CPaintContext& aDC)
{

}

} // namespace md

// clang-format on
