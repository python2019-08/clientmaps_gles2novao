#ifndef OPENGL_INTERFACE__H
#define OPENGL_INTERFACE__H

#if defined(OsID_ANDROID) || defined(OsID_QNX)
    // #define GL_GLEXT_PROTOTYPES
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#elif defined(OsID_IOS)
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif defined(OsID_MAC)    // not used
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>
#elif defined(OsID_LINUX) // not used
#   if defined(SWDevFW_QT)   
    #include <QtGui/QOpenGLFunctions>
#   else 
    #define GL_GLEXT_PROTOTYPES 1
    #ifndef MD_USE_GLES2
    #include <GL/gl.h>
    #include <GL/glext.h>
    #else
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #endif
#   endif
#elif defined(OsID_WINDOWS) 
#   if defined(SWDevFW_QT)   
#   include <QtGui/QOpenGLFunctions>
#   else 
#   include "gl/glew.h"
#   include "gl/gl.h"
#   endif 
#else
    #error "unknown platform!"
#endif

//#ifndef NDEBUG  //#ifdef DEBUG 
//#define GL_CHECK(cmd) do { cmd; md::glCheckError(#cmd, __FILE__, __LINE__); } while (0)
//#else
//#define GL_CHECK(cmd) do { cmd; } while (0)
//#endif

#ifndef NDEBUG
#define MDGL_CHECK(cmd) ([&]() { struct TCHECK_ERROR { \
     ~TCHECK_ERROR() noexcept(false) { \
       md::glCheckError(#cmd, __FILE__, __LINE__); \
     } \
  } _varCheckError; \
  return cmd; }())
#else
#define MDGL_CHECK(cmd) (cmd)
#endif

namespace md { 

//struct GL  {
    GLenum glGetError(void);
    const GLubyte* glGetString(GLenum name);

    void glClearColor(GLclampf red, GLclampf green,
                    GLclampf blue, GLclampf alpha);
    void glClear(GLbitfield mask);

    void glLineWidth(GLfloat width);
    void glViewport( GLint x,  GLint y, 
                     GLsizei width,  GLsizei height);

