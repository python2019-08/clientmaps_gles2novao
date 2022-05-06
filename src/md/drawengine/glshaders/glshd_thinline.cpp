#include <md/util/logtool.hpp>

#include "md/drawengine/glshaders/glshd_thinline.h"
 
namespace clientmap {

GlShdThinLine::GlShdThinLine()
{
    //================================================
	// Uncompressed source of line.vertex.glsl:
    //================================================
    m_vertShaderSource = R"RAW_GLSL(
        uniform mat4   uMVPMat;         
        attribute vec4 aPos; 
        attribute vec2 aUV; 
        varying vec2   vUV; 
        void main() { 
            gl_Position = uMVPMat * aPos; 
            vUV      = aUV; 
            gl_PointSize = 2.0;
        }; 
    )RAW_GLSL";
    //================================================
	// Uncompressed source of line.fragment.glsl:
    //================================================
    m_fragShaderSource = R"RAW_GLSL(
        #version 120
		precision mediump float;
        uniform vec4      uColor; 
        uniform sampler2D uTexture0; 
        varying vec2      vUV; 
        void gaussFilter(in float _texT,inout float _newA) {   
              float  n =64.0; 
              float  idxMin = 0.0; 
              float  idxMax = (n - 1.0) * 0.5; 
              float  idx    = (n - 1.0) * _texT; 
              float xMin = idxMin - (n-1.0)*0.5;  
              float xMax = idxMax - (n-1.0)*0.5;  
              float x    = idx    - (n-1.0)*0.5;  
              float sigmaX = ((n-1.0)*0.5 - 1.0)*0.3 + 0.8; 
              float scale2X = -0.5/(sigmaX * sigmaX); 
              float tMin = exp(scale2X * xMin *xMin); 
              float tMax = exp(scale2X * xMax *xMax); 
              float t    = exp(scale2X * x    *x ); 
              _newA = (t -tMin) / (tMax - tMin); 
        } 
        void main() { 
            vec4 texClr = texture2D(uTexture0, vUV);
            float t1 = 0.0;  
            gaussFilter( vUV.t, t1);    
            gl_FragColor = vec4(uColor.rgb, texClr.a *t1 ); 
        }   
    )RAW_GLSL";

    // An optional shader description printed on compile failure
    m_description = "GlShdThinLine";
}

GlShdThinLine::~GlShdThinLine()
{ 
     
}

// virtual function 
void GlShdThinLine:: initLocation() 
{
    //---get attribute locations
    m_aPosLoc = MDGL_CHECK(md::glGetAttribLocation(m_progId, "aPos"));
    m_aUVLoc = MDGL_CHECK(md::glGetAttribLocation(m_progId, "aUV"));

    //---get uniform locations
    m_uMVPMatLoc = MDGL_CHECK(md::glGetUniformLocation(m_progId, "uMVPMat"));
    m_uColorLoc = MDGL_CHECK(md::glGetUniformLocation(m_progId, "uColor"));
}
void GlShdThinLine::enableVertexAttrib()
{
  


}

}// namespace clientmap {
