#ifndef GL_SHADER__ALPHA_TEXTURE__UNI_COLOR_H_INCLUDED
#define GL_SHADER__ALPHA_TEXTURE__UNI_COLOR_H_INCLUDED


#pragma once
#include <string>
#include <vector>
#include <memory>
//#include "glm/glm.hpp"
#include "md/drawengine/glshaders/glshaderbase.h"

namespace clientmap {
//
// an OpenGL shader program for background_pattern
//
class GlShdAlphaTexUColor final: public  GlShaderBase 
{
	friend class GLPaintLine;
public:
    GlShdAlphaTexUColor();
    virtual ~GlShdAlphaTexUColor();
  
    
protected:
    virtual void initLocation()override;
    void enableVertexAttrib()override;
private:
    GLint m_aPosLoc = -1;
    GLint m_aUVLoc  = -1;

    GLint m_uMVPMatLoc = -1;
    GLint m_uColorLoc  = -1;
};

}//namespace clientmap {
#endif // !GL_SHADER__ALPHA_TEXTURE__UNI_COLOR_H_INCLUDED
