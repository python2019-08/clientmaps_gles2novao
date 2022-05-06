#pragma once

#include <md/util/constants.hpp>
#include <md/util/geo.hpp> 
#include <md/util/math_tools.hpp>
#include <md/util/tile_id.hpp>

#include <md/util/merc_prj.hpp>

namespace clientmap {
 
// Web Mercator projection 

    double MercPrj::scale4zoom(double zoom)
    {
        //int scale = ( 1 << ((int)zoom)  ) ;
        return std::pow(2.0, zoom);//== std::exp2(zoom);
    }

    double MercPrj::zoom4scale(double s)
    {
        return clientmap::log2(s);
    }
    //
    //  Map pixel width at given scale.
    //
    // MNOTE: scale = pow(2.0, zoom);
    // MNOTE: zoom is ZoomLevel, which has a scope [MIN_ZOOM,MAX_ZOOM].    
    double MercPrj::worldPxWidth(double aZoom)
    {
        return  tileWIDTH * scale4zoom(aZoom);
    }

    // zoomAtMetersPerPixel
    double MercPrj::zoomAtMetersPerPx(double metersPerPixel)
    {
        return log2(EARTH_CIRCUMFERENCE_M / (metersPerPixel * tileWIDTH));
    }
    // metersPerPixelAtZoom
    double MercPrj::metersPerPxAtZoom(double aMpxZoom)
    {
#if 1
        return EARTH_CIRCUMFERENCE_M / (std::exp2(aMpxZoom) * tileWIDTH);
#else
        return tileGeoWidthAtZoom(zoom) / tileMpxWidthAtZoom(aMpxZoom);
#endif
    }

    double MercPrj::pixelsPerMeterAtZoom(double aMpxZoom)
    {
        double metersPerTile = MercPrj::EARTH_CIRCUMFERENCE_M * std::exp2(-aMpxZoom);
        return MercPrj::tileWIDTH / metersPerTile;
    }

    // get metersPerPixel at given latitude.
    double MercPrj::metersPerPixelAtLat(double lat, double aMpxZoom)
    {
        const double zoom1 = cm_clamp(aMpxZoom, MIN_ZOOM, MAX_ZOOM);

        const double Lat1 = cm_clamp(lat, -LATITUDE_MAX, LATITUDE_MAX);

        return std::cos(Lat1 * DEG2RAD)
            * M2PI * EARTH_RADIUS_M / worldPxWidth(zoom1);
    }
    // 宽度方向上，一个mpx 对应几个bpx
    double MercPrj::bpxsPerPx(double aMpxZoom)
    {
        aMpxZoom = cm_clamp<double>(aMpxZoom, MIN_ZOOM, BASE_ZOOM);
        return std::exp2(BASE_ZOOM - aMpxZoom);
    }

	double MercPrj::pxGeoWidth(double aZoom)
	{
		return bpxsPerPx(aZoom);
	}

    // metersPerTileAtZoom
    double MercPrj::tileMetersWidth(double aTileZoom)
    {
        return EARTH_CIRCUMFERENCE_M / std::exp2(aTileZoom);
    }
    // bpxsPerTileAtZoom
    double MercPrj::tileBpxWidth(double aTileZoom)
    {
        aTileZoom = cm_clamp<double>(aTileZoom, MIN_ZOOM, BASE_ZOOM);
        return std::exp2(BASE_ZOOM - aTileZoom)* tileWIDTH;
    }

    int MercPrj::tileBpxWidthAtZoomi(int aTileZoom)
    {
        aTileZoom = cm_clamp<int>(aTileZoom, MIN_ZOOM, BASE_ZOOM);
        return (1 << (BASE_ZOOM - aTileZoom))*((int)tileWIDTH);
    }
    // mpxsPerTileAtZoom
    double MercPrj::tilePxWidth(double aTileZoom)
    {
        return tileWIDTH;
    }
    

    MercMeters MercPrj::mercM4LngLat(const LonLat& latLng)
    {
        double Lon1 = cm_wrap(latLng.lng(),  -LONGITUDE_MAX, LONGITUDE_MAX);
        double Lat1 = cm_clamp(latLng.lat(), -LATITUDE_MAX,  LATITUDE_MAX);
        
        Pt2D mercRadXY= mercRad4LngLat_1(Lon1, Lat1);

        const double northing = mercRadXY.y * EARTH_RADIUS_M;
        const double easting  = mercRadXY.x * EARTH_RADIUS_M;

        return { easting, northing };
    }