    void glEnable(GLenum);
    void glDisable(GLenum);
    void glDepthFunc(GLenum func);
    void glDepthMask(GLboolean flag);
    void glDepthRangef(GLfloat n, GLfloat f);
    void glClearDepthf(GLfloat d);
    void glBlendFunc(GLenum sfactor, GLenum dfactor);
    void glStencilFunc(GLenum func, GLint ref, GLuint mask);
    void glStencilMask(GLuint mask);
    void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass);
    void glClearStencil(GLint s);
    void glColorMask(GLboolean red, GLboolean green,
                     GLboolean blue, GLboolean alpha);
    void glCullFace(GLenum mode);
    void glFrontFace(GLenum mode);
    
    void glGetIntegerv(GLenum pname, GLint *params);

    // Program
    void glUseProgram(GLuint program);
    void glDeleteProgram(GLuint program);
    void glDeleteShader(GLuint shader);
    GLuint glCreateShader(GLenum type);
    GLuint glCreateProgram();
    void glCompileShader(GLuint shader);
    void glAttachShader(GLuint program, GLuint shader);
    void glLinkProgram(GLuint program);
    void glShaderSource(GLuint shader, GLsizei count,
        const GLchar** string, const GLint *length);
    void glGetShaderInfoLog(GLuint shader, GLsizei bufSize,
        GLsizei *length, GLchar *infoLog);
    void glGetProgramInfoLog(GLuint program, GLsizei bufSize,
        GLsizei *length, GLchar *infoLog);
    // OpenGL does not formally define a type for uniform locations,
    // but all APIs use GLint. The value -1 is special, typically used
    // as a placeholder for an unused uniform and "silently ignored".
    GLint glGetUniformLocation(GLuint program, const GLchar *name);
    // OpenGL does not formally define a type for attribute locations,
    // but most APIs use GLuint. The exception is glGetAttribLocation, 
    //which returns GLint so that -1 can be used as an error indicator.
    GLint glGetAttribLocation(GLuint program, const GLchar *name);
    void glGetProgramiv(GLuint program, GLenum pname, GLint *params);
    void glGetShaderiv(GLuint shader, GLenum pname, GLint *params);

    // Buffers
    void glBindBuffer(GLenum target, GLuint buffer);
    void glDeleteBuffers(GLsizei n, const GLuint *buffers);
    void glGenBuffers(GLsizei n, GLuint *buffers);
    void glBufferData(GLenum target, GLsizeiptr size, 
        const void *data, GLenum usage);
    void glBufferSubData(GLenum target, GLintptr offset, 
        GLsizeiptr size, const void *data);

    // Framebuffers
    void glBindFramebuffer(GLenum target, GLuint framebuffer);
    void glGenFramebuffers(GLsizei n, GLuint *framebuffers);
    void glFramebufferTexture2D(GLenum target,
        GLenum attachment, GLenum textarget,
        GLuint texture,    GLint level);

    void glRenderbufferStorage(GLenum target,
        GLenum internalformat, GLsizei width, GLsizei height);

    void glFramebufferRenderbuffer(GLenum target, GLenum attachment, 
        GLenum renderbuffertarget, GLuint renderbuffer);

    void genRenderbuffers(GLsizei n, GLuint *renderbuffers);
    void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
    void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);
    void glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
    GLenum glCheckFramebufferStatus(GLenum target);

    // Texture
    void glBindTexture(GLenum target, GLuint texture);
    void glActiveTexture(GLenum texture);
    void glGenTextures(GLsizei n, GLuint *textures);
    void glDeleteTextures(GLsizei n, const GLuint *textures);
    void glTexParameteri(GLenum target, GLenum pname, GLint param);
    void glTexImage2D(GLenum target, GLint level, 
        GLint internalFormat,  GLsizei width, GLsizei height,
        GLint border, GLenum format, GLenum type, const GLvoid *pixels);

    void glTexSubImage2D(GLenum target, GLint level,
        GLint xoffset, GLint yoffset,
        GLsizei width, GLsizei height,
        GLenum format, GLenum type,
        const GLvoid *pixels);

    void glGenerateMipmap(GLenum target);


    void glEnableVertexAttribArray(GLuint index);
    void glDisableVertexAttribArray(GLuint index);
    void glVertexAttribPointer(GLuint index, GLint size,
        GLenum type, GLboolean normalized,
        GLsizei stride, const void *pointer);

    void glDrawArrays(GLenum mode, GLint first, GLsizei count);
    void glDrawElements(GLenum mode, GLsizei count,
        GLenum type, const GLvoid *indices);

   void glUniform1f(GLint location, GLfloat v0);
   void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
   void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
   void glUniform4f(GLint location, 
        GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

    void glUniform1i(GLint location, GLint v0);
    void glUniform2i(GLint location, GLint v0, GLint v1);
    void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2);
    void glUniform4i(GLint location,
        GLint v0, GLint v1, GLint v2, GLint v3);

    void glUniform1fv(GLint location, GLsizei count, const GLfloat *value);
    void glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
    void glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
    void glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
    void glUniform1iv(GLint location, GLsizei count, const GLint *value);
    void glUniform2iv(GLint location, GLsizei count, const GLint *value);
    void glUniform3iv(GLint location, GLsizei count, const GLint *value);
    void glUniform4iv(GLint location, GLsizei count, const GLint *value);

    void glUniformMatrix2fv(GLint location,
        GLsizei count, GLboolean transpose, const GLfloat *value);
    void glUniformMatrix3fv(GLint location, 
        GLsizei count, GLboolean transpose, const GLfloat *value);
    void glUniformMatrix4fv(GLint location, 
        GLsizei count, GLboolean transpose, const GLfloat *value);

    
    void glFinish(void);

    void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height,
        GLenum format, GLenum type, GLvoid* pixels);

    // mapbuffer
    void *glMapBuffer(GLenum target, GLenum access);
    GLboolean glUnmapBuffer(GLenum target);

    void glBindVertexArray(GLuint array);
    void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
    void glGenVertexArrays(GLsizei n, GLuint *arrays);
