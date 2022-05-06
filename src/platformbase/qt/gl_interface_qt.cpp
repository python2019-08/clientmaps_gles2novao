#ifdef SWDevFW_QT    
    #include <QtGui/QOpenGLContext>
    #include <QtGui/QOpenGLFunctions>
    #define  QGLCurCtxFuncs  QOpenGLContext::currentContext()->functions()
#endif

#include "platformbase/gl_interface.h"
 
#ifdef SWDevFW_QT   
namespace md {

///////////////////////////////////////////
///////////////////////////////////////////
GLenum  glGetError() 
{
    return QGLCurCtxFuncs->glGetError();
}

const GLubyte*  glGetString(GLenum name) 
{
    return QGLCurCtxFuncs->glGetString(name);
}

void  glClear(GLbitfield mask) {
    QGLCurCtxFuncs->glClear(mask);
}
void  glLineWidth(GLfloat width) {
    QGLCurCtxFuncs->glLineWidth(width);
}
void  glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    QGLCurCtxFuncs->glViewport(x, y, width, height);
}

void  glEnable(GLenum id) {
    QGLCurCtxFuncs->glEnable(id);
}
void  glDisable(GLenum id) {
    QGLCurCtxFuncs->glDisable(id);
}
void  glDepthFunc(GLenum func) {
    QGLCurCtxFuncs->glDepthFunc(func);
}
void  glDepthMask(GLboolean flag) {
    QGLCurCtxFuncs->glDepthMask(flag);
}
void  glDepthRangef(GLfloat n, GLfloat f) {
    QGLCurCtxFuncs->glDepthRangef(n, f);
}
void  glClearDepthf(GLfloat d) {
    QGLCurCtxFuncs->glClearDepthf(d);
}
void  glBlendFunc(GLenum sfactor, GLenum dfactor) {
    QGLCurCtxFuncs->glBlendFunc(sfactor, dfactor);
}
void  glStencilFunc(GLenum func, GLint ref, GLuint mask) {
    QGLCurCtxFuncs->glStencilFunc(func, ref, mask);
}
void  glStencilMask(GLuint mask) {
    QGLCurCtxFuncs->glStencilMask(mask);
}
void  glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
    QGLCurCtxFuncs->glStencilOp(fail, zfail, zpass);
}
void  glClearStencil(GLint s) {
    QGLCurCtxFuncs->glClearStencil(s);
}
void  glColorMask(GLboolean red, GLboolean green, 
                     GLboolean blue, GLboolean alpha) 
{
    QGLCurCtxFuncs->glColorMask(red, green, blue, alpha);
}
void  glCullFace(GLenum mode) {
    QGLCurCtxFuncs->glCullFace(mode);
}
void  glFrontFace(GLenum mode) {
    QGLCurCtxFuncs->glFrontFace(mode);
}
void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) 
{
    QGLCurCtxFuncs->glClearColor(red, green, blue, alpha);
}
void  glGetIntegerv(GLenum pname, GLint *params )
{
    QGLCurCtxFuncs->glGetIntegerv(pname, params );
}

// Program
void  glUseProgram(GLuint program)
{
    QGLCurCtxFuncs->glUseProgram(program);
}
void  glDeleteProgram(GLuint program) {
    QGLCurCtxFuncs->glDeleteProgram(program);
}
void  glDeleteShader(GLuint shader) {
    QGLCurCtxFuncs->glDeleteShader(shader);
}
GLuint  glCreateShader(GLenum type) {
    auto result = QGLCurCtxFuncs->glCreateShader(type);
    return result;
}
GLuint  glCreateProgram() {
    auto result = QGLCurCtxFuncs->glCreateProgram();     
    return result;
}

void  glCompileShader(GLuint shader) {
    QGLCurCtxFuncs->glCompileShader(shader);
}
void  glAttachShader(GLuint program, GLuint shader) {
    QGLCurCtxFuncs->glAttachShader(program,shader);
}
 

void  glLinkProgram(GLuint program) {
    QGLCurCtxFuncs->glLinkProgram(program);
}