    LonLat 
    MercPrj::lngLat4mercM(const MercMeters& aProjectedMeters)
    {
        double mercRadX = aProjectedMeters.x / EARTH_RADIUS_M;
        double mercRadY = aProjectedMeters.y / EARTH_RADIUS_M;

        Pt2D LL= lngLat4MercRad(mercRadX, mercRadY);

        LL.y = cm_clamp(LL.y,-LATITUDE_MAX, LATITUDE_MAX);
        //LL.x= cm_clamp(LL.x,-LONGITUDE_MAX,LONGITUDE_MAX);

        return { LL.x, LL.y };
    }


//------------------------------------------------
//------------------------------------------------
// MNOTE: latLng -> MapPixel Coordinate
//     见src\mbgl\tile\geometry_tile_data.cpp/latLonToTileCoodinates函数
//     见  render-test\runner.cpp/ getTileCenterCoordinates函数
//     见       tilePyramid.update(
//     见     src\mbgl\map\transform_state.cpp
// MapPixel坐标系原点在世界的左上角，正方向向右向下。
Pt2D MercPrj::pxFromLngLat(const LonLat& latLng,  double aZoom)
{ 
    double worldSize = worldPxWidth(aZoom);
    Pt2D mercRadPt = mercRad4LngLat(latLng.lng(), latLng.lat());
    return Pt2D{ mercRadPt.x, mercRadPt.y} * worldSize / M2PI;
}

LonLat  MercPrj::LngLatFromPx(const Pt2D& aMpxPt,
    double aMpxZoom,  
    bool isWrapped)
{
    auto p2 = aMpxPt * M2PI / worldPxWidth(aMpxZoom);
    Pt2D LL = lngLat4MercRad(p2.x, p2.y);
    return LonLat{ LL.x, LL.y, isWrapped};
}

TileInPx MercPrj::tileInPx4MercM(const MercMeters& aMercMPt,
    const TileID& aTileId)
{
    Box2D   tileBbox     = tileBoundsInMercM(aTileId);
    double  metersPerMpx = metersPerPxAtZoom(aTileId.z);

    double x = (aMercMPt.x - tileBbox.mMinX) / metersPerMpx;
    double y = (aMercMPt.y - tileBbox.mMinY) / metersPerMpx;

    return { (short)x, (short)y };
}

MercMeters MercPrj::mercM4TileInPx(const TileInPx& aTileInPx,
    const TileID& aTileId)
{
    //X = PX * SCALE(LEVEL) + MINX
    //Y = PY * SCALE(LEVEL) + MINY
    Box2D   tileBbox = tileBoundsInMercM(aTileId);
    double  metersPerMpx = metersPerPxAtZoom(aTileId.z);

    double x = tileBbox.mMinX + aTileInPx.x * metersPerMpx;
    double y = tileBbox.mMinY + aTileInPx.y * metersPerMpx;
    return { x,  y };
}

//--------------------
// MNOTE: latLng -> tileIdX,tileIdY
// tileIdXY的坐标系原点也在世界的左上角，正方向向右向下。
//Returns point on tile
Pt2D MercPrj::tileXYFromLngLat(const LonLat& aLngLat, 
    int32_t aZoom)
{
    int32_t tileCnt_1D = 1 << aZoom; 
    Pt2D mercRadPt = mercRad4LngLat(aLngLat.lng(), aLngLat.lat());
    Pt2D tileXY    = Pt2D{ M_PI + mercRadPt.x,  M_PI + mercRadPt.y } 
                  * ((double)tileCnt_1D) / M2PI;

    tileXY.x = cm_clamp<double>(tileXY.x, 0, tileCnt_1D - 1);
    tileXY.y = cm_clamp<double>(tileXY.y, 0, tileCnt_1D - 1);

    return tileXY;
}

// 函数名尾部的Zd是ZoomDouble的缩写 
Pt2D MercPrj::tileXYFromLngLatZd(const LonLat& aLngLat, 
    double aZoom)
{
    const double tileCnt_1D = std::pow(2.0, aZoom);
    Pt2D mercRadPt = mercRad4LngLat(aLngLat.lng(), aLngLat.lat());
    Pt2D tileXY = Pt2D{ M_PI + mercRadPt.x,  M_PI + mercRadPt.y
                       } *tileCnt_1D / M2PI;

    tileXY.x = cm_clamp<double>(tileXY.x, 0, tileCnt_1D - 1);
    tileXY.y = cm_clamp<double>(tileXY.y, 0, tileCnt_1D - 1);

    return tileXY;
}
	
	
	
//-------------------------------------------------
TileCoord
MercPrj::tileCoordFromLatLng(const LonLat& aLL, double aZoom)
{
    auto  tileXY = tileXYFromLngLatZd(aLL, aZoom);
    return { tileXY.x, tileXY.y, static_cast<int>(aZoom) };
}

TileID
MercPrj::tileIdFromLatLng(const LonLat& aLL, int aZoom)
{
    auto  tileXY = tileXYFromLngLat(aLL, aZoom);
    return { static_cast<uint8_t>(aZoom),
             (uint32_t)tileXY.x, 
             (uint32_t)tileXY.y};
}

TileCoord
MercPrj::tileCoordZoomTo(const TileCoord& aTC, double aZoom)
{
    //-----------------------------------------------
    // const float scale = curScale/ (1 << id.canonical.z); 
    // ##equivalent-to## std::pow(2, curZoom - id.canonical.z);
    //-----------------------------------------------
    const double scaleDiff = std::pow(2.0, aZoom - aTC.z);
    return { aTC.x * scaleDiff,    aTC.y * scaleDiff, 
            static_cast<int>(aZoom) };
}

 
MercMeters MercPrj::mercM4tileCoord(const TileCoord&  aTC)
{
    double metersPerTile = tileMetersWidth(aTC.z);

    MercMeters  mm;
    mm.x = aTC.x * metersPerTile - EARTH_HALF_CIRCUMFERENCE_M;
    mm.y = aTC.y * metersPerTile - EARTH_HALF_CIRCUMFERENCE_M;
        
    return mm;
}

TileCoord MercPrj::tileCoord4mercM(const MercMeters& aMM, int aZoom)
{
    double mpt = tileMetersWidth(aZoom);
    return {
        (aMM.x + EARTH_HALF_CIRCUMFERENCE_M) / mpt,
        (aMM.y + EARTH_HALF_CIRCUMFERENCE_M) / mpt,
        aZoom
    };
}
 
Box2D MercPrj::tileBoundsInMercM(const TileID&  tile)
{
    TileCoord minTileCoord{ static_cast<double>(tile.x),
                            static_cast<double>(tile.y),
                            tile.z };
    TileCoord maxTileCoord{ static_cast<double>(tile.x) + 1.0,
                            static_cast<double>(tile.y) + 1.0,
                            tile.z };
    auto minProjectedMeters = mercM4tileCoord(minTileCoord);
    auto maxProjectedMeters = mercM4tileCoord(maxTileCoord);
    return Box2D{ minProjectedMeters, maxProjectedMeters };
}

MercMeters MercPrj::tileCenterInMercM(const TileID& tile)
{
    TileCoord tileCoord{static_cast<double>(tile.x) + 0.5,
                        static_cast<double>(tile.y) + 0.5,
                        tile.z};
    return mercM4tileCoord(tileCoord);
}

MercMeters MercPrj::tileOriginInMercM(const TileID& tile)
{
    TileCoord tileCoord{ static_cast<double>(tile.x),
                        static_cast<double>(tile.y),
                        tile.z };
    return mercM4tileCoord(tileCoord);
}

Pt2D MercPrj::bpx4tileCoord(const TileCoord&  aTileCoord)
{
    double bpxPerTile = tileBpxWidth(aTileCoord.z);

    return{ aTileCoord.x * bpxPerTile - WORLD_HALF_WIDTH_BPX, 
            aTileCoord.y * bpxPerTile - WORLD_HALF_WIDTH_BPX };
}


Pt2D MercPrj::px4tileCoord(const TileCoord&  aTileCoord)
{
    double worldHalfWidthPx= worldPxWidth(aTileCoord.z)*0.5;
    double bpxPerTile = tilePxWidth(aTileCoord.z);

    return{ aTileCoord.x * bpxPerTile - worldHalfWidthPx,
            aTileCoord.y * bpxPerTile - worldHalfWidthPx };
}

Pt2D  MercPrj::tileXY4bpx(const Pt2D& aBpxPt, int aTileZoom)
{
    double bpxPerTile = tileBpxWidth(aTileZoom);
    double tileX = (aBpxPt.x + WORLD_HALF_WIDTH_BPX) / bpxPerTile;
    double tileY = (aBpxPt.y + WORLD_HALF_WIDTH_BPX) / bpxPerTile;

    const double tileCnt_1D = std::pow(2.0, aTileZoom);
    tileX = cm_clamp<double>(tileX, 0.0, tileCnt_1D - 1);
    tileY = cm_clamp<double>(tileY, 0.0, tileCnt_1D - 1);
    return Pt2D{ tileX, tileY };
}
TileCoord MercPrj::tileCoord4bpx(const Pt2D& aBpxPt, int aTileZoom)
{
    Pt2D tileXY = tileXY4bpx(aBpxPt, aTileZoom);



    return{ tileXY.x,tileXY.y, aTileZoom };
}

TileID MercPrj::tileId4bpx(const Pt2D& aBpxPt, int aTileZoom)
{
    double bpxPerTile = (double)tileBpxWidthAtZoomi(aTileZoom);
    return{ (uint8_t)aTileZoom,
            (uint32_t)( (aBpxPt.x + WORLD_HALF_WIDTH_BPX) / bpxPerTile),
            (uint32_t)( (aBpxPt.y + WORLD_HALF_WIDTH_BPX) / bpxPerTile)
              };
}

Box2D MercPrj::tileBoundsInBpx(const TileID& tile)
{
    TileCoord minTileCoord{ static_cast<double>(tile.x),
                            static_cast<double>(tile.y),
                            tile.z };
    TileCoord maxTileCoord{ static_cast<double>(tile.x) + 1.0,
                            static_cast<double>(tile.y) + 1.0,
                            tile.z };
    auto minBpx = bpx4tileCoord(minTileCoord);
    auto maxBpx = bpx4tileCoord(maxTileCoord);
    return Box2D{ minBpx, maxBpx };
}

Box2D MercPrj::tileBoundsInPx(const TileID& tile)
{
    TileCoord minTileCoord{ static_cast<double>(tile.x),
                            static_cast<double>(tile.y),
                            tile.z };
    TileCoord maxTileCoord{ static_cast<double>(tile.x) + 1.0,
                            static_cast<double>(tile.y) + 1.0,
                            tile.z };
    auto minPx = px4tileCoord(minTileCoord);
    auto maxPx = px4tileCoord(maxTileCoord);
    return Box2D{ minPx, maxPx };
}

Pt2D  MercPrj::tileCenterInBpx(const TileID& aTile)
{
    TileCoord tileCoord{ static_cast<double>(aTile.x) + 0.5,
                    static_cast<double>(aTile.y) + 0.5,
                    aTile.z };
    return bpx4tileCoord(tileCoord);
}

Pt2D  MercPrj::tileOriginInBpx(const TileID& aId)
{
    TileCoord tileCoord{ static_cast<double>(aId.x),
                        static_cast<double>(aId.y),
                        aId.z };
    return bpx4tileCoord(tileCoord);
}
//-------------------------------------------------
//
// get mercator degree from longitude/latitude degree.
Pt2D  MercPrj::lngLat4MercRad(const double aMercRadX,
                            const double aMercRadY)
{
    double longitude = aMercRadX * RAD2DEG;
    double latitude = (2 * std::atan(std::exp(aMercRadY)) -M_PI * 0.5) 
                        *  RAD2DEG;        
    return { longitude ,latitude };
}
// 实现方法2
Pt2D MercPrj::lngLat4MercRad_1(const double aMercRadX,
                                const double aMercRadY)
{ 
    return { aMercRadX *  RAD2DEG,
             std::atan(std::sinh(aMercRadY)) *  RAD2DEG }; 
}

// get mercator Rad from longitude/latitude degree.
Pt2D MercPrj::mercRad4LngLat(const double aLng, const double aLat)
{
    double  mercRadX = aLng * DEG2RAD;

    double lat1 = cm_clamp(aLat, -LATITUDE_MAX, LATITUDE_MAX);
    double mercRadY = std::log(
        std::tan(M_PI * 0.25 + lat1 * M_PI / DEGREES_MAX)
    ); 

    return { mercRadX , mercRadY }; 
}
// 实现方法2
Pt2D MercPrj::mercRad4LngLat_1(const double aLng, const double aLat)
{ 
    //const double Lon1 = cm_clamp(aLng, -LONGITUDE_MAX, LONGITUDE_MAX);
    const double mercRadX = aLng * DEG2RAD;

    double Lat1 = cm_clamp(aLat, -LATITUDE_MAX, LATITUDE_MAX);
    const double m = 1 - 1e-15;
    const double f = cm_clamp(std::sin(DEG2RAD * Lat1), -m, m);
    const double mercRadY = 0.5 * std::log((1 + f) / (1 - f));

    return { mercRadX , mercRadX }; 
}

Pt2D MercPrj::px4MercRad(const Pt2D& aMercRad, double aZoom)
{
    //double worldSize = (1 << aZoom) * tileWIDTH;
	double worldSize = worldPxWidth( aZoom ) ;
    return Pt2D{ aMercRad.x, aMercRad.y} * worldSize / M2PI;
}

//Map Pixel Points to Mercator coordinates
Pt2D MercPrj::mercRad4px(const Pt2D& aMpx, double aZoom)
{
    //double worldSize = (1 << aZoom) * tileWIDTH;
    double worldSize = worldPxWidth( aZoom);
    return { aMpx.x / worldSize * M2PI, aMpx.y / worldSize * M2PI};
}

Pt2D MercPrj::px4bpx(const Pt2D& aBpx, double aMpxZoom)
{
    double scaleDiff = std::pow(2.0, aMpxZoom - MAX_ZOOM_I);
    return { aBpx.x * scaleDiff, aBpx.y * scaleDiff };
}
Pt2D MercPrj::bpx4px(const Pt2D& aMpx, double aMpxZoom)
{
    double scaleDiff = std::pow(2.0, MAX_ZOOM_I - aMpxZoom);
    return { aMpx.x * scaleDiff, aMpx.y * scaleDiff };
}

//Tile ID to Pixel Points
Pt2D MercPrj::px4TileXY(const Pt2Di& aTileXY, double aMpxZoom)
{
    const double tileCnt_1D = std::pow(2.0, aMpxZoom);
    double tileX = cm_clamp<double>(aTileXY.x, 0.0, tileCnt_1D -1);
    double tileY = cm_clamp<double>(aTileXY.y, 0.0, tileCnt_1D -1);

    double hw = worldPxWidth(aMpxZoom) * 0.5;// half world size
    return { tileX * tileWIDTH - hw,  tileY * tileWIDTH - hw };
}

//Pixel Points to TileID (no zoom)
Pt2Di MercPrj::tileXY4px(const Pt2D&  aMpx, double aMpxZoom)
{     
    double hw = worldPxWidth(aMpxZoom) * 0.5;// half world size
    double x = cm_wrap(aMpx.x, -hw, hw);
    double y = cm_clamp(aMpx.y, -hw, hw);

    int tileX = static_cast<int>((x + hw) / tileWIDTH);
    int tileY = static_cast<int>((y + hw) / tileWIDTH);

    const double tileCnt_1D = std::pow(2.0, aMpxZoom);
    tileX = cm_clamp<int>(tileX, 0, tileCnt_1D - 1);
    tileY = cm_clamp<int>(tileY, 0, tileCnt_1D - 1);
    return Pt2Di{ tileX, tileY};
}
 

/*
class TilePyramid {
public:
	constexpr static double m_pxRatio = 1.0;

	static double getPixelGeoWidth(int aZoom)
	{
		return getTileGeoWidth(aZoom) / getTilePixelWidth();
	}
	static double getTilePixelWidth() {
		auto width = 256;
		return std::ceil(width * m_pxRatio);
	}
	static double getTileGeoWidth(int layer) {
		return (1 << (18 - layer)) * getTileGeoWidthBottom();
	}
	static double getTileGeoWidthBottom()
	{
		return  125;
	}
	static TileID getTileID(double x, double y, int level)
	{
		auto glpp = getTileGeoWidth(level);
		return TileID(int(x / glpp), int(y / glpp), level);
	}
};
*/

} // namespace MdU
