// Some structures are defined to represent the results of layouting.
// And these structures  are convinient to used as input parameters 
// of opengl interface functions.
#ifndef  SceneLayer_OBJECT_DEFINITION
#define  SceneLayer_OBJECT_DEFINITION

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
#include "md/drawengine/glpainters/glpainters_obj_def.hpp"
  
namespace clientmap { 

class LayoutPart {
public:
	LayoutPart(std::size_t vertexOffset_,
		std::size_t indexOffset_,
		std::size_t vertexLength_ = 0,
		std::size_t indexLength_ = 0)
		: _vertexOffset(vertexOffset_),
		_indexOffset(indexOffset_),
		_vertexLen(vertexLength_),
		_indexLen(indexLength_)
	{
	}

	// FIXME: clang-tidy-8 still complains here and clang-cl
	// on Windows gives the error: "exception specification of
	// explicitly defaulted move constructor does not match
	// the calculated one" when marking this 'noexcept'.
	// NOLINTNEXTLINE(performance-noexcept-move-constructor)
	LayoutPart(LayoutPart&&) = default;

	std::size_t _vertexOffset;
	std::size_t _indexOffset;

	std::size_t _vertexLen;// vertex count
	std::size_t _indexLen; // index  count
}; 
} //  namespace clientmap

#endif //!SceneLayer_OBJECT_DEFINITION 