void  glShaderSource(GLuint shader, GLsizei count,
    const GLchar **string, const GLint *length) 
{
    auto source = const_cast<const GLchar**>(string);
    QGLCurCtxFuncs->glShaderSource(shader, count, source, length);
}
void  glGetShaderInfoLog(GLuint shader, GLsizei bufSize,
    GLsizei *length, GLchar *infoLog) 
{
    QGLCurCtxFuncs->glGetShaderInfoLog(shader, bufSize, length, infoLog);
}
void  glGetProgramInfoLog(GLuint program, GLsizei bufSize,
    GLsizei *length, GLchar *infoLog) 
{
    QGLCurCtxFuncs->glGetProgramInfoLog(program, bufSize, length, infoLog);
}
GLint  glGetUniformLocation(GLuint program, const GLchar *name)
{
    auto result = QGLCurCtxFuncs->glGetUniformLocation(program, name);
    return result;
}
GLint  glGetAttribLocation(GLuint program, const GLchar *name) {
    auto result = QGLCurCtxFuncs->glGetAttribLocation(program, name);
    return result;
}
 
void  glGetProgramiv(GLuint program, GLenum pname, GLint *params) {
    QGLCurCtxFuncs->glGetProgramiv(program,pname,params);
}
void  glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
    QGLCurCtxFuncs->glGetShaderiv(shader,pname, params);
}

// Buffers
void  glBindBuffer(GLenum target, GLuint buffer) {
    QGLCurCtxFuncs->glBindBuffer(target, buffer);
}
 

void  glDeleteBuffers(GLsizei n, const GLuint *buffers) {
    QGLCurCtxFuncs->glDeleteBuffers(n, buffers);
}
void  glGenBuffers(GLsizei n, GLuint *buffers) {
    QGLCurCtxFuncs->glGenBuffers(n, buffers);
}
void  glBufferData(GLenum target, GLsizeiptr size, 
    const void *data, GLenum usage) 
{
    QGLCurCtxFuncs->glBufferData(target, size, data, usage);
}
void  glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data) {
    QGLCurCtxFuncs->glBufferSubData(target, offset, size, data);
}
void  glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height,
                    GLenum format, GLenum type, GLvoid* pixels) {
     QGLCurCtxFuncs->glReadPixels(x, y, width, height, 
        format, type, pixels);
}

// Texture
void  glBindTexture(GLenum target, GLuint texture ) {
    QGLCurCtxFuncs->glBindTexture(target, texture );
}
void  glActiveTexture(GLenum texture) {
    QGLCurCtxFuncs->glActiveTexture(texture);
}

void  glGenTextures(GLsizei n, GLuint *textures ) {
    QGLCurCtxFuncs->glGenTextures(n, textures );
}
void  glDeleteTextures(GLsizei n, const GLuint *textures) {
    QGLCurCtxFuncs->glDeleteTextures(n, textures);
}
void  glTexParameteri(GLenum target, GLenum pname, GLint param )
{
    QGLCurCtxFuncs->glTexParameteri(target, pname, param );
}
void  glTexImage2D(GLenum target, GLint level,
                    GLint internalFormat, 
                    GLsizei width, GLsizei height, GLint border, 
                    GLenum format, GLenum type, const GLvoid *pixels) 
{
    QGLCurCtxFuncs->glTexImage2D(target, level, 
        internalFormat, 
        width, height, border, 
        format, type, pixels); 
}

void  glTexSubImage2D(GLenum target, GLint level,
    GLint   xoffset,  GLint   yoffset, 
    GLsizei width,    GLsizei height,
    GLenum  format,   GLenum  type,  const GLvoid *pixels ) 
{
     QGLCurCtxFuncs->glTexSubImage2D(target, level,
        xoffset, yoffset, width, height, 
        format, type, pixels); 
}

void  glGenerateMipmap(GLenum target) {
    QGLCurCtxFuncs->glGenerateMipmap(target);
}

void  glEnableVertexAttribArray(GLuint index) {
    QGLCurCtxFuncs->glEnableVertexAttribArray(index);
}
void  glDisableVertexAttribArray(GLuint index) {
    QGLCurCtxFuncs->glDisableVertexAttribArray(index);
}
void  glVertexAttribPointer(GLuint index,  GLint size,
    GLenum type,  GLboolean normalized,
    GLsizei stride,  const void *pointer) 
{
    QGLCurCtxFuncs->glVertexAttribPointer(index, 
        size, type,  normalized, 
        stride,  pointer);
}

void  glDrawArrays(GLenum mode, GLint first, GLsizei count )
{
    QGLCurCtxFuncs->glDrawArrays(mode, first, count );
}
void  glDrawElements(GLenum mode, GLsizei count,
                        GLenum type, const GLvoid *indices ) 
{
    QGLCurCtxFuncs->glDrawElements(mode, count, type, indices );
}

void  glUniform1f(GLint location, GLfloat v0) {
    QGLCurCtxFuncs->glUniform1f(location, v0);
}
void  glUniform2f(GLint location, GLfloat v0, GLfloat v1) {
    QGLCurCtxFuncs->glUniform2f(location, v0, v1);
}
void  glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    QGLCurCtxFuncs->glUniform3f(location, v0, v1, v2);
}
void  glUniform4f(GLint location, 
    GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    QGLCurCtxFuncs->glUniform4f(location, v0, v1, v2, v3);
}

void  glUniform1i(GLint location, GLint v0) {
    QGLCurCtxFuncs->glUniform1i(location, v0);
}
void  glUniform2i(GLint location, GLint v0, GLint v1) {
    QGLCurCtxFuncs->glUniform2i(location, v0, v1);
}
void  glUniform3i(GLint location, GLint v0, GLint v1, GLint v2) 
{
    QGLCurCtxFuncs->glUniform3i(location, v0, v1, v2);
}
void  glUniform4i(GLint location, 
    GLint v0, GLint v1, GLint v2, GLint v3) 
{
    QGLCurCtxFuncs->glUniform4i(location, v0, v1, v2, v3);
}

void  glUniform1fv(GLint location, GLsizei count, const GLfloat *value) 
{
    QGLCurCtxFuncs->glUniform1fv(location, count, value);
}
void  glUniform2fv(GLint location, GLsizei count, const GLfloat *value) {
    QGLCurCtxFuncs->glUniform2fv(location, count, value);
}
void  glUniform3fv(GLint location, GLsizei count, const GLfloat *value) {
    QGLCurCtxFuncs->glUniform3fv(location, count, value);
}
void  glUniform4fv(GLint location, GLsizei count, const GLfloat *value) {
    QGLCurCtxFuncs->glUniform4fv(location, count, value);
}
void  glUniform1iv(GLint location, GLsizei count, const GLint *value) {
    QGLCurCtxFuncs->glUniform1iv(location, count, value);
}
void  glUniform2iv(GLint location, GLsizei count, const GLint *value) {
    QGLCurCtxFuncs->glUniform2iv(location, count, value);
}
void  glUniform3iv(GLint location, GLsizei count, const GLint *value) {
    QGLCurCtxFuncs->glUniform3iv(location, count, value);
}
void  glUniform4iv(GLint location, GLsizei count, const GLint *value) {
    QGLCurCtxFuncs->glUniform4iv(location, count, value);
}

void  glUniformMatrix2fv(GLint location,
    GLsizei count, 
    GLboolean transpose, 
    const GLfloat *value
) 
{
    QGLCurCtxFuncs->glUniformMatrix2fv(location, count, transpose, value);
}
void  glUniformMatrix3fv(GLint location,
    GLsizei count, 
    GLboolean transpose, 
    const GLfloat *value
) 
{
    QGLCurCtxFuncs->glUniformMatrix3fv(location, count, transpose, value);
}
void  glUniformMatrix4fv(GLint location,
    GLsizei count, 
    GLboolean transpose, 
    const GLfloat *value
) 
{
    QGLCurCtxFuncs->glUniformMatrix4fv(location, count, transpose, value);
}

void  glFinish(void) {
    QGLCurCtxFuncs->glFinish();
}

// mapbuffer
void*  glMapBuffer(GLenum target, GLenum access) {
    void* result = NULL;// QGLCurCtxFuncs->glMapBuffer(target, access);//gmq
    //GL_CHECK("glMapBuffer");
    return result;
}

GLboolean  glUnmapBuffer(GLenum target) {
    GLboolean result = false;// QGLCurCtxFuncs->glUnmapBuffer(target);//gmq
    //GL_CHECK("glUnmapBuffer);
    return result;
}
 

// VAO
void  glBindVertexArray(GLuint array) 
{
    //GL_CHECK( QGLCurCtxFuncs->glBindVertexArray(array));
}

void  glDeleteVertexArrays(GLsizei n, const GLuint *arrays) 
{
    //GL_CHECK( QGLCurCtxFuncs->glDeleteVertexArrays(n, arrays));
}
void  glGenVertexArrays(GLsizei n, GLuint *arrays) 
{
    //GL_CHECK( QGLCurCtxFuncs->glGenVertexArrays(n, arrays));
}

// Framebuffer
void  glBindFramebuffer(GLenum target, GLuint framebuffer) {
    QGLCurCtxFuncs->glBindFramebuffer(target, framebuffer);
}

void  glGenFramebuffers(GLsizei n, GLuint *framebuffers) {
    QGLCurCtxFuncs->glGenFramebuffers(n, framebuffers);
}

void  glFramebufferTexture2D(GLenum target,
    GLenum attachment, 
    GLenum textarget,
    GLuint texture, 
    GLint level) 
{
    QGLCurCtxFuncs->glFramebufferTexture2D(target, 
        attachment, textarget, texture, level);
}

void  glRenderbufferStorage(GLenum target,
    GLenum internalformat, 
    GLsizei width,
    GLsizei height
) 
{
    QGLCurCtxFuncs->glRenderbufferStorage(target, 
        internalformat, width, height);
}

void  glFramebufferRenderbuffer(GLenum target,
    GLenum attachment,
    GLenum renderbuffertarget, 
    GLuint renderbuffer) 
{
    QGLCurCtxFuncs->glFramebufferRenderbuffer(target, 
        attachment, 
        renderbuffertarget, 
        renderbuffer);
}

void  genRenderbuffers(GLsizei n, GLuint *renderbuffers) {
    QGLCurCtxFuncs->glGenRenderbuffers(n, renderbuffers);
}

void  glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
    QGLCurCtxFuncs->glBindRenderbuffer(target, renderbuffer);
}

void  glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers) {
    QGLCurCtxFuncs->glDeleteFramebuffers(n, framebuffers);
}

void  glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) {
    QGLCurCtxFuncs->glDeleteRenderbuffers(n, renderbuffers);
}

GLenum  glCheckFramebufferStatus(GLenum target) {
    GLenum status = QGLCurCtxFuncs->glCheckFramebufferStatus(target);
    return status;
}

///////////////////////////////////////////
///////////////////////////////////////////

void(*const glBlendColor)(GLfloat, GLfloat,
	GLfloat, GLfloat) = [](auto... args) //--
{
	return QGLCurCtxFuncs->glBlendColor(args...);
};

void(*const glBlendEquation)(GLenum) = [](auto... args) {//--
	return QGLCurCtxFuncs->glBlendEquation(args...);
};

void(*const glBlendEquationSeparate)(GLenum, GLenum) = [](auto... args) {//--
	return QGLCurCtxFuncs->glBlendEquationSeparate(args...);
};


void(*const glBlendFuncSeparate)(GLenum, GLenum,
	GLenum, GLenum) = [](auto... args) //--
{
	return QGLCurCtxFuncs->glBlendFuncSeparate(args...);
};


void(*const glCompressedTexImage2D)(GLenum, GLint, //--
	GLenum, GLsizei, GLsizei,
	GLint, GLsizei, const void *) = [](auto... args)
{
	return QGLCurCtxFuncs->glCompressedTexImage2D(args...);
};

void(*const glCompressedTexSubImage2D)(GLenum, GLint, //--
	GLint, GLint,
	GLsizei, GLsizei,
	GLenum, GLsizei,
	const void *) = [](auto... args)
{
	return QGLCurCtxFuncs->glCompressedTexSubImage2D(args...);
};

void(*const glCopyTexImage2D)(GLenum, GLint, //--
	GLenum, GLint, GLint,
	GLsizei, GLsizei, GLint) = [](auto... args)
{
	return QGLCurCtxFuncs->glCopyTexImage2D(args...);
};

void(*const glCopyTexSubImage2D)(GLenum, GLint, //--
	GLint, GLint,
	GLint, GLint,
	GLsizei, GLsizei) = [](auto... args)
{
	return QGLCurCtxFuncs->glCopyTexSubImage2D(args...);
};

void(*const glDetachShader)(GLuint, GLuint) = [](auto... args) //--
{
	return QGLCurCtxFuncs->glDetachShader(args...);
};

void(*const glFlush)() = [](auto... args) //--
{
	return QGLCurCtxFuncs->glFlush(args...);
};


void(*const glGetActiveAttrib)(GLuint, GLuint, GLsizei, //--
	GLsizei *, GLint *,
	GLenum *, GLchar *) = [](auto... args)
{
	return QGLCurCtxFuncs->glGetActiveAttrib(args...);
};

void(*const glGetActiveUniform)(GLuint, GLuint, GLsizei, //--
	GLsizei *, GLint *,
	GLenum *, GLchar *) = [](auto... args)
{
	return QGLCurCtxFuncs->glGetActiveUniform(args...);
};

void(*const glGetAttachedShaders)(GLuint, GLsizei,
	GLsizei *, GLuint *) = [](auto... args) //--
{
	return QGLCurCtxFuncs->glGetAttachedShaders(args...);
};

void(*const glBindAttribLocation)(GLuint, //--
	GLuint,
	const GLchar *
	) = [](auto... args)
{
	return QGLCurCtxFuncs->glBindAttribLocation(args...);
};



void(*const glGetBooleanv)(GLenum, GLboolean *) = [](auto... args)  //--
{
	return QGLCurCtxFuncs->glGetBooleanv(args...);
};

void(*const glGetBufferParameteriv)(GLenum,
	GLenum,
	GLint *) = [](auto... args) //--
{
	return QGLCurCtxFuncs->glGetBufferParameteriv(args...);
};



void(*const glGetFloatv)(GLenum, GLfloat *) = [](auto... args) {//--
	return QGLCurCtxFuncs->glGetFloatv(args...);
};

void(*const glGetFramebufferAttachmentParameteriv)(GLenum,
	GLenum,
	GLenum,
	GLint *) = [](auto... args) //--
{
	return QGLCurCtxFuncs->glGetFramebufferAttachmentParameteriv(args...);
};




void(*const glGetRenderbufferParameteriv)(GLenum,
	GLenum,
	GLint *) = [](auto... args) //--
{
	return QGLCurCtxFuncs->glGetRenderbufferParameteriv(args...);
};



void(*const glGetShaderSource)(GLuint,//-- 
	GLsizei,
	GLsizei *,
	GLchar *) = [](auto... args)
{
	return QGLCurCtxFuncs->glGetShaderSource(args...);
};



void(*const glGetTexParameterfv)(GLenum,//-- 
	GLenum,
	GLfloat *) = [](auto... args)
{
	return QGLCurCtxFuncs->glGetTexParameterfv(args...);
};

void(*const glGetTexParameteriv)(GLenum,
	GLenum,
	GLint *) = [](auto... args) //-- 
{
	return QGLCurCtxFuncs->glGetTexParameteriv(args...);
};

void(*const glGetUniformfv)(GLuint,
	GLint,
	GLfloat *) = [](auto... args) //-- 
{
	return QGLCurCtxFuncs->glGetUniformfv(args...);
};

void(*const glGetUniformiv)(GLuint,
	GLint,
	GLint *) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glGetUniformiv(args...);
};


void(*const glGetVertexAttribfv)(GLuint,
	GLenum,
	GLfloat *) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glGetVertexAttribfv(args...);
};

void(*const glGetVertexAttribiv)(GLuint,
	GLenum,
	GLint *) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glGetVertexAttribiv(args...);
};

void(*const glGetVertexAttribPointerv)(GLuint,
	GLenum,
	void **) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glGetVertexAttribPointerv(args...);
};

void(*const glHint)(GLenum, GLenum) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glHint(args...);
};

GLboolean(*const glIsBuffer)(GLuint) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glIsBuffer(args...);
};

GLboolean(*const glIsEnabled)(GLenum) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glIsEnabled(args...);
};

GLboolean(*const glIsFramebuffer)(GLuint) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glIsFramebuffer(args...);
};

GLboolean(*const glIsProgram)(GLuint) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glIsProgram(args...);
};

GLboolean(*const glIsRenderbuffer)(GLuint) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glIsRenderbuffer(args...);
};

GLboolean(*const glIsShader)(GLuint) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glIsShader(args...);
};

GLboolean(*const glIsTexture)(GLuint) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glIsTexture(args...);
};



void(*const glPixelStorei)(GLenum, GLint) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glPixelStorei(args...);
};

void(*const glPolygonOffset)(GLfloat, GLfloat) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glPolygonOffset(args...);
};



void(*const glSampleCoverage)(GLfloat,
	GLboolean) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glSampleCoverage(args...);
};

void(*const glScissor)(GLint, GLint,
	GLsizei, GLsizei) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glScissor(args...);
};
 
void(*const glStencilFuncSeparate)(GLenum, GLenum,
	GLint, GLuint) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glStencilFuncSeparate(args...);
};

void(*const glStencilMaskSeparate)(GLenum,
	GLuint) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glStencilMaskSeparate(args...);
};

void(*const glStencilOpSeparate)(GLenum,
	GLenum,
	GLenum,
	GLenum) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glStencilOpSeparate(args...);
};

void(*const glTexParameterf)(GLenum,
	GLenum,
	GLfloat) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glTexParameterf(args...);
};

void(*const glTexParameterfv)(GLenum,
	GLenum,
	const GLfloat *) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glTexParameterfv(args...);
};

void(*const glTexParameteriv)(GLenum, GLenum,
	const GLint *) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glTexParameteriv(args...);
};

void(*const glValidateProgram)(GLuint) = [](auto... args) 
{//-- 
	return QGLCurCtxFuncs->glValidateProgram(args...);
};

void(*const glVertexAttrib1f)(GLuint,
	GLfloat) = [](auto... args) 
{//-- 
	return QGLCurCtxFuncs->glVertexAttrib1f(args...);
};

void(*const glVertexAttrib1fv)(GLuint, 
                               const GLfloat *) = [](auto... args) 
{
	return QGLCurCtxFuncs->glVertexAttrib1fv(args...);
};

void(*const glVertexAttrib2f)(GLuint,
	GLfloat, GLfloat) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glVertexAttrib2f(args...);
};

void(*const glVertexAttrib2fv)(GLuint,
	const GLfloat *) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glVertexAttrib2fv(args...);
};

void(*const glVertexAttrib3f)(GLuint,
	GLfloat, GLfloat, GLfloat) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glVertexAttrib3f(args...);
};

void(*const glVertexAttrib3fv)(GLuint,
	const GLfloat *) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glVertexAttrib3fv(args...);
};

void(*const glVertexAttrib4f)(GLuint,
	GLfloat, GLfloat, GLfloat, GLfloat) = [](auto... args) {//-- 
	return QGLCurCtxFuncs->glVertexAttrib4f(args...);
};

void(*const glVertexAttrib4fv)(GLuint,
	const GLfloat *) = [](auto... args) //-- 
{
	return QGLCurCtxFuncs->glVertexAttrib4fv(args...);
};
 
}// namespace md
#endif
