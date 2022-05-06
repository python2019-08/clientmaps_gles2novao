 
#include <md/util/math_tools.hpp>
#include <md/util/merc_prj.hpp>
#include <md/drawengine/transform_state.hpp>

#include <md/mapengine/tile/vec_tile.hpp>
#include <md/mapengine/tile/raster_tile_source.hpp>
 

namespace clientmap {

RasterTileSource::RasterTileSource( )
{
 
}

RasterTileSource::~RasterTileSource() = default;

bool RasterTileSource::onViewChange(const TileIdSet & aCurTiles,
    double aZoom
)
{
    
    
    return false;
}

} // namespace clientmap
