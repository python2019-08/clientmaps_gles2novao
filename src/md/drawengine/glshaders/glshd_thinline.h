#pragma once
#include <string>
#include <vector>
#include <memory>
//#include "glm/glm.hpp"
#include "md/drawengine/glshaders/glshaderbase.h"

namespace clientmap {
//  
// an OpenGL shader program for background_pattern
// GlShdTexUColor    
class GlShdThinLine final: public  GlShaderBase 
{
    friend class GLPaintLine;
public:
    GlShdThinLine();
    virtual ~GlShdThinLine();
   
protected:
    virtual void initLocation()override;
    virtual void enableVertexAttrib()override;
private:
    GLint m_aPosLoc = -1;
    GLint m_aUVLoc = -1;

    GLint m_uMVPMatLoc = -1;
    GLint m_uColorLoc = -1;
};

}
