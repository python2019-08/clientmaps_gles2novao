#pragma once
 

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
 

#include <md/util/geo.hpp> 
#include <md/drawengine/transform_state.hpp>

namespace clientmap {
class VectTile;
//class TransformState;
class CMsgQWorker;

class RasterTileSource {
public:
    RasterTileSource( );
    ~RasterTileSource(); 

    bool onViewChange(const TileIdSet & aCurrentTiles,
        double aLayoutZoom
    );
	 
     
private:  
};
  
} // namespace clientmap
