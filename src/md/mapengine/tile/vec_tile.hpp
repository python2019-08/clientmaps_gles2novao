#ifndef  VECT_TILE_HPP_INCLUDED
#define  VECT_TILE_HPP_INCLUDED
  
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <map> 
#include <set>
#include <unordered_map>  
#include <assert.h>
#include <glm/glm.hpp>
#include "md/util/tile_id.hpp"

#include "md/util/geo.hpp" 
#include "md/mapengine/scenelayer/scenelayer_obj_def.hpp"

namespace clientmap {

// Normalized vector tile coordinates.
// Each geometry coordinate represents a point in a bidimensional space,
// varying from -V...0...+V, where V is the maximum extent applicable. 
using InTilePt = glm::i16vec2;//<int16_t>;
using InTilePts = std::vector<InTilePt>;// linestring or polygon
    
using InTileLineStr  = InTilePts;// linestring
using InTileMultiLineStr = std::vector <InTileLineStr>;// multi-linestring

using InTileLineRing = InTilePts;// linestring
using InTilePolygon = std::vector<InTileLineRing>;
using InTileMultiPolygon = std::vector<InTilePolygon>;

using GeomCollection = std::vector<InTileLineRing>;//==InTilePolygon  

//--
enum class FeatureType : uint8_t {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3,
    Building = 4,
    Overlay = 5
};
//------
enum class PropValueType : uint8_t {
    eBool = 0,
    eU64 = 1,
    eI64 = 2,
    eD = 3,
    eStr = 4
};

struct PropValue
{
    PropValueType _valType = PropValueType::eStr;
    union {
        uint64_t _u64 = 0;
        int64_t  _i64;
        double   _d;
        bool     _b;
    };
    std::string _str;
};

using PropertyMap = std::unordered_map<std::string, PropValue>;

//-------------------------
class VecTileFeature
{
public:
    VecTileFeature() {}

    uint64_t        _ftId = (uint64_t)-1;// 0XFFFFFFFFFFFFFFFF;
    FeatureType     _ftType = FeatureType::Polygon;

    InTilePts            _points;
    InTileMultiLineStr   _lines;
	// polygon 
    InTileMultiPolygon   _polygons;
    float                _buildingHeight;
	// todo: a triangulated  polygon£¨m_vertices.size()must less 
	//       than 65535£© ......
	std::vector<glm::vec2>        m_vertices;
	std::vector<GLTrianglesIndex> m_triangles;// triangles  Index  Array 

	//
    PropertyMap _props;
};
using ShGeomTileFeature = std::shared_ptr<VecTileFeature>;
 
//-------------------------
using VecTileLayerType = FeatureType;
class VecTileLayer {
public:
    VecTileLayer(const std::string& aLayerId,
		FeatureType  aType)
		: _id(aLayerId), _type(aType)
    {

    }

    std::string      _id; // layer name 
	VecTileLayerType _type = VecTileLayerType::Polygon;// layer type

	std::vector<ShGeomTileFeature> _features;
};
using ShVecTileLayer =std::shared_ptr<VecTileLayer>;
//-------------------------
class VectTile;
using ShVectTile = std::shared_ptr<VectTile>;

class VectTile {    
public:
    VectTile(const TileID& aID);
    ~VectTile(); 
     
    TileID     m_tileId; 

    std::map<std::string, ShVecTileLayer > m_Id2layer; 

public:
    //--------------------------------------------- 
    //  doParse()  
    //--------------------------------------------- 
    static ShVectTile doParse(const TileID& aID,
        const std::string&  aTileRawData);
 

};
//------------------------------------------ 
} // namespace clientmap
#endif // ! VECT_TILE_HPP_INCLUDED