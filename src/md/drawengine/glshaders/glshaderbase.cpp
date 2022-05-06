//#include "glcanvas/renderState.h"
#include <md/util/logtool.hpp>

#include "md/drawengine/glshaders/glshaderbase.h"
 
namespace clientmap {

GlShaderBase::GlShaderBase()
{
    // Nothing to do.
}

GlShaderBase::~GlShaderBase()
{ 
    destroy();
}

void GlShaderBase::destroy()
{
    // Delete handle for old program and shaders.
    if (m_progId && 
        MDGL_CHECK(md::glIsProgram(m_progId)))
    {
        MDGL_CHECK( md::glDeleteProgram(m_progId) );
    }
    m_progId = 0;

    if (m_fsId && 
        MDGL_CHECK(md::glIsShader(m_fsId)) 
        ) 
    {
        MDGL_CHECK(md::glDeleteShader(m_fsId));
    }
    m_fsId = 0;


    if (m_vsId && 
        MDGL_CHECK(md::glIsShader(m_vsId))
        ) 
    {
        MDGL_CHECK(md::glDeleteShader(m_vsId));
    }
    m_vsId = 0;     
}

//GLint GlShaderBase::getAttribLocation(const std::string& _attribName) 
//{ 
//    // If this is a new entry, get the actual location from OpenGL.
//    GLint location = MDGL_CHECK(
//     md::glGetAttribLocation(m_progId, _attribName.c_str()) );
//}
//
//GLint GlShaderBase::getUniformLocation(const UniformLocation& _uniform) 
//{
//
//    if (_uniform.location == -2) {
//        _uniform.location = md::glGetUniformLocation(m_progId, _uniform.name.c_str());
//    }
//    return _uniform.location;
//}

bool GlShaderBase::use( ) 
{
    if ( !isValid() ) {
        build( );    
        initLocation();
    }
 
    if (isValid()) {
        MDGL_CHECK( md::glUseProgram(m_progId) );//rs.shaderProgram(m_progId);

        enableVertexAttrib();
        return true;
    }

    return false;
}

bool GlShaderBase::build(  )
{
    if (isValid()) {
        return true; 
    } 

    // Delete handle for old program and shaders.
    destroy();
    //
    auto& vertSrc = m_vertShaderSource;
    auto& fragSrc = m_fragShaderSource;
    if (vertSrc.empty() || fragSrc.empty())
    {
        return false;
    }

    // Compile vertex and fragment shaders
    GLint vertexShader = makeCompiledShader( vertSrc, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
         Log::Error( "vert Shader compilation failed for %s", 
             m_description.c_str() );
        return false;
    }

    GLint fragmentShader = makeCompiledShader( fragSrc, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
         Log::Error( "frag Shader compilation failed for %s", 
             m_description.c_str());
        return false;
    }

    // Link shaders into a program
    GLint programId = makeLinkedShaderProgram(fragmentShader, vertexShader);
    if (programId == 0) {
         Log::Error( "Shader Linked failed for %s",
             m_description.c_str());
        return false;
    }
     

    m_progId = programId;
    m_fsId = fragmentShader;
    m_vsId = vertexShader;
  
    return true;
}

GLuint 
GlShaderBase::makeLinkedShaderProgram(GLint aFragShaderId, 
                                      GLint aVertShaderId ) 
{
    GLuint programId = 0;
    do
    {
        programId = MDGL_CHECK(md::glCreateProgram());
        if (0 == programId)
        {
            break;// failed
        }

        MDGL_CHECK(md::glAttachShader(programId, aFragShaderId) );
        MDGL_CHECK(md::glAttachShader(programId, aVertShaderId) );
        MDGL_CHECK(md::glLinkProgram(programId) );

        GLint isLinked;
        MDGL_CHECK(md::glGetProgramiv(programId, GL_LINK_STATUS, &isLinked) );
        if (isLinked )
        {
            break;// Link successfully
        }

        // if Link error
        GLint infoLen = 0;
        MDGL_CHECK(md::glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLen));
        if (infoLen > 1)
        {
            std::vector<GLchar> infoLog(infoLen);
            MDGL_CHECK(md::glGetProgramInfoLog(programId, infoLen,
                NULL, &infoLog[0]) );
            Log::Error(  "linking program:\n%s", &infoLog[0]);

             assert(0);
        }
        MDGL_CHECK(md::glDeleteProgram(programId));
        programId = 0;
        
    } while (0);    

    return programId;
}
// @_type   GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
GLuint GlShaderBase::makeCompiledShader( const std::string& _src, 
                                         GLenum _type) 
{
    GLuint shaderId = 0;
    do
    {
        shaderId = MDGL_CHECK(md::glCreateShader(_type));
        if (0 == shaderId) {
            break; // error
        }
        //----
        const GLchar* source = (const GLchar*)_src.c_str();
        MDGL_CHECK(md::glShaderSource(shaderId, 1, &source, NULL));
        MDGL_CHECK(md::glCompileShader(shaderId) );

        GLint isCompiled;
        MDGL_CHECK(md::glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled) );
        if (isCompiled )
        {
            break; // ok 
        }

        //---- glCompileShader failed
        GLint infoLen = 0;
        MDGL_CHECK(md::glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLen));
        if (infoLen > 0)
        {
            //std::string infoLog;
            //infoLog.resize(infoLen);
            //md::glGetShaderInfoLog(shaderId, infoLen, NULL,
            //                       static_cast<GLchar*>(&infoLog[0]));
            // Log::Error( Event::Shader,
            // "Shader compilation failed\n%s", infoLog.c_str());
            const auto infoLog = std::make_unique<GLchar[]>(infoLen);
            MDGL_CHECK(md::glGetShaderInfoLog(shaderId, infoLen, NULL, 
                                              infoLog.get()));
             Log::Error( "Shader compilation failed\n%s", infoLog.get());
             assert(0);
        }
        MDGL_CHECK(md::glDeleteShader(shaderId));
        shaderId =  0;
    } while (0);
    
    return shaderId;
}
 
}//namespace clientmap {
