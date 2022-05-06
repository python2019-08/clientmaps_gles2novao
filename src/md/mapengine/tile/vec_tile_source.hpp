#pragma once
 

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include <glm/glm.hpp>
#include <md/util/geo.hpp> 
#include "md/util/lru_cache.h"
#include <md/mapengine/tile/response.hpp>
#include "md/mapengine/style/style4map.hpp"  

namespace clientmap {
 
class CMsgQWorker;
class VectTile; 

class LayoutVecTile;
using ShLayoutVT = std::shared_ptr<LayoutVecTile>;
using ShLayoutVTArray = std::vector<ShLayoutVT>;
 
class CDrawEngine;
class GlPainters;
class CPaintContext;

class VecTileSource {
public:
    VecTileSource( );
    ~VecTileSource( ); 
     

	bool onViewChange(const TileIdSet & aCurrentTiles,
        double aLayoutZoom,
        std::shared_ptr<Style4Map> aMapStyle
        );

    void onDataRecv(ShVoid aTileData, 		
		std::shared_ptr<Style4Map> aMapStyle);
    
	bool drawStyleLayer(
		const GlPainters& aPainter,
		const CPaintContext& aDC,
		ShStyleLayer aMapStyle);

private:
    bool _updateDrawTile(const TileIdSet& aCurTiles, double aZoom);
	static bool twoDrawTileArray_isDiff(
		std::shared_ptr<ShLayoutVTArray> aL,
		std::shared_ptr<ShLayoutVTArray> aR);



 
	double  mCurZoom; // set in this->onViewChange()

    LruCache<TileID,   ShVectTile>  m_dataCache;  // half extrusion result
    LruCache<TileIDEx, ShLayoutVT>  m_layoutCache;// extrusion result
    
    std::mutex   m_swapCS;
	std::shared_ptr<ShLayoutVTArray>    m_layoutToDraw;
};
  
} // namespace clientmap
