#pragma once
#include <md/util/geo.hpp> 
 
namespace clientmap {

/*
Mercator-Meters  OR BPX
----------------
    Map-projections or BPX define a 2D coordinate system whose origin 
    is at longitude and latitude zero .

    +------------------+
    |        |         |
    |     +y ^         |
    |        |   +x    |     N
    |---<----+---->----|  W <|> E
    |        |(0,0)    |     S
    |        v         |
    |        |         |
    +------------------+
*/ 
template <typename T> struct SPoint2D;// 模板声明
using MercMeters = SPoint2D<double>;//ProjectedMeters


/*
Tile Coordinates 这是TileID浮点数版本
----------------
 Tiles are addressed within a 2D coordinate system at each zoom level whose
 origin is at the bottom-left corner of the spherical mercator projection space.
 The space is divided into 2^z tiles at each zoom, so that the boundary of the
 coordinates in each dimension is 2^z.

 +------------------+
 |(0,2^z) |         | (2^z,2^z)
 |        |         |
 |        |         |     N
 |--------+---------|  W <|> E
 A +y     |         |     S
 |        |         |
 |    +x  |         |
 +------>-----------+
 (0,0)             (2^z,0)
*/
struct TileCoord {
    double x;
    double y;
    int z;
};


// 其他类型声明
using Pt2D = SPoint2D<double>;
using Pt2Di = SPoint2D<int>;
using TileInPx = SPoint2D<int16_t>;// Tile inner pixel

class LonLat;
class TileID;
// ---------------


// Web Mercator projection 
class MercPrj
{
public:
    constexpr static double tileWIDTH = 256.0;
    // WORLD_WIDTH_BPX = worldPxWidth(0); //1,073,741,824//10 7374 1824
    constexpr static double WORLD_WIDTH_BPX      = 1073741824.0; 
    // bpx (- [-536870912.0,536870912.0)
    constexpr static double WORLD_HALF_WIDTH_BPX =  536870912.0;

    constexpr static double PI = 3.14159265358979323846;
    constexpr static double EARTH_RADIUS_M = 6378137.0;
    constexpr static double EARTH_HALF_CIRCUMFERENCE_M = PI * EARTH_RADIUS_M;
    constexpr static double EARTH_CIRCUMFERENCE_M = 2 * PI * EARTH_RADIUS_M;
    constexpr static double MAX_LATITUDE_DEG = 85.05112878;
    //-------------------------------------------------
    MercPrj() = delete;
    //-------------------------------------------------
    static double scale4zoom(double zoom);//scaleFromZoom
    static double zoom4scale(double s);//zoomFromScale
    //
    //  earth pixel width at given scale.
    //
    // MNOTE: scale = pow(2.0, zoom);
    // MNOTE: zoom is ZoomLevel, which has a scope [MIN_ZOOM,MAX_ZOOM].    
    static double worldPxWidth(double aZoom);
    //-------------------------------------------------    
    static double zoomAtMetersPerPx(double aMetersPerPixel);// not used  
    static double metersPerPxAtZoom(double aMpxZoom);
    static double pixelsPerMeterAtZoom(double aMpxZoom);
    // getMetersPerPixelAtLatitude.
    static double metersPerPixelAtLat(double lat, double aMpxZoom);
	// The unit of GeoWidth is bpx.
    static double bpxsPerPx(double aMpxZoom);//pxGeoWidth(zoom) 
	static double pxGeoWidth(double aZoom);

    static double tileMetersWidth(double aTileZoom);
    static double tileBpxWidth(double aTileZoom);
    static int    tileBpxWidthAtZoomi(int aTileZoom);
    static double tilePxWidth(double aTileZoom);
    //-------------------------------------------------
    static MercMeters mercM4LngLat(const LonLat& latLng);
    static LonLat lngLat4mercM(const MercMeters& projectedM);
     
    //----------------- 
    //----------------- 
    // MNOTE: latLng -> mpx(MapPixel Coordinate, also abbreviated as px )
    // mpx(MapPixel )是指 比例尺scale下，将全球看作一个位图，
    // 地球上某个点在该位图内的像素坐标。
    //
    // bpx(BasePixel )是指瓦片金字塔的BASE_ZOOM下，将全球看作一个位图，
    // 地球上某个点在该位图内的像素坐标。
    //
    // MapPixel坐标系原点在经纬度的原点处，正方向向右向上。
    static Pt2D  pxFromLngLat(const LonLat& latLng, double aZoom);
    
