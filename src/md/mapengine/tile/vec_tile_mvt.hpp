// mapbox mvt parser
#ifndef  MAPBOX_VECT_TILE_HPP_INCLUDED
#define  MAPBOX_VECT_TILE_HPP_INCLUDED

#include <md/mapengine/tile/vec_tile.hpp>

namespace mapbox {namespace vector_tile {
class feature;
}}

namespace clientmap {
class MapboxVectTile {    
public:
    MapboxVectTile( );
    ~MapboxVectTile( );     

public: 
    static ShVectTile parse(const TileID& aID,
        const std::string&  aTileRawData);
private:
    //--------------------------------------------- 

	static ShVecTileLayer getTileLayer(ShVectTile  aVT, 
		const std::string& aLayerId,  FeatureType aType);
    static ShGeomTileFeature
        parseFeatureGeom(const mapbox::vector_tile::feature& aMbFt);
	

    static GeomCollection fixupPolygons(const GeomCollection& rings);
    // classifies an array of rings into polygons with outer rings and holes
    static std::vector<InTilePolygon> rings2polygons(const GeomCollection&);

	// Truncate polygon to the largest `maxHoles` inner rings by area.
	static void limitHoles(GeomCollection&, uint32_t maxHoles); 

};
//------------------------------------------ 
} // namespace clientmap
#endif // ! MAPBOX_VECT_TILE_HPP_INCLUDED