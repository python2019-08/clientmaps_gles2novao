#pragma once

#include <md/util/constants.hpp>
#include <md/util/cpp_misc.hpp>

#include <cstdint>
#include <array>
#include <tuple>
#include <set>
#include <vector>
#include <forward_list>
#include <algorithm>
#include <iosfwd>
#include <cassert>

namespace clientmap {
 
// to do: ¸ÄÎª x/y/zË³Ðò
// Has integer z/x/y coordinates
// All tiles must be derived from 0/0/0 (=no tiles outside of the main tile pyramid)
// Used for requesting data; represents data tiles that exist out there.
// z is never larger than the source's maxzoom
class TileID {
public:
    TileID(uint8_t z, uint32_t x, uint32_t y);
    bool operator==(const TileID&) const;
    bool operator!=(const TileID&) const;
    bool operator<(const TileID&) const;
    bool isChildOf(const TileID&) const;
    TileID scaledTo(uint8_t z) const;
    std::array<TileID, 4> children() const;

    uint8_t  z = 0;
    uint32_t x=0;
    uint32_t y=0;
};
 

inline bool 
TileID::operator==(const TileID& rhs) const 
{
    return z == rhs.z && x == rhs.x && y == rhs.y;
}

inline bool 
TileID::operator!=(const TileID& rhs) const 
{
    return z != rhs.z || x != rhs.x || y != rhs.y;
}

inline bool 
TileID::operator<(const TileID& rhs) const 
{
    return std::tie(z, x, y) < std::tie(rhs.z, rhs.x, rhs.y);
}
 
typedef std::set<TileID>     TileIdSet;
typedef std::vector<TileID>  TileIdArray;
//=======================================
class TileIDEx {
public:
    TileIDEx(uint32_t x, uint32_t y, uint8_t z, uint8_t w );
    bool operator==(const TileIDEx&) const;
    bool operator!=(const TileIDEx&) const;
    bool operator<(const TileIDEx&) const;
    TileID tileId() const;

    uint32_t x = 0;
    uint32_t y = 0;
    uint8_t  z = 0;
    uint8_t  w = 0;// layout_zoom
};

inline bool
TileIDEx::operator==(const TileIDEx& rhs) const
{
    return w == rhs.w && z == rhs.z && 
           x == rhs.x && y == rhs.y;
}

inline bool
TileIDEx::operator!=(const TileIDEx& rhs) const
{
    return  w != rhs.w || z != rhs.z || 
            x != rhs.x || y != rhs.y;
}

inline bool
TileIDEx::operator<(const TileIDEx& rhs) const
{
    return std::tie(w, z, x, y) < std::tie(rhs.w, rhs.z, rhs.x, rhs.y);
}

typedef std::set<TileIDEx>     TileIdExSet;
typedef std::vector<TileIDEx>  TileIdExArray;
//---- TileIDEx.w is usd as "ETileType"----
typedef TileIDEx        TypedTileId;
typedef TileIdExSet     TypedTileIdSet;
typedef TileIdExArray   TypedTileIdArray;
//=======================================
} // clientmap 

  
namespace std {

template <>
struct hash<clientmap::TileID>
{
    size_t operator()(const clientmap::TileID& id) const
    {
        std::size_t seed = 0;
        clientmap::hash_combine(seed, id.x);
        clientmap::hash_combine(seed, id.y);
        clientmap::hash_combine(seed, id.z);
        return seed;
    }
}; 
} // namespace std
 
 
    
