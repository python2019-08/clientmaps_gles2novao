#include <sstream>
#include <iostream>
#include <chrono> 

#include <md/util/logtool.hpp>
#include <md/util/math_tools.hpp> 
#include <md/util/merc_prj.hpp> 
#include <md/util/geom.hpp> 

#include "md/md_global.h" 
#include <md/mapengine/tile/vec_tile_mvt.hpp>

#include <md/mapengine/tile/vec_tile.hpp>
  

using namespace clientmap;

//--------------------------------------------------
// class VectTile 
//--------------------------------------------------
VectTile::VectTile(const TileID& aID)
    :m_tileId(aID)
{ 
}

VectTile::~VectTile() = default;
 

ShVectTile VectTile::doParse(const TileID& aID,
	const std::string&  aTileRawData)
{ 
	ShVectTile  retTile = MapboxVectTile::parse( aID, aTileRawData);

	//layoutPolygonLayers(retTile);

	return retTile;
}


 
 
 
