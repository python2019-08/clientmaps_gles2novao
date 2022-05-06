#pragma once
#include <string>
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include "md/util/color.hpp"
#include "md/drawengine/glshaders/glshaderbase.h"
#include <md/drawengine/transform_state.hpp>

namespace clientmap {
//class TransformState;

//
// an OpenGL shader program for background_pattern
//
class GlShdUColor final: public  GlShaderBase 
{
    friend class GLPaintBackground;
    friend class GLPaintDebug;
    friend class GLPaintFace;
    friend class GLPaintLine;
public:
    GlShdUColor();
    virtual ~GlShdUColor();
  
    // glDrawElements
    int drawLineStrip(const TransformState&  aState, 
        float*     aVertArray,
        uint16_t*  aIndices,
        int32_t    aIndexNum,
        const ColorF& aColor );

    int drawLineStrip(
        const TransformState&  aCamera,
        float*     aVertArray,
        int32_t    aVertNum,
        const ColorF& aColor);

	void  draw2dText(const TransformState&  aCamera,
        int _posx,
        int _posy,
        const ColorF&      aColor,
		const std::string& _text
	);

	void draw3dText(const TransformState&  aCamera,
        int aBpxPosx,
        int aBpxPosy,
        const ColorF& aColor,
        const std::string& _text
	);
	 

protected:


    virtual void initLocation()override;
    void enableVertexAttrib()override;
private:
    GLint m_aPosLoc   = -1; 

    GLint m_uMVPMatLoc = -1;
    GLint m_uColorLoc  = -1;
	//
	std::vector<char> m_vertexBuf;
};

}
