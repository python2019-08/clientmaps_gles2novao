#include <functional>
#include <list>
#include <glm/glm.hpp>

#include <md/util/math_tools.hpp> 
#include <md/util/constants.hpp> 
#include <md/util/geo.hpp>
#include <md/util/merc_prj.hpp>
#include <md/util/tile_id.hpp> 
#include <md/util/logtool.hpp>

#include <md/util/tile_cover_bound.hpp>
 
 
//namespace {
//
//} // namespace

namespace clientmap {

namespace {
    using ScanLineF = const std::function<void(int32_t x0,
        int32_t x1,
        int32_t y)>;

    // Taken from polymaps src/Layer.js
    // https://github.com/simplegeo/polymaps/blob/master/src/Layer.js#L333-L383
    struct edge
    {
        double x0 = 0, y0 = 0;
        double x1 = 0, y1 = 0;
        double dx = 0, dy = 0;

        edge( SPoint2D<double> a,  SPoint2D<double> b)
        {
            if (a.y > b.y)
                std::swap(a, b);
            x0 = a.x;
            y0 = a.y;
            x1 = b.x;
            y1 = b.y;
            dx = b.x - a.x;
            dy = b.y - a.y;
        }
    };

    // scan-line conversion
    void scanSpans(edge e0, edge e1,
        int32_t ymin, int32_t ymax,
        ScanLineF& aScanLineF
    )
    {
        double y0 = ::fmax(ymin, std::floor(e1.y0));
        double y1 = ::fmin(ymax, std::ceil(e1.y1));
		 
        // sort edges by x-coordinate
        if ((e0.x0 == e1.x0 && e0.y0 == e1.y0) ?
            (e0.x0 + e1.dy / e0.dy * e0.dx < e1.x1) :
            (e0.x1 - e1.dy / e0.dy * e0.dx < e1.x0))
        {
            std::swap(e0, e1);
        }

        // scan lines!
        double m0 = e0.dx / e0.dy;
        double m1 = e1.dx / e1.dy;
        double d0 = e0.dx > 0; // use y + 1 to compute x0
        double d1 = e1.dx < 0; // use y + 1 to compute x1
        //for (int32_t y = y0; y < y1; y++)//raw code  
		for (int32_t y = y0; y <= y1; y++) //mq code
        {
            double x0 = m0 * ::fmax(0, ::fmin(e0.dy, y + d0 - e0.y0)) + e0.x0;
            double x1 = m1 * ::fmax(0, ::fmin(e1.dy, y + d1 - e1.y0)) + e1.x0;
            //aScanLineF(std::floor(x1), std::ceil(x0), y); // raw code 

			double new_x0 = std::floor(x1);//mq code
			double new_x1 = std::ceil(x0);//mq code
			aScanLineF(new_x0, new_x1, y);//mq code
        }
    }

    // scan-line conversion
    void scanTriangle(const  SPoint2D<double>& a,
        const SPoint2D<double>& b,
        const SPoint2D<double>& c,
        int32_t ymin, int32_t ymax,
        ScanLineF& aScanLineF)
    {
        edge ab = edge(a, b);
        edge bc = edge(b, c);
        edge ca = edge(c, a);

        // sort edges by y-length
        if (ab.dy > bc.dy) {
            std::swap(ab, bc);
        }
        if (ab.dy > ca.dy) {
            std::swap(ab, ca);
        }
        if (bc.dy > ca.dy) {
            std::swap(bc, ca);
        }

        // scan span! scan span!
        if (ab.dy)
            scanSpans(ca, ab, ymin, ymax, aScanLineF);
        if (bc.dy)
            scanSpans(ca, bc, ymin, ymax, aScanLineF);
    }


std::vector<TileID>
tileCover(const SPoint2D<double>& tl,
        const SPoint2D<double>& tr, 
        const SPoint2D<double>& br,
        const SPoint2D<double>& bl, 
        const SPoint2D<double>& c,
        uint8_t z) 
{
	z = glm::clamp<uint8_t>(z, 0, MAX_ZOOM_I);// gm added
    const int32_t tiles = 1 << z;// gm: tile Cnt on one axis

    struct ID {
        int32_t x, y;
        double sqDist;
    };

    std::vector<ID> t;

    auto scanLineF = [&](int32_t x0, int32_t x1, int32_t y) 
    {        
        int32_t x;
		if (y >= 0 && y <= tiles)  
        {
            //for (x = x0; x < x1; ++x)// raw code 
			for (x = x0; x <= x1; ++x)// mq code 
            {
                //// gm: limit x to the range [0, (1 << z)),start
                int32_t tmp_x = glm::clamp<int32_t>(x, 0, tiles-1);// mq code 
				int32_t tmp_y = glm::clamp<int32_t>(y, 0, tiles-1);// mq code 
                //// gm: limit x to the range [0, (1 << z)),end
				if (t.size() > 100)
				{
					Log::Debug("t.size=%d\n", t.size());
				}
                const auto dx = tmp_x + 0.5 - c.x;
                const auto dy = tmp_y + 0.5 - c.y;
				//t.emplace_back(ID{ x, y, dx * dx + dy * dy }); // raw code 		             
				t.emplace_back(ID{ tmp_x, tmp_y, dx * dx + dy * dy });// mq code 
            }
        }
    };

    // Divide the screen up in two triangles and scan each of them:
    // \---+
    // | \ |
    // +---\.
    scanTriangle(tl, tr, br, 0, tiles, scanLineF);
    scanTriangle(br, bl, tl, 0, tiles, scanLineF);

    // Sort first by distance, then by x/y.
    std::sort(t.begin(), t.end(), [](const ID& a, const ID& b) 
    {
        return std::tie(a.sqDist, a.x, a.y) 
             < std::tie(b.sqDist, b.x, b.y);
    });

    // Erase duplicate tile IDs (they typically occur at the 
    // common side of both triangles).
    t.erase(std::unique(t.begin(), t.end(), [](const ID& a, const ID& b) 
        {
                return a.x == b.x && a.y == b.y;
        }), t.end());

    std::vector<TileID> result;
    result.reserve(t.size());
    for (const auto& id : t) {
        result.emplace_back(z, id.x, id.y);
    }
    return result;
}
//----------------------------------
// zoomLevel --> dataLevel 
//----------------------------------
/*
const int32 kWorldDataLevelByZoomLevel[kMaxZoomLevelForDataLevel+1] = {
	3, 3, 3, 3, 3,		// 0 - 4
	3, 3, 6, 6, 6,		// 5 - 9
	6, 6, 6, 9, 9,		// 10 - 14
	9, 9,				// 15 - 16
};

// 显示级别对应的数据级别
const int32 kMaxZoomLevelForDataLevel = 16;
const int32 kDataLevelByZoomLevel[kMaxZoomLevelForDataLevel+1] = {
	3, 3, 3, 3, 3,		// 0 - 4
	3, 3, 6, 6, 6,		// 5 - 9
	9, 9, 9, 12,12,		// 10 - 14
	12,15,				// 15 - 16
};
*/
#if  0 
constexpr int32_t c_MaxZoomForDataLevel = MAX_ZOOM_I;
constexpr int32_t c_DataLevelByZoom[c_MaxZoomForDataLevel + 1] = {
    3, 3, 3, 3, 3,		// 0 - 4
    3, 3, 6, 6, 6,		// 5 - 9
    9, 9, 9, 12,12,		// 10 - 14
    12,15,15,15,15,     // 15 - 19
	15,15,15,           // 20- 22
};
#else
constexpr int32_t c_MaxZoomForDataLevel = MAX_ZOOM_I;
constexpr int32_t c_DataLevelByZoom[c_MaxZoomForDataLevel + 1] = {
    3, 3, 3, 3, 3,		// 0 - 4
    3, 6, 6, 7, 9,		// 5 - 9
    9, 11, 12, 13,14,	// 10 - 14
    15,16,16,16,16,     // 15 - 19
    16,16,16,           // 20- 22
};
#endif
int32_t getDataZByZoom(int32_t aZ)
{
    int32_t validZ = glm::clamp<int32_t>(aZ, 0, c_MaxZoomForDataLevel);
    int32_t dataZ = c_DataLevelByZoom[validZ];
    return dataZ;
}

} // namespace
 

// getCoveredTilesbyLonLatBound 
std::vector<TileID> 
getCoveredTilesbyLonLatBbox(const Box2D& aLngLatBBox,
          uint8_t z) 
{
    z= getDataZByZoom(z);
    if (aLngLatBBox.isEmpty() ||
        aLngLatBBox.mMinY >  LATITUDE_MAX ||
        aLngLatBBox.mMaxY < - LATITUDE_MAX)
    {
        return {};
    }
     

    Pt2D  minPt{ aLngLatBBox.mMinX,glm::max(aLngLatBBox.mMinY, -LATITUDE_MAX) };
    Pt2D  maxPt{ aLngLatBBox.mMaxX,glm::min(aLngLatBBox.mMaxY,  LATITUDE_MAX) };
    Box2D bounds(minPt, minPt);
    bounds.unite(maxPt.x, maxPt.y);     
#if 0
    LonLat southwest(bounds.southWest().x, bounds.southWest().y);
    LonLat northeast(bounds.northEast().x, bounds.northEast().y);    
    Pt2D tileXY0 =MercPrj::tileXYFromLngLat(southwest,  z);
    Pt2D tileXY1 = MercPrj::tileXYFromLngLat(northeast, z);
    int tileX0 = (int)tileXY0.x;
    int tileY0 = (int)tileXY0.y;
    int tileX1 = (int)glm::round(tileXY1.x);
    int tileY1 = (int)glm::round(tileXY1.y);

    std::vector<TileID>  ret;
    for (int x = tileX0; x < tileX1 + 1; x++)
    {
        for (int y = tileY0; y < tileY1 + 1; y++)
        {
            ret.emplace_back(z,x,y);
        }
    }
    return ret;
#else
    LonLat northwest(bounds.northWest().x, bounds.northWest().y);
    LonLat northeast(bounds.northEast().x, bounds.northEast().y);
    LonLat southwest(bounds.southWest().x, bounds.southWest().y);    
    LonLat southeast(bounds.southEast().x, bounds.southEast().y);
    LonLat center(bounds.center().x, bounds.center().y);

    return tileCover(
        MercPrj::tileXYFromLngLat(northwest, z),
        MercPrj::tileXYFromLngLat(northeast, z),
        MercPrj::tileXYFromLngLat(southeast, z),
        MercPrj::tileXYFromLngLat(southwest, z),
        MercPrj::tileXYFromLngLat(center, z),
        z);
#endif
}

int
getCoveredTilesbyLonLatQuad(const std::array<LonLat, 5>& aQuad,
    uint8_t z,
    std::vector<TileID>& aOut
)
{ 
    z = getDataZByZoom(z);

    aOut = tileCover(
        MercPrj::tileXYFromLngLat(aQuad[0], z),
        MercPrj::tileXYFromLngLat(aQuad[1], z),
        MercPrj::tileXYFromLngLat(aQuad[2], z),
        MercPrj::tileXYFromLngLat(aQuad[3], z),
        MercPrj::tileXYFromLngLat(aQuad[4], z),
        z);

    return aOut.size();
}
 

//tileCover(const SPoint2D<double>& tl,
//    const SPoint2D<double>& tr,
//    const SPoint2D<double>& br,
//    const SPoint2D<double>& bl,
//    const SPoint2D<double>& c,
int
getCoveredTilesbyBpxQuad(const std::array<Pt2D, 5>& aQuad,
    uint8_t z,
    std::vector<TileID>& aOut
)
{ 
    int32_t dataZ = getDataZByZoom( z );

    Pt2D tl = MercPrj::tileXY4bpx(aQuad[0], dataZ);
    Pt2D tr = MercPrj::tileXY4bpx(aQuad[1], dataZ);
    Pt2D br = MercPrj::tileXY4bpx(aQuad[2], dataZ);
    Pt2D bl = MercPrj::tileXY4bpx(aQuad[3], dataZ);
    Pt2D ct = MercPrj::tileXY4bpx(aQuad[4], dataZ);
    aOut = tileCover(tl,tr, br, bl,ct, dataZ);

    return aOut.size();
}


int getCoveredTilesByBpxBbox(const Box2D& aBpxBBox,
	uint8_t aZ,
	std::vector<TileID>& aOut
	)
{
	if (aBpxBBox.isEmpty())
	{
		return 0;
	}
	int32_t dataZ = getDataZByZoom(aZ);

	auto sw = MercPrj::tileXY4bpx(aBpxBBox.southWest(), dataZ);
	auto ne = MercPrj::tileXY4bpx(aBpxBBox.northEast(), dataZ);
	auto maxTile = std::pow(2.0, dataZ);
	//auto x1 = glm::floor(sw.x);
	//auto x2 = glm::ceil(ne.x) - 1;
	auto x1 = glm::floor(sw.x);
	auto x2 = glm::floor(ne.x);
	x1 = glm::clamp(x1, 0.0, maxTile - 1);
	x2 = glm::clamp(x2, 0.0, maxTile - 1);
	auto y1 = glm::clamp(floor(sw.y), 0.0, maxTile - 1);
	auto y2 = glm::clamp(floor(ne.y), 0.0, maxTile - 1);

	//Frustum frst= Frustum::fromInvProjMatrix( );

	for (int x = x1; x < x2 + 1; x++)
	{
		for (int y = y1; y < y2 + 1; y++)
		{


			aOut.emplace_back(dataZ, x, y);
		}
	}
	return aOut.size();
}


int
getCoveredTileCountbyBpxBox(const Box2D& aBpxBound,
	uint8_t zoom)
{
	if (zoom == 0) {
		return 1;
	}
	auto sw = MercPrj::tileXY4bpx(aBpxBound.southWest(), zoom);
	auto ne = MercPrj::tileXY4bpx(aBpxBound.northEast(), zoom);
	auto maxTile = glm::pow(2.0, zoom);
	auto x1 = glm::floor(sw.x);
	auto x2 = glm::ceil(ne.x) - 1;
	auto y1 = glm::clamp(glm::floor(sw.y), 0.0, maxTile - 1);
	auto y2 = glm::clamp(glm::floor(ne.y), 0.0, maxTile - 1);

	auto dx = x1 > x2 ? (maxTile - x1) + x2 : x2 - x1;
	auto dy = y1 - y2;
	return (dx + 1) * (dy + 1);
}

// Taken from 
// https://github.com/mapbox/sphericalmercator#xyzbbox-zoom-tms_style-srs
// Computes the projected tiles for the lower left and upper right points of the bounds
// and uses that to compute the tile cover count
uint64_t tileCount(const Box2D& aLngLatBound, uint8_t zoom)
{
    if (zoom == 0) {
        return 1;
    }
    LonLat southwest(aLngLatBound.southWest().x, 
                     aLngLatBound.southWest().y);
    LonLat northeast(aLngLatBound.northEast().x, 
                     aLngLatBound.northEast().y);
    auto sw = MercPrj::tileXYFromLngLat(southwest, zoom);
    auto ne = MercPrj::tileXYFromLngLat(northeast, zoom);
    auto maxTile = std::pow(2.0, zoom);
    auto x1 = glm::floor(sw.x);
    auto x2 = glm::ceil(ne.x) - 1;
    auto y1 = glm::clamp(floor(sw.y), 0.0, maxTile - 1);
    auto y2 = glm::clamp(floor(ne.y), 0.0, maxTile - 1);

    auto dx = x1 > x2 ? (maxTile - x1) + x2 : x2 - x1;
    auto dy = y1 - y2;
    return (dx + 1) * (dy + 1);
}

} // namespace util
 
