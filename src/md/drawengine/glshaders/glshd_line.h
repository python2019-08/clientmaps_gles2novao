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
class GlShdLine final: public  GlShaderBase 
{
public:
    GlShdLine();
    virtual ~GlShdLine();
   
protected:
    virtual void initLocation()override;
    virtual void enableVertexAttrib()override;
private:
    //using LineLayoutAttributes = TypeList<
    //    attributes::pos_normal, attributes::data<uint8_t, 4>>;
    GLint m_a_pos_normalLoc = -1;
    GLint m_a_dataLoc = -1;
    //TypeList<
    //    uniforms::matrix,
    //    uniforms::ratio,
    //    uniforms::units_to_pixels,
    //    uniforms::device_pixel_ratio>,
    GLint m_u_matrixLoc = -1;
    GLint m_u_ratioLoc  = -1;
    GLint m_u_units_to_pixelsLoc    = -1;
    GLint m_u_device_pixel_ratioLoc  = -1;
};

}