    //unproject function
    static LonLat LngLatFromPx(const Pt2D& aMpxPt,
            double aMpxZoom, bool isWrapped = false);
    
    //  tileInPx  <-->  MercM
    static TileInPx tileInPx4MercM(const MercMeters& aMercMPt,
        const TileID& aTileId);
    static MercMeters mercM4TileInPx(const TileInPx& aTileInPt,
        const TileID& aTileId);
    // bpx  <--> MercM ,need ???

    //----------------- 
    //----------------- 
    // MNOTE: latLng -> tileXY(tileIdX,tileIdY)
    // The origin of tileXY's coordinate system is also at the upper-left 
    // corner of the world, and the positive direction is right and down.
    // Returns point on tile.
    static Pt2D tileXYFromLngLat(const LonLat& aLngLat, int32_t aZoom);
    // ZD at the end of the function name is the abbreviation of zoomDouble.
    static Pt2D tileXYFromLngLatZd(const LonLat& aLngLat, double aZoom);
    //--------------------
    //-----TileCoord ----- 
    static TileCoord
        tileCoordFromLatLng(const LonLat& aLL, double aZoom);
    static TileID tileIdFromLatLng(const LonLat& aLL, int aZoom);

    static TileCoord tileCoordZoomTo(const TileCoord& aTC, double aZoom);
    //--------------------
    //  tileCoord <--> mercator meters
    static MercMeters mercM4tileCoord(const TileCoord& aTC);
    static TileCoord  tileCoord4mercM(const MercMeters& aMM,int aZoom);   
    // Tile bound for mercator meters coordinates 
    static Box2D      tileBoundsInMercM(const TileID& aTile);
    static MercMeters tileCenterInMercM(const TileID& aTile);
    static MercMeters tileOriginInMercM(const TileID& tile);

    // tileCoord <--> bpx coordinates
    static Pt2D  px4tileCoord(const TileCoord&  aTileCoord);
    static Pt2D bpx4tileCoord(const TileCoord& aTC);
    static Pt2D tileXY4bpx(const Pt2D& aBpxPt, int aTileZoom);
    static TileCoord tileCoord4bpx(const Pt2D& aBpxPt, int aTileZoom);    
    static TileID tileId4bpx(const Pt2D& aBpxPt, int aTileZoom);
    static Box2D tileBoundsInBpx(const TileID& aTile);
    static Box2D tileBoundsInPx(const TileID& aTile);
    static Pt2D  tileCenterInBpx(const TileID& aTile);
    // (tileSouthWestCorner)
    static Pt2D  tileOriginInBpx(const TileID& tile);
    //--------------------
    
public:
    //-------------MercRad--LL---------
    // get mercator Rad from longitude/latitude degree.
    static Pt2D lngLat4MercRad(const double aMercRadX,
        const double aMercRadY);
    // 实现方法2
    static Pt2D lngLat4MercRad_1(const double aMercRadX,
        const double aMercRadY);

    // get mercator Rad from longitude/latitude degree.
    static Pt2D mercRad4LngLat(const double aLng, const double aLat);
    // 实现方法2
    static Pt2D mercRad4LngLat_1(const double aLng, const double aLat);
    //-----------------mpx-----MercRad---------------
    //Mercator coordinates to Map Pixel Points
    static Pt2D px4MercRad(const Pt2D& aMercRad, double zoom);

    //Map Pixel Points to Mercator coordinates
    static Pt2D mercRad4px(const Pt2D& aMpx, double zoom);
    //----mpx-----bpx
    static Pt2D px4bpx(const Pt2D& aBpx, double aMpxZoom);// m <-- b
    static Pt2D bpx4px(const Pt2D& aMpx, double aMpxZoom);// b <-- m
    //-------------TileXY-----mpx-------------------
    //Tile ID to Map Pixel Points
    static Pt2D px4TileXY(const Pt2Di& aTileXY, double aMpxZoom);

    //Pixel Points to TileID (no zoom)
    static Pt2Di tileXY4px(const Pt2D&  aMpx, double aMpxZoom);
    //------------------- 
};


} // namespace clientmap
