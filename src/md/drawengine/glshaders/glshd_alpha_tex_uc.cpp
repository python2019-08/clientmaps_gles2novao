//#include "glcanvas/renderState.h"
#include <md/util/logtool.hpp>

#include "md/drawengine/glshaders/glshd_alpha_tex_uc.h"
 
namespace clientmap {

GlShdAlphaTexUColor::GlShdAlphaTexUColor()
{
    //================================================
    // source of line.vertex.glsl:
    //================================================
    m_vertShaderSource = R"RAW_GLSL(
        uniform mat4   uMVPMat;         
        attribute vec4 aPos; 
        attribute vec2 aUV; 
        varying vec2   vUV; 
        void main() { 
            gl_Position = uMVPMat * aPos; 
            vUV      = aUV; 
        };
    )RAW_GLSL";
    //================================================
    // source of line.fragment.glsl:
    //================================================
    m_fragShaderSource = R"RAW_GLSL(
		precision mediump float;
        uniform vec4      uColor; 
        uniform sampler2D uTexture0; 
        varying vec2      vUV; 
        void main() { 
			vec4  texClr = texture2D(uTexture0, vUV);
			gl_FragColor = vec4(uColor.rgb,uColor.a*texClr.a);          
        } 
    )RAW_GLSL";

    // An optional shader description printed on compile failure
    m_description = "GlShd_alpha_tex_uc";
}

GlShdAlphaTexUColor::~GlShdAlphaTexUColor()
{ 
     
}

// virtual function 
void GlShdAlphaTexUColor:: initLocation() 
{
    //---get attribute locations
    m_aPosLoc = MDGL_CHECK(md::glGetAttribLocation(m_progId, "aPos"));
    m_aUVLoc  = MDGL_CHECK(md::glGetAttribLocation(m_progId, "aUV"));

    //---get uniform locations
    m_uMVPMatLoc= MDGL_CHECK(md::glGetUniformLocation(m_progId, "uMVPMat"));
    m_uColorLoc = MDGL_CHECK(md::glGetUniformLocation(m_progId, "uColor"));
} 

void GlShdAlphaTexUColor::enableVertexAttrib()
{ 
    //md::glEnableVertexAttribArray(m_aPosLoc);
    //md::glEnableVertexAttribArray(m_aUVLoc);
}

}// namespace clientmap {
