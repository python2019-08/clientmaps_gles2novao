#pragma once
#include <cmath>
#include <algorithm> // std::min 
#include <stdexcept>
#include <vector>
#include <array>
#include <md/util/constants.hpp>
#include <md/util/geom.hpp>
// 因为这个接口文件可能会向外提供，所以不用glm中的类或结构体（glm太大了）。
namespace clientmap {
//===============================================================
class TileID;

class LonLat {
public:
    enum WrapMode : bool { Unwrapped, Wrapped };

    LonLat(double lon_ = 0, 
           double lat_ = 0,
           bool isWrapped=false)
        :  mLon(lon_), mLat(lat_)
    {
        if (std::isnan(mLat)) {
            //throw std::domain_error("latitude must not be NaN");
            mLat = 0;
        }
        if (std::isnan(mLon)) {
            //throw std::domain_error("longitude must not be NaN");
            mLon = 0;
        }
        if (std::abs(mLat) > 90.0) {
            //throw std::domain_error("latitude must be between -90 and 90");
            mLat = 90;
        }
        if (!std::isfinite(mLon)) {
            //throw std::domain_error("longitude must not be infinite");
            mLon = 0;
        }
        if (isWrapped) {
            wrap();
        }
    }

    double lat() const { return mLat; }
    double lng() const { return mLon; }

    LonLat wrapped() const { 
        return {mLon, mLat,  Wrapped }; 
    }

    void wrap();

    // If the distance from start to end longitudes is between half and full
    // world, unwrap the start longitude to ensure the shortest path is taken.
    void unwrapForShortestPath(const LonLat& end);

    // Constructs a LonLat object with the top left position
    // of the specified tile.
    LonLat(const TileID& id); 

    friend bool operator==( const LonLat& a, 
                            const LonLat& b) 
    {
        return a.mLat == b.mLat && 
               a.mLon == b.mLon;
    }

    friend bool operator!=( const LonLat& a, 
                            const LonLat& b) 
    {
        return !(a == b);
    }
public:
    double mLon;//longitude
    double mLat;//latitude
}; 

} // MdU//namespace md
