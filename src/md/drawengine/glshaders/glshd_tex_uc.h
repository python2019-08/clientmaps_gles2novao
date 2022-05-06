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
class GlShdTexUColor final: public  GlShaderBase 
{
	friend class GLPaintBackground;
	friend class GLPaintLine;
    friend class GLPaintBuilding;
public:
    GlShdTexUColor();
    virtual ~GlShdTexUColor();
  
    
protected:
    virtual void initLocation()override;
    void enableVertexAttrib()override;
private:
    GLint m_aPosLoc = -1;
    GLint m_aUVLoc  = -1;

    GLint m_uMVPMatLoc = -1;
    GLint m_uColorLoc  = -1;
};

}
