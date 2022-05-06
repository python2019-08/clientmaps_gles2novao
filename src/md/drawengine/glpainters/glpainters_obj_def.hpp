// Some structures are defined for being conviniently used as input
// parameters of opengl interface functions .
#ifndef  GL_PAINTERS_OBJECT_DEFINITION
#define  GL_PAINTERS_OBJECT_DEFINITION

#include <cstdint>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept> 
   
#include <glm/glm.hpp>
//#include <md/util/tile_id.hpp>
//#include <md/util/constants.hpp> 
//#include <md/util/geo.hpp>  
  
namespace clientmap {

//===================================================
// structures used for layout
//===================================================
class XyUv
{
public:

    XyUv(float aX = 0.0, float aY = 0.0, 
		 float aU = 0.0, float aV = 0.0)
        :_x(aX), _y(aY), _u(aU), _v(aV)
    {	}

    float _x;
    float _y;

    float _u;
    float _v;
};
static_assert(sizeof(XyUv) < 256, 
    "vertex type must be smaller than 256 bytes");
static_assert(std::is_standard_layout<XyUv>::value, 
    "vertex type must use standard layout");
using  TriStrip2DPart = std::vector<XyUv>;
//---------------
class XyzUv
{
public:

    XyzUv(float aX = 0.0, float aY = 0.0, float aZ = 0.0,
        float aU = 0.0, float aV = 0.0)
        :_x(aX), _y(aY), _z(aZ), _u(aU), _v(aV)
    {	}

    float _x;
    float _y;
    float _z;

    float _u;
    float _v;
};
static_assert(sizeof(XyzUv) < 256,
    "vertex type must be smaller than 256 bytes");
static_assert(std::is_standard_layout<XyzUv>::value,
    "vertex type must use standard layout");
using  TriStrip3DPart = std::vector<XyzUv>;

//---------------
class GLLinesIndex {
public:
	GLLinesIndex(uint16_t aPt0Idx, uint16_t aPt1Idx)
		: _pt0(aPt0Idx), _pt1(aPt1Idx)
	{
	}
	uint16_t _pt0 = 0;
	uint16_t _pt1 = 1;
};


class GLTrianglesIndex {
public:
	GLTrianglesIndex(uint16_t aPt0Idx, 
        uint16_t aPt1Idx, 
        uint16_t aPt2Idx)
		: _pt0(aPt0Idx), 
        _pt1(aPt1Idx), 
        _pt2(aPt2Idx)
	{
	}
	uint16_t _pt0 = 0;
	uint16_t _pt1 = 1;
	uint16_t _pt2 = 2;
}; 
  
} //  namespace clientmap

#endif //!GL_PAINTERS_OBJECT_DEFINITION