//};//GL

	//------------------------------------------------
	//------------------------------------------------
	extern void(*const glBlendColor)(GLfloat, GLfloat, GLfloat, GLfloat);
	extern void(*const glBlendEquation)(GLenum);
	extern void(*const glBlendEquationSeparate)(GLenum, GLenum);
	extern void(*const glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum);


	extern void(*const glTexParameterf)(GLenum, GLenum, GLfloat);
	extern void(*const glTexParameterfv)(GLenum, GLenum, const GLfloat *);
	extern void(*const glTexParameteriv)(GLenum, GLenum, const GLint *);
	extern void(*const glCompressedTexImage2D)(GLenum, GLint, GLenum,
		GLsizei, GLsizei, GLint,
		GLsizei, const void *);
	extern void(*const glCompressedTexSubImage2D)(GLenum, GLint, GLint,
		GLint, GLsizei, GLsizei,
		GLenum, GLsizei, const void *);
	extern void(*const glCopyTexImage2D)(GLenum, GLint, GLenum, GLint,
		GLint, GLsizei, GLsizei, GLint);
	extern void(*const glCopyTexSubImage2D)(GLenum, GLint, GLint, GLint,
		GLint, GLint, GLsizei, GLsizei);


	extern void(*const glFlush)();

	extern void(*const glBindAttribLocation)(GLuint, GLuint, const GLchar *);
	extern void(*const glDetachShader)(GLuint, GLuint);
	extern void(*const glGetActiveAttrib)(GLuint, GLuint, GLsizei,
		GLsizei *, GLint *, GLenum *, GLchar *);
	extern void(*const glGetActiveUniform)(GLuint, GLuint, GLsizei,
		GLsizei *, GLint *, GLenum *, GLchar *);
	extern void(*const glGetAttachedShaders)(GLuint, GLsizei,
		GLsizei *, GLuint *);
	extern void(*const glGetShaderSource)(GLuint, GLsizei, GLsizei *, GLchar *);
	extern void(*const glGetUniformfv)(GLuint, GLint, GLfloat *);
	extern void(*const glGetUniformiv)(GLuint, GLint, GLint *);


	extern void(*const glGetBooleanv)(GLenum, GLboolean *);
	extern void(*const glGetBufferParameteriv)(GLenum, GLenum, GLint *);
	extern void(*const glGetFloatv)(GLenum, GLfloat *);

	extern void(*const glGetFramebufferAttachmentParameteriv)(GLenum,
		GLenum, GLenum, GLint *);
	extern void(*const glGetRenderbufferParameteriv)(GLenum, GLenum, GLint *);


	extern void(*const glGetTexParameterfv)(GLenum, GLenum, GLfloat *);
	extern void(*const glGetTexParameteriv)(GLenum, GLenum, GLint *);


	extern void(*const glGetVertexAttribfv)(GLuint, GLenum, GLfloat *);
	extern void(*const glGetVertexAttribiv)(GLuint, GLenum, GLint *);
	extern void(*const glGetVertexAttribPointerv)(GLuint, GLenum, void **);
	extern void(*const glHint)(GLenum, GLenum);

	extern GLboolean(*const glIsBuffer)(GLuint);
	extern GLboolean(*const glIsEnabled)(GLenum);
	extern GLboolean(*const glIsFramebuffer)(GLuint);
	extern GLboolean(*const glIsProgram)(GLuint);
	extern GLboolean(*const glIsRenderbuffer)(GLuint);
	extern GLboolean(*const glIsShader)(GLuint);
	extern GLboolean(*const glIsTexture)(GLuint);


	extern void(*const glPixelStorei)(GLenum, GLint);
	extern void(*const glPolygonOffset)(GLfloat, GLfloat);

	extern void(*const glSampleCoverage)(GLfloat, GLboolean);
	extern void(*const glScissor)(GLint, GLint, GLsizei, GLsizei);


	extern void(*const glStencilFuncSeparate)(GLenum, GLenum,
		GLint, GLuint);

	extern void(*const glStencilMaskSeparate)(GLenum, GLuint);

	extern void(*const glStencilOpSeparate)(GLenum, GLenum,
		GLenum, GLenum);
	

	extern void(*const glValidateProgram)(GLuint);
	extern void(*const glVertexAttrib1f)(GLuint, GLfloat);
	extern void(*const glVertexAttrib1fv)(GLuint, const GLfloat *);
	extern void(*const glVertexAttrib2f)(GLuint, GLfloat, GLfloat);
	extern void(*const glVertexAttrib2fv)(GLuint, const GLfloat *);
	extern void(*const glVertexAttrib3f)(GLuint, GLfloat, GLfloat, GLfloat);
	extern void(*const glVertexAttrib3fv)(GLuint, const GLfloat *);
	extern void(*const glVertexAttrib4f)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
	extern void(*const glVertexAttrib4fv)(GLuint, const GLfloat *);

#ifndef MBGL_USE_GLES2
	extern void(*const glDrawPixels)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
	extern void(*const glGetDoublev)(GLenum, GLdouble *);
	extern void(*const glPixelTransferf)(GLenum, GLfloat);
	extern void(*const glPixelZoom)(GLfloat, GLfloat);
	extern void(*const glPointSize)(GLfloat);
	extern void(*const glRasterPos4d)(GLdouble, GLdouble, GLdouble, GLdouble);
#endif

#ifndef NDEBUG
	/// Check for GL errors and print on the console.
	void glCheckError(const char *cmd, const char *file, int line);
#endif
}// namespace md


#endif // #ifndef OPENGL_DECLARATION__H