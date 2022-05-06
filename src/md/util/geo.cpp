#include <cmath> 
#include <glm/glm.hpp>

#include <md/util/constants.hpp>
#include <md/util/math_tools.hpp> 
#include <md/util/tile_id.hpp>

#include <md/util/geo.hpp>
 

namespace clientmap {
//------------------------------------------------------------
namespace {
// TileIdY
double lat_(const uint8_t z, const int64_t aTileIdY)
{
    const double n = M_PI - 2.0 * M_PI * aTileIdY / std::pow(2.0, z);
    return RAD2DEG * std::atan(0.5 * (std::exp(n) - std::exp(-n)));
}

double lon_(const uint8_t z, const int64_t aTileIdX) {
    return aTileIdX / std::pow(2.0, z) * DEGREES_MAX 
          - LONGITUDE_MAX;
}

} // end namespace

void LonLat::wrap() 
{
    mLon = cm_wrap(mLon, -LONGITUDE_MAX, LONGITUDE_MAX);
}

// If the distance from start to end longitudes is between half and full
// world, unwrap the start longitude to ensure the shortest path is taken.
void LonLat::unwrapForShortestPath(const LonLat& end) 
{
    const double delta = std::abs(end.mLon - mLon);

    if (delta <= LONGITUDE_MAX ||
        delta >= DEGREES_MAX)
        return;

    if (mLon > 0 && end.mLon < 0)
        mLon -= DEGREES_MAX;
    else if (mLon < 0 && end.mLon > 0)
        mLon += DEGREES_MAX;
}

LonLat::LonLat(const TileID& id) 
    : mLon(lon_(id.z, id.x)) ,mLat(lat_(id.z, id.y))
{
}
 
} // MdU//end namespace mbgl
