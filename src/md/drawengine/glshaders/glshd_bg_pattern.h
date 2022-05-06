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
class GlShdBgPattern : public  GlShaderBase 
{
public:
    GlShdBgPattern();
    virtual ~GlShdBgPattern();
  
protected:
    virtual void initLocation()override;
};

}
