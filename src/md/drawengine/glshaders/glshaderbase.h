#pragma once
#include <string>
#include <vector>
#include <memory>
//#include "glm/glm.hpp"
#include "platformbase/gl_interface.h"
 

namespace clientmap {
 
//
// GlShaderBase - base class representing an OpenGL shader program
//
class GlShaderBase 
{
public:
    GlShaderBase();
    virtual ~GlShaderBase();

    GlShaderBase(const GlShaderBase &) = delete;
    GlShaderBase& operator=(const GlShaderBase &) = delete;
 
    // Bind the program in OpenGL if it is not already bound; If the
    // shader sources have been modified since the last time build() 
    // was called, also calls build().
    // Returns true if shader can be used (i.e. is valid).
    bool use();

    /*virtual int draw() {}*/
protected:
    // Return true if this object represents a valid OpenGL shader program.
    bool isValid() const 
    {
        return (  (m_progId != 0)&& 
            MDGL_CHECK(md::glIsProgram(m_progId) ) 
            );
    };
    // Apply all source blocks to the source strings for this shader
    // and attempt to compile and then link the resulting vertex and 
    // fragment shaders; if compiling or linking fails this prints the 
    // compiler log, returns false, and keeps the program's previous 
    // state; if successful it returns true.
    bool build();
    static GLuint makeLinkedShaderProgram(GLint aFragShaderId, 
                                          GLint aVertShaderId);
    //
    static GLuint makeCompiledShader(const std::string& _src, GLenum _type);
    //
    virtual void initLocation() {};
    virtual void enableVertexAttrib() {}

    void destroy();
protected: // ---- data members----

    GLuint m_progId = 0;// gl shader program Id
    GLuint m_fsId = 0; // GLSL  fragmentShader id
    GLuint m_vsId   = 0;//glsl VertexShader id
 
    std::string m_vertShaderSource;//vertex Shader Source
    std::string m_fragShaderSource; // fragment Shader Source    

    // An optional shader description printed on compile failure
    std::string m_description="GlShaderBase";
};

}// namespace clientmap {
