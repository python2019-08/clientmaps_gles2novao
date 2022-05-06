#pragma once

#include <vector>
#include <memory>
#include <md/util/tc_bound_frustum.hpp>

//#include <md/util/geo.hpp> 

 

namespace clientmap {
class TileID;
class LonLat; // ������
template <typename T> struct SBox2D;// ģ������
using Box2D = SBox2D<double>;// ������

template <typename T> struct SPoint2D;
using Pt2D = SPoint2D<double>;

//---- tileCover functions 
std::vector<TileID>
getCoveredTilesbyLonLatBbox(const Box2D& aLngLatBBox, uint8_t z);

// ��ȡ ��γ���ı��θ��ǵ���Ƭ
int 
getCoveredTilesbyLonLatQuad(const std::array<LonLat,5>& aQuad,
    uint8_t z,
    std::vector<TileID>& aOut 
);
// ------------

int getCoveredTilesByBpxBbox(const Box2D& aBpxBBox,
	uint8_t z,
	std::vector<TileID>& aOut);
int
getCoveredTilesbyBpxQuad(const std::array<Pt2D, 5>& aQuad,
    uint8_t z,
    std::vector<TileID>& aOut
);
 
// Compute only the count of tiles needed for tileCover
uint64_t tileCount(const Box2D& aLngLatBound, uint8_t z);

int
getCoveredTileCountbyBpxBox(const Box2D& aBpxBound,
    uint8_t z);

} // namespace MdU 
