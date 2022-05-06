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
#include <md/util/tile_id.hpp>

namespace clientmap {
  
TileID::TileID(uint8_t z_, uint32_t x_, uint32_t y_) 
	: z(z_), x(x_), y(y_) 
{
    assert(z >= 0 && z <= 22);
    assert(x < (1ull << z));
    assert(y < (1ull << z));
} 

bool 
TileID::isChildOf(const TileID& parent) const 
{
    // We're first testing for z == 0, to avoid a 32 bit shift, which is undefined.
    return parent.z == 0 ||
           (parent.z < z && 
			parent.x == (x >> (z - parent.z)) && 
			parent.y == (y >> (z - parent.z))
			);
}

TileID 
TileID::scaledTo(uint8_t targetZ) const 
{
    if (targetZ <= z) {
        return { targetZ, 
                 x >> (z - targetZ), 
                 y >> (z - targetZ) }; // parent or same
    } else {
        return { targetZ, 
                 x << (targetZ - z), 
                 y << (targetZ - z) }; // child
    }
}

std::array<TileID, 4> 
TileID::children() const 
{
    const uint8_t childZ = z + 1;
    const uint32_t childX = x * 2;
    const uint32_t childY = y * 2;
    return { {
        { childZ, childX, childY },
        { childZ, childX, childY + 1 },
        { childZ, childX + 1, childY },
        { childZ, childX + 1, childY + 1 },
    } };
}
//================================================

TileIDEx::TileIDEx(uint32_t x_, uint32_t y_, uint8_t z_, uint8_t w_)
    : x(x_), y(y_), z(z_), w(w_)
{
    assert(z >= 0 && z <= 220);
    assert(z >= 0 && z <= 22);
    assert(x < (1ull << z));
    assert(y < (1ull << z));
}

TileID TileIDEx::tileId() const
{
    return {z,x,y};
}
 



} // clientmap 
 
 
 
    
