//
// LayoutVTLayer  && LayoutVecTile
#ifndef  VECT_TILE_LAYOUT_HPP_INCLUDED
#define VECT_TILE_LAYOUT_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <md/util/tile_id.hpp>

#include <md/util/geo.hpp> 
#include <md/mapengine/scenelayer/scenelayer.hpp>

namespace clientmap {
class Style4Map;
using ShStyle4Map = std::shared_ptr<Style4Map>;
 
//===================================================
//  LayoutVecTile
//===================================================
class LayoutVecTile;
using ShLayoutVT = std::shared_ptr<LayoutVecTile>;

class LayoutVecTile {
public:
    // layout aVT, and return a ShLayoutVT .
	static ShLayoutVT  doLayout(ShVectTile  aVT,
		float        aLayoutZoom,
		ShStyle4Map  aMapStyle);

public:
    LayoutVecTile(const TileIDEx& aID);
    ~LayoutVecTile();

    const TileIDEx& tileIdEx() {
        return m_layoutTileId;
    }

    int draw(const GlPainters& aPainter,
        const CPaintContext& aDC ,
        const std::string& aStyleLayerId);
private: 	 
	static ShLayoutVTLayer getLayoutTileLayer(ShLayoutVT  aVT,
		const std::string& aLayerId, VecTileLayerType aType); 

    bool needDraw(const CPaintContext& aPC);
private:
    TileIDEx   m_layoutTileId; 

	std::map<std::string, ShLayoutVTLayer > m_Id2layer;
};  
using ShLayoutVTArray = std::vector<ShLayoutVT>;
} // namespace clientmap
#endif // ! VECT_TILE_LAYOUT_HPP_INCLUDED