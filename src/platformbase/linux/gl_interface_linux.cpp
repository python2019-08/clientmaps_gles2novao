#include "platformbase/gl_interface.h"
 
#ifdef OsID_LINUX
namespace md {

///////////////////////////////////////////
///////////////////////////////////////////
GLenum  glGetError() 
{
    return ::glGetError();
}

const GLubyte*  glGetString(GLenum name) 
{
    auto result = ::glGetString(name);
    return result;
}

void  glClear(GLbitfield mask) {
    ::glClear(mask);
}
void  glLineWidth(GLfloat width) {
    ::glLineWidth(width);
}
void  glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    ::glViewport(x, y, width, height);
}

void  glEnable(GLenum id) {
    ::glEnable(id);
}
void  glDisable(GLenum id) {
    ::glDisable(id);
}
void  glDepthFunc(GLenum func) {
    ::glDepthFunc(func);
}
void  glDepthMask(GLboolean flag) {
    ::glDepthMask(flag);
}
void  glDepthRangef(GLfloat n, GLfloat f) {
    ::glDepthRangef(n, f);
}
void  glClearDepthf(GLfloat d) {
    ::glClearDepthf(d);
}
void  glBlendFunc(GLenum sfactor, GLenum dfactor) {
    ::glBlendFunc(sfactor, dfactor);
}
void  glStencilFunc(GLenum func, GLint ref, GLuint mask) {
    ::glStencilFunc(func, ref, mask);
}
void  glStencilMask(GLuint mask) {
    ::glStencilMask(mask);
}
void  glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
    ::glStencilOp(fail, zfail, zpass);
}
void  glClearStencil(GLint s) {
    ::glClearStencil(s);
}
void  glColorMask(GLboolean red, GLboolean green, 
                     GLboolean blue, GLboolean alpha) 
{
    ::glColorMask(red, green, blue, alpha);
}
void  glCullFace(GLenum mode) {
    ::glCullFace(mode);
}
void  glFrontFace(GLenum mode) {
    ::glFrontFace(mode);
}
void  glClearColor(GLclampf red,GLclampf green,GLclampf blue,GLclampf alpha) 
{
    ::glClearColor(red, green, blue, alpha);
}
void  glGetIntegerv(GLenum pname, GLint *params )
{
    ::glGetIntegerv(pname, params );
}

// Program
void  glUseProgram(GLuint program)
{
    ::glUseProgram(program);
}
void  glDeleteProgram(GLuint program) {
    ::glDeleteProgram(program);
}
void  glDeleteShader(GLuint shader) {
    ::glDeleteShader(shader);
}
GLuint  glCreateShader(GLenum type) {
    auto result = ::glCreateShader(type);
    return result;
}
GLuint  glCreateProgram() {
    auto result = ::glCreateProgram();
    return result;
}

void  glCompileShader(GLuint shader) {
    ::glCompileShader(shader);
}
void  glAttachShader(GLuint program, GLuint shader) {
    ::glAttachShader(program,shader);
}
 

void  glLinkProgram(GLuint program) {
    ::glLinkProgram(program);
}
//
// Some implementations use `const GLchar* const*` 
// and others `const GLchar**`.
//
//void (* const glShaderSource)(GLuint, GLsizei, 
//        const GLchar * const*, const GLint *) = [](GLuint shader, 
//          GLsizei count, const GLchar * const * string, 
//          const GLint *length) 
//{
//    ::glShaderSource(shader, count, 
//                     const_cast<const GLchar **>(string), 
//                      length);
//};
void  glShaderSource(GLuint shader, GLsizei count,
    const GLchar **string, const GLint *length) 
{
    auto source = const_cast<const GLchar**>(string);
    ::glShaderSource(shader, count, source, length);
}

void  glGetShaderInfoLog(GLuint shader, GLsizei bufSize,
    GLsizei *length, GLchar *infoLog) 
{
    ::glGetShaderInfoLog(shader, bufSize, length, infoLog);
}
void  glGetProgramInfoLog(GLuint program, GLsizei bufSize,
    GLsizei *length, GLchar *infoLog) 
{
    ::glGetProgramInfoLog(program, bufSize, length, infoLog);
}
GLint  glGetUniformLocation(GLuint program, const GLchar *name)
{
    auto result = ::glGetUniformLocation(program, name);
    return result;
}
GLint  glGetAttribLocation(GLuint program, const GLchar *name) {
    auto result = ::glGetAttribLocation(program, name);
    return result;
}
 
void  glGetProgramiv(GLuint program, GLenum pname, GLint *params) {
    ::glGetProgramiv(program,pname,params);
}
void  glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
    ::glGetShaderiv(shader,pname, params);
}

// Buffers
void  glBindBuffer(GLenum target, GLuint buffer) {
    ::glBindBuffer(target, buffer);
}
 

void  glDeleteBuffers(GLsizei n, const GLuint *buffers) {
    ::glDeleteBuffers(n, buffers);
}
void  glGenBuffers(GLsizei n, GLuint *buffers) {
    ::glGenBuffers(n, buffers);
}
void  glBufferData(GLenum target, GLsizeiptr size, 
    const void *data, GLenum usage) 
{
    ::glBufferData(target, size, data, usage);
}
void  glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data) {
    ::glBufferSubData(target, offset, size, data);
}
void  glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height,
                    GLenum format, GLenum type, GLvoid* pixels) 
{
    ::glReadPixels(x, y, width, height, 
        format, type, pixels) ;
}

// Texture
void  glBindTexture(GLenum target, GLuint texture ) {
    ::glBindTexture(target, texture );
}
void  glActiveTexture(GLenum texture) {
    ::glActiveTexture(texture);
}

void  glGenTextures(GLsizei n, GLuint *textures ) {
    ::glGenTextures(n, textures);
}
void  glDeleteTextures(GLsizei n, const GLuint *textures) {
    ::glDeleteTextures(n, textures);
}
void  glTexParameteri(GLenum target, GLenum pname, GLint param )
{
    ::glTexParameteri(target, pname, param );
}
void  glTexImage2D(GLenum target, GLint level,
                    GLint internalFormat, 
                    GLsizei width, GLsizei height, GLint border, 
                    GLenum format, GLenum type, const GLvoid *pixels) 
{
    ::glTexImage2D(target, level, 
        internalFormat, 
        width, height, border, 
        format, type, pixels); 
}

void  glTexSubImage2D(GLenum target, GLint level,
    GLint   xoffset,  GLint   yoffset, 
    GLsizei width,    GLsizei height,
    GLenum  format,   GLenum  type,  const GLvoid *pixels ) 
{
     ::glTexSubImage2D(target, level,
        xoffset, yoffset, width, height, 
        format, type, pixels); 
}

void  glGenerateMipmap(GLenum target) {
    ::glGenerateMipmap(target);
}

void  glEnableVertexAttribArray(GLuint index) {
    ::glEnableVertexAttribArray(index);
}
void  glDisableVertexAttribArray(GLuint index) {
    ::glDisableVertexAttribArray(index);
}
void  glVertexAttribPointer(GLuint index,  GLint size,
    GLenum type,  GLboolean normalized,
    GLsizei stride,  const void *pointer) 
{
    ::glVertexAttribPointer(index, 
        size, type,  normalized, 
        stride,  pointer);
}

void  glDrawArrays(GLenum mode, GLint first, GLsizei count )
{
    ::glDrawArrays(mode, first, count );
}
void  glDrawElements(GLenum mode, GLsizei count,
                        GLenum type, const GLvoid *indices ) 
{
    ::glDrawElements(mode, count, type, indices);
}

void  glUniform1f(GLint location, GLfloat v0) {
    ::glUniform1f(location, v0);
}
void  glUniform2f(GLint location, GLfloat v0, GLfloat v1) {
    ::glUniform2f(location, v0, v1);
}
void  glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    ::glUniform3f(location, v0, v1, v2);
}
void  glUniform4f(GLint location, 
    GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    ::glUniform4f(location, v0, v1, v2, v3);
}

void  glUniform1i(GLint location, GLint v0) {
    ::glUniform1i(location, v0);
}
void  glUniform2i(GLint location, GLint v0, GLint v1) {
    ::glUniform2i(location, v0, v1);
}
void  glUniform3i(GLint location, GLint v0, GLint v1, GLint v2) 
{
    ::glUniform3i(location, v0, v1, v2);
}
void  glUniform4i(GLint location, 
    GLint v0, GLint v1, GLint v2, GLint v3) 
{
    ::glUniform4i(location, v0, v1, v2, v3);
}

void  glUniform1fv(GLint location, GLsizei count, const GLfloat *value) 
{
    ::glUniform1fv(location, count, value);
}
void  glUniform2fv(GLint location, GLsizei count, const GLfloat *value) {
    ::glUniform2fv(location, count, value);
}
void  glUniform3fv(GLint location, GLsizei count, const GLfloat *value) {
    ::glUniform3fv(location, count, value);
}
void  glUniform4fv(GLint location, GLsizei count, const GLfloat *value) {
    ::glUniform4fv(location, count, value);
}
void  glUniform1iv(GLint location, GLsizei count, const GLint *value) {
    ::glUniform1iv(location, count, value);
}
void  glUniform2iv(GLint location, GLsizei count, const GLint *value) {
    ::glUniform2iv(location, count, value);
}
void  glUniform3iv(GLint location, GLsizei count, const GLint *value) {
    ::glUniform3iv(location, count, value);
}
void  glUniform4iv(GLint location, GLsizei count, const GLint *value) {
    ::glUniform4iv(location, count, value);
}

void  glUniformMatrix2fv(GLint location,
    GLsizei count, 
    GLboolean transpose, 
    const GLfloat *value
) 
{
    ::glUniformMatrix2fv(location, count, transpose, value);
}
void  glUniformMatrix3fv(GLint location,
    GLsizei count, 
    GLboolean transpose, 
    const GLfloat *value
) 
{
    ::glUniformMatrix3fv(location, count, transpose, value);
}
void  glUniformMatrix4fv(GLint location,
    GLsizei count, 
    GLboolean transpose, 
    const GLfloat *value
) 
{
    ::glUniformMatrix4fv(location, count, transpose, value);
}

void  glFinish(void) {
    ::glFinish();
}

// Framebuffer
void  glBindFramebuffer(GLenum target, GLuint framebuffer) {
     ::glBindFramebuffer(target, framebuffer);
}

void  glGenFramebuffers(GLsizei n, GLuint *framebuffers) {
    ::glGenFramebuffers(n, framebuffers);
}

void  glFramebufferTexture2D(GLenum target,
    GLenum attachment, 
    GLenum textarget,
    GLuint texture, 
    GLint level) 
{
    ::glFramebufferTexture2D(target, 
        attachment, textarget, texture, level);
}

void  glRenderbufferStorage(GLenum target,
    GLenum internalformat, 
    GLsizei width,
    GLsizei height) 
{
    ::glRenderbufferStorage(target, 
        internalformat, width, height);
}

void  glFramebufferRenderbuffer(GLenum target,
    GLenum attachment,
    GLenum renderbuffertarget, 
    GLuint renderbuffer) 
{
    ::glFramebufferRenderbuffer(target,attachment, 
        renderbuffertarget, renderbuffer);
}

void  genRenderbuffers(GLsizei n, GLuint *renderbuffers) {
    ::glGenRenderbuffers(n, renderbuffers);
}

void  glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
    ::glBindRenderbuffer(target, renderbuffer);
}

void  glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers) {
    ::glDeleteFramebuffers(n, framebuffers);
}

void  glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) {
    ::glDeleteRenderbuffers(n, renderbuffers);
}

GLenum  glCheckFramebufferStatus(GLenum target) {
    GLenum status = ::glCheckFramebufferStatus(target);
    return status;
}

// mapbuffer
void*  glMapBuffer(GLenum target, GLenum access) {
    void* result = NULL;// ::glMapBuffer(target, access);//gmq
    return result;
}

GLboolean  glUnmapBuffer(GLenum target) {
    GLboolean result = false;// ::glUnmapBuffer(target);//gmq
    return result;
}
 

// VAO
void  glBindVertexArray(GLuint array) 
{
    //::glBindVertexArray(array);
}

void  glDeleteVertexArrays(GLsizei n, const GLuint *arrays) 
{
    //::glDeleteVertexArrays(n, arrays);
}
void  glGenVertexArrays(GLsizei n, GLuint *arrays) 
{
    //::glGenVertexArrays(n, arrays);
}

///////////////////////////////////////////
///////////////////////////////////////////
void(*const glBlendColor)(GLfloat, GLfloat,
	GLfloat, GLfloat) = [](auto... args) //--
{
	return ::glBlendColor(args...);
};	
 
void(*const glBlendEquation)(GLenum) = [](auto... args) {//--
	return ::glBlendEquation(args...);
};
 
void(*const glBlendEquationSeparate)(GLenum, GLenum) = [](auto... args) {//--
	return ::glBlendEquationSeparate(args...);
};


void(*const glBlendFuncSeparate)(GLenum, GLenum,
	GLenum, GLenum) = [](auto... args) //--
{
	return ::glBlendFuncSeparate(args...);
};


void(*const glCompressedTexImage2D)(GLenum, GLint, //--
	GLenum, GLsizei, GLsizei,
	GLint, GLsizei, const void *) = [](auto... args)
{
	return ::glCompressedTexImage2D(args...);
};

void(*const glCompressedTexSubImage2D)(GLenum, GLint, //--
	GLint, GLint,
	GLsizei, GLsizei,
	GLenum, GLsizei,
	const void *) = [](auto... args)
{
	return ::glCompressedTexSubImage2D(args...);
};

void(*const glCopyTexImage2D)(GLenum, GLint, //--
	GLenum, GLint, GLint,
	GLsizei, GLsizei, GLint) = [](auto... args)
{
	return ::glCopyTexImage2D(args...);
};

void(*const glCopyTexSubImage2D)(GLenum, GLint, //--
	GLint, GLint,
	GLint, GLint,
	GLsizei, GLsizei) = [](auto... args)
{
	return ::glCopyTexSubImage2D(args...);
};

void(*const glDetachShader)(GLuint, GLuint) = [](auto... args) //--
{
	return ::glDetachShader(args...);
};

void(*const glFlush)() = [](auto... args) //--
{
	return ::glFlush(args...);
};


void(*const glGetActiveAttrib)(GLuint, GLuint, GLsizei, //--
	GLsizei *, GLint *,
	GLenum *, GLchar *) = [](auto... args)
{
	return ::glGetActiveAttrib(args...);
};

void(*const glGetActiveUniform)(GLuint, GLuint, GLsizei, //--
	GLsizei *, GLint *,
	GLenum *, GLchar *) = [](auto... args)
{
	return ::glGetActiveUniform(args...);
};

void(*const glGetAttachedShaders)(GLuint, GLsizei,
	GLsizei *, GLuint *) = [](auto... args) //--
{
	return ::glGetAttachedShaders(args...);
};

void(*const glBindAttribLocation)(GLuint, //--
	GLuint,
	const GLchar *
	) = [](auto... args)
{
	return ::glBindAttribLocation(args...);
};



void(*const glGetBooleanv)(GLenum, GLboolean *) = [](auto... args)  //--
{
	return ::glGetBooleanv(args...);
};

void(*const glGetBufferParameteriv)(GLenum,
	GLenum,
	GLint *) = [](auto... args) //--
{
	return ::glGetBufferParameteriv(args...);
};



void(*const glGetFloatv)(GLenum, GLfloat *) = [](auto... args) {//--
	return ::glGetFloatv(args...);
};

void(*const glGetFramebufferAttachmentParameteriv)(GLenum,
	GLenum,
	GLenum,
	GLint *) = [](auto... args) //--
{
	return ::glGetFramebufferAttachmentParameteriv(args...);
};




void(*const glGetRenderbufferParameteriv)(GLenum,
	GLenum,
	GLint *) = [](auto... args) //--
{
	return ::glGetRenderbufferParameteriv(args...);
};



void(*const glGetShaderSource)(GLuint,//-- 
	GLsizei,
	GLsizei *,
	GLchar *) = [](auto... args)
{
	return ::glGetShaderSource(args...);
};



void(*const glGetTexParameterfv)(GLenum,//-- 
	GLenum,
	GLfloat *) = [](auto... args)
{
	return ::glGetTexParameterfv(args...);
};

void(*const glGetTexParameteriv)(GLenum,
	GLenum,
	GLint *) = [](auto... args) //-- 
{
	return ::glGetTexParameteriv(args...);
};

void(*const glGetUniformfv)(GLuint,
	GLint,
	GLfloat *) = [](auto... args) //-- 
{
	return ::glGetUniformfv(args...);
};

void(*const glGetUniformiv)(GLuint,
	GLint,
	GLint *) = [](auto... args) {//-- 
	return ::glGetUniformiv(args...);
};


void(*const glGetVertexAttribfv)(GLuint,
	GLenum,
	GLfloat *) = [](auto... args) {//-- 
	return ::glGetVertexAttribfv(args...);
};

void(*const glGetVertexAttribiv)(GLuint,
	GLenum,
	GLint *) = [](auto... args) {//-- 
	return ::glGetVertexAttribiv(args...);
};

void(*const glGetVertexAttribPointerv)(GLuint,
	GLenum,
	void **) = [](auto... args) {//-- 
	return ::glGetVertexAttribPointerv(args...);
};

void(*const glHint)(GLenum, GLenum) = [](auto... args) {//-- 
	return ::glHint(args...);
};

GLboolean(*const glIsBuffer)(GLuint) = [](auto... args) {//-- 
	return ::glIsBuffer(args...);
};

GLboolean(*const glIsEnabled)(GLenum) = [](auto... args) {//-- 
	return ::glIsEnabled(args...);
};

GLboolean(*const glIsFramebuffer)(GLuint) = [](auto... args) {//-- 
	return ::glIsFramebuffer(args...);
};

GLboolean(*const glIsProgram)(GLuint) = [](auto... args) {//-- 
	return ::glIsProgram(args...);
};

GLboolean(*const glIsRenderbuffer)(GLuint) = [](auto... args) {//-- 
	return ::glIsRenderbuffer(args...);
};

GLboolean(*const glIsShader)(GLuint) = [](auto... args) {//-- 
	return ::glIsShader(args...);
};

GLboolean(*const glIsTexture)(GLuint) = [](auto... args) {//-- 
	return ::glIsTexture(args...);
};



void(*const glPixelStorei)(GLenum, GLint) = [](auto... args) {//-- 
	return ::glPixelStorei(args...);
};

void(*const glPolygonOffset)(GLfloat, GLfloat) = [](auto... args) {//-- 
	return ::glPolygonOffset(args...);
};

void(*const glSampleCoverage)(GLfloat,
	GLboolean) = [](auto... args) {//-- 
	return ::glSampleCoverage(args...);
};

void(*const glScissor)(GLint, GLint,
	GLsizei, GLsizei) = [](auto... args) {//-- 
	return ::glScissor(args...);
};

void(*const glStencilFuncSeparate)(GLenum, GLenum,
	GLint, GLuint) = [](auto... args) {//-- 
	return ::glStencilFuncSeparate(args...);
};


void(*const glStencilMaskSeparate)(GLenum,
	GLuint) = [](auto... args) {//-- 
	return ::glStencilMaskSeparate(args...);
};


void(*const glStencilOpSeparate)(GLenum,
	GLenum,
	GLenum,
	GLenum) = [](auto... args) {//-- 
	return ::glStencilOpSeparate(args...);
};



void(*const glValidateProgram)(GLuint) = [](auto... args) {//-- 
	return ::glValidateProgram(args...);
};

void(*const glVertexAttrib1f)(GLuint,
	GLfloat) = [](auto... args) {//-- 
	return ::glVertexAttrib1f(args...);
};

void(*const glVertexAttrib1fv)(GLuint, const GLfloat *) = [](auto... args) {
	return ::glVertexAttrib1fv(args...);
};

void(*const glVertexAttrib2f)(GLuint,
	GLfloat, GLfloat) = [](auto... args) {//-- 
	return ::glVertexAttrib2f(args...);
};

void(*const glVertexAttrib2fv)(GLuint,
	const GLfloat *) = [](auto... args) {//-- 
	return ::glVertexAttrib2fv(args...);
};

void(*const glVertexAttrib3f)(GLuint,
	GLfloat, GLfloat, GLfloat) = [](auto... args) {//-- 
	return ::glVertexAttrib3f(args...);
};

void(*const glVertexAttrib3fv)(GLuint,
	const GLfloat *) = [](auto... args) {//-- 
	return ::glVertexAttrib3fv(args...);
};

void(*const glVertexAttrib4f)(GLuint,
	GLfloat, GLfloat, GLfloat, GLfloat) = [](auto... args) {//-- 
	return ::glVertexAttrib4f(args...);
};

void(*const glVertexAttrib4fv)(GLuint,
	const GLfloat *) = [](auto... args) //-- 
{
	return ::glVertexAttrib4fv(args...);
};


void(*const glTexParameterf)(GLenum, GLenum, GLfloat) =  ::glTexParameterf(args...); 
void(*const glTexParameterfv)(GLenum,GLenum, const GLfloat *) = ::glTexParameterfv(args...); 
void(*const glTexParameteriv)(GLenum, GLenum,const GLint *) =::glTexParameteriv(args...);

#ifndef MD_USE_GLES2
void (* const glDrawPixels)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = ::glDrawPixels;
void (* const glGetDoublev)(GLenum, GLdouble *) = ::glGetDoublev;
void (* const glPixelTransferf)(GLenum, GLfloat) = ::glPixelTransferf;
void (* const glPixelZoom)(GLfloat, GLfloat) = ::glPixelZoom;
void (* const glPointSize)(GLfloat) = ::glPointSize;
void (* const glRasterPos4d)(GLdouble, GLdouble, GLdouble, GLdouble) = ::glRasterPos4d;
#endif

 
}  // namespace mbgl
#endif // #ifdef OsID_LINUX