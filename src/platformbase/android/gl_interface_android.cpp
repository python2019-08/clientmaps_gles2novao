#include "platformbase/gl_interface.h"



#ifdef OsID_ANDROID

#define GL_CHECK(cmd)  (cmd) 

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
    GL_CHECK("glGetString");
    return result;
}

void  glClear(GLbitfield mask) {
    GL_CHECK(::glClear(mask));
}
void  glLineWidth(GLfloat width) {
    GL_CHECK(::glLineWidth(width));
}
void  glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    GL_CHECK(::glViewport(x, y, width, height));
}

void  glEnable(GLenum id) {
    GL_CHECK(::glEnable(id));
}
void  glDisable(GLenum id) {
    GL_CHECK(::glDisable(id));
}
void  glDepthFunc(GLenum func) {
    GL_CHECK(::glDepthFunc(func));
}
void  glDepthMask(GLboolean flag) {
    GL_CHECK(::glDepthMask(flag));
}
void  glDepthRangef(GLfloat n, GLfloat f) {
    GL_CHECK(::glDepthRangef(n, f));
}
void  glClearDepthf(GLfloat d) {
    GL_CHECK(::glClearDepthf(d));
}
void  glBlendFunc(GLenum sfactor, GLenum dfactor) {
    GL_CHECK(::glBlendFunc(sfactor, dfactor));
}
void  glStencilFunc(GLenum func, GLint ref, GLuint mask) {
    GL_CHECK(::glStencilFunc(func, ref, mask));
}
void  glStencilMask(GLuint mask) {
    GL_CHECK(::glStencilMask(mask));
}
void  glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
    GL_CHECK(::glStencilOp(fail, zfail, zpass));
}
void  glClearStencil(GLint s) {
    GL_CHECK(::glClearStencil(s));
}
void  glColorMask(GLboolean red, GLboolean green, 
                     GLboolean blue, GLboolean alpha) 
{
    GL_CHECK(::glColorMask(red, green, blue, alpha));
}
void  glCullFace(GLenum mode) {
    GL_CHECK(::glCullFace(mode));
}
void  glFrontFace(GLenum mode) {
    GL_CHECK(::glFrontFace(mode));
}
void  glClearColor(GLclampf red, 
                      GLclampf green, 
                      GLclampf blue, 
                      GLclampf alpha) 
{
    GL_CHECK(::glClearColor(red, green, blue, alpha));
}
void  glGetIntegerv(GLenum pname, GLint *params )
{
    GL_CHECK(::glGetIntegerv(pname, params ));
}

// Program
void  glUseProgram(GLuint program)
{
    GL_CHECK(::glUseProgram(program));
}
void  glDeleteProgram(GLuint program) {
    GL_CHECK(::glDeleteProgram(program));
}
void  glDeleteShader(GLuint shader) {
    GL_CHECK(::glDeleteShader(shader));
}
GLuint  glCreateShader(GLenum type) {
    auto result = ::glCreateShader(type);
    GL_CHECK("glCreateShader");
    return result;
}
GLuint  glCreateProgram() {
    auto result = ::glCreateProgram();
    GL_CHECK("glCreateProgram");
    return result;
}

void  glCompileShader(GLuint shader) {
    GL_CHECK(::glCompileShader(shader));
}
void  glAttachShader(GLuint program, GLuint shader) {
    GL_CHECK(::glAttachShader(program,shader));
}
 

void  glLinkProgram(GLuint program) {
    GL_CHECK(::glLinkProgram(program));
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
    GL_CHECK(::glShaderSource(shader, count, source, length));
}

void  glGetShaderInfoLog(GLuint shader, GLsizei bufSize,
    GLsizei *length, GLchar *infoLog) 
{
    GL_CHECK(::glGetShaderInfoLog(shader, bufSize, length, infoLog));
}
void  glGetProgramInfoLog(GLuint program, GLsizei bufSize,
    GLsizei *length, GLchar *infoLog) 
{
    GL_CHECK(::glGetProgramInfoLog(program, bufSize, length, infoLog));
}
GLint  glGetUniformLocation(GLuint program, const GLchar *name)
{
    auto result = ::glGetUniformLocation(program, name);
    GL_CHECK("glGetUniformLocation");
    return result;
}
GLint  glGetAttribLocation(GLuint program, const GLchar *name) {
    auto result = ::glGetAttribLocation(program, name);
    GL_CHECK("glGetAttribLocation");
    return result;
}
 
void  glGetProgramiv(GLuint program, GLenum pname, GLint *params) {
    GL_CHECK(::glGetProgramiv(program,pname,params));
}
void  glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
    GL_CHECK(::glGetShaderiv(shader,pname, params));
}

// Buffers
void  glBindBuffer(GLenum target, GLuint buffer) {
    GL_CHECK(::glBindBuffer(target, buffer));
}
 

void  glDeleteBuffers(GLsizei n, const GLuint *buffers) {
    GL_CHECK(::glDeleteBuffers(n, buffers));
}
void  glGenBuffers(GLsizei n, GLuint *buffers) {
    GL_CHECK(::glGenBuffers(n, buffers));
}
void  glBufferData(GLenum target, GLsizeiptr size, 
    const void *data, GLenum usage) 
{
    GL_CHECK(::glBufferData(target, size, data, usage));
}
void  glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data) {
    GL_CHECK(::glBufferSubData(target, offset, size, data));
}
void  glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height,
                    GLenum format, GLenum type, GLvoid* pixels) {
    GL_CHECK( ::glReadPixels(x, y, width, height, 
        format, type, pixels) );
}

// Texture
void  glBindTexture(GLenum target, GLuint texture ) {
    GL_CHECK(::glBindTexture(target, texture ));
}
void  glActiveTexture(GLenum texture) {
    GL_CHECK(::glActiveTexture(texture));
}

void  glGenTextures(GLsizei n, GLuint *textures ) {
    GL_CHECK(::glGenTextures(n, textures ));
}
void  glDeleteTextures(GLsizei n, const GLuint *textures) {
    GL_CHECK(::glDeleteTextures(n, textures));
}
void  glTexParameteri(GLenum target, GLenum pname, GLint param )
{
    GL_CHECK(::glTexParameteri(target, pname, param ));
}
void  glTexImage2D(GLenum target, GLint level,
                    GLint internalFormat, 
                    GLsizei width, GLsizei height, GLint border, 
                    GLenum format, GLenum type, const GLvoid *pixels) 
{
    GL_CHECK(::glTexImage2D(target, level, 
        internalFormat, 
        width, height, border, 
        format, type, pixels)); 
}

void  glTexSubImage2D(GLenum target, GLint level,
    GLint   xoffset,  GLint   yoffset, 
    GLsizei width,    GLsizei height,
    GLenum  format,   GLenum  type,  const GLvoid *pixels ) 
{
    GL_CHECK( ::glTexSubImage2D(target, level,
        xoffset, yoffset, 
        width, height, 
        format, type, pixels
    )); }

void  glGenerateMipmap(GLenum target) {
    GL_CHECK( ::glGenerateMipmap(target));
}

void  glEnableVertexAttribArray(GLuint index) {
    GL_CHECK(::glEnableVertexAttribArray(index));
}
void  glDisableVertexAttribArray(GLuint index) {
    GL_CHECK(::glDisableVertexAttribArray(index));
}
void  glVertexAttribPointer(GLuint index,  GLint size,
    GLenum type,  GLboolean normalized,
    GLsizei stride,  const void *pointer) 
{
    GL_CHECK(::glVertexAttribPointer(index, 
        size, type,  normalized, 
        stride,  pointer));
}

void  glDrawArrays(GLenum mode, GLint first, GLsizei count )
{
    GL_CHECK(::glDrawArrays(mode, first, count ));
}
void  glDrawElements(GLenum mode, GLsizei count,
                        GLenum type, const GLvoid *indices ) 
{
    GL_CHECK(::glDrawElements(mode, count, type, indices ));
}

void  glUniform1f(GLint location, GLfloat v0) {
    GL_CHECK(::glUniform1f(location, v0));
}
void  glUniform2f(GLint location, GLfloat v0, GLfloat v1) {
    GL_CHECK(::glUniform2f(location, v0, v1));
}
void  glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    GL_CHECK(::glUniform3f(location, v0, v1, v2));
}
void  glUniform4f(GLint location, 
    GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GL_CHECK(::glUniform4f(location, v0, v1, v2, v3));
}

void  glUniform1i(GLint location, GLint v0) {
    GL_CHECK(::glUniform1i(location, v0));
}
void  glUniform2i(GLint location, GLint v0, GLint v1) {
    GL_CHECK(::glUniform2i(location, v0, v1));
}
void  glUniform3i(GLint location, GLint v0, GLint v1, GLint v2) 
{
    GL_CHECK(::glUniform3i(location, v0, v1, v2));
}
void  glUniform4i(GLint location, 
    GLint v0, GLint v1, GLint v2, GLint v3) 
{
    GL_CHECK(::glUniform4i(location, v0, v1, v2, v3));
}

void  glUniform1fv(GLint location, GLsizei count, const GLfloat *value) 
{
    GL_CHECK(::glUniform1fv(location, count, value));
}
void  glUniform2fv(GLint location, GLsizei count, const GLfloat *value) {
    GL_CHECK(::glUniform2fv(location, count, value));
}
void  glUniform3fv(GLint location, GLsizei count, const GLfloat *value) {
    GL_CHECK(::glUniform3fv(location, count, value));
}
void  glUniform4fv(GLint location, GLsizei count, const GLfloat *value) {
    GL_CHECK(::glUniform4fv(location, count, value));
}
void  glUniform1iv(GLint location, GLsizei count, const GLint *value) {
    GL_CHECK(::glUniform1iv(location, count, value));
}
void  glUniform2iv(GLint location, GLsizei count, const GLint *value) {
    GL_CHECK(::glUniform2iv(location, count, value));
}
void  glUniform3iv(GLint location, GLsizei count, const GLint *value) {
    GL_CHECK(::glUniform3iv(location, count, value));
}
void  glUniform4iv(GLint location, GLsizei count, const GLint *value) {
    GL_CHECK(::glUniform4iv(location, count, value));
}

void  glUniformMatrix2fv(GLint location,
    GLsizei count, 
    GLboolean transpose, 
    const GLfloat *value
) 
{
    GL_CHECK(::glUniformMatrix2fv(location, count, transpose, value));
}
void  glUniformMatrix3fv(GLint location,
    GLsizei count, 
    GLboolean transpose, 
    const GLfloat *value
) 
{
    GL_CHECK(::glUniformMatrix3fv(location, count, transpose, value));
}
void  glUniformMatrix4fv(GLint location,
    GLsizei count, 
    GLboolean transpose, 
    const GLfloat *value
) 
{
    GL_CHECK(::glUniformMatrix4fv(location, count, transpose, value));
}

void  glFinish(void) {
    GL_CHECK(::glFinish());
}

// Framebuffer
void  glBindFramebuffer(GLenum target, GLuint framebuffer) {
    GL_CHECK( ::glBindFramebuffer(target, framebuffer));
}

void  glGenFramebuffers(GLsizei n, GLuint *framebuffers) {
    GL_CHECK(::glGenFramebuffers(n, framebuffers));
}

void  glFramebufferTexture2D(GLenum target,
    GLenum attachment, 
    GLenum textarget,
    GLuint texture, 
    GLint level) 
{
    GL_CHECK(::glFramebufferTexture2D(target, 
        attachment, textarget, texture, level)
    );
}

void  glRenderbufferStorage(GLenum target,
    GLenum internalformat, 
    GLsizei width,
    GLsizei height
) 
{
    GL_CHECK( ::glRenderbufferStorage(target, 
        internalformat, width, height)
    );
}

void  glFramebufferRenderbuffer(GLenum target,
    GLenum attachment,
    GLenum renderbuffertarget, 
    GLuint renderbuffer) 
{
    GL_CHECK(::glFramebufferRenderbuffer(target, 
        attachment, 
        renderbuffertarget, 
        renderbuffer));
}

void  genRenderbuffers(GLsizei n, GLuint *renderbuffers) {
    GL_CHECK(::glGenRenderbuffers(n, renderbuffers));
}

void  glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
    GL_CHECK(::glBindRenderbuffer(target, renderbuffer));
}

void  glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers) {
    GL_CHECK(::glDeleteFramebuffers(n, framebuffers));
}

void  glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) {
    GL_CHECK(::glDeleteRenderbuffers(n, renderbuffers));
}

GLenum  glCheckFramebufferStatus(GLenum target) {
    GLenum status = ::glCheckFramebufferStatus(target);
    GL_CHECK("glCheckFramebufferStatus");
    return status;
}

// mapbuffer
void*  glMapBuffer(GLenum target, GLenum access) {
    void* result = NULL;// ::glMapBuffer(target, access);//gmq
    //GL_CHECK("glMapBuffer");
    return result;
}

GLboolean  glUnmapBuffer(GLenum target) {
    GLboolean result = false;// ::glUnmapBuffer(target);//gmq
    //GL_CHECK("glUnmapBuffer");
    return result;
}
 

// VAO
void  glBindVertexArray(GLuint array) 
{
    //GL_CHECK( ::glBindVertexArray(array));
}

void  glDeleteVertexArrays(GLsizei n, const GLuint *arrays) 
{
    //GL_CHECK( ::glDeleteVertexArrays(n, arrays));
}
void  glGenVertexArrays(GLsizei n, GLuint *arrays) 
{
    //GL_CHECK( ::glGenVertexArrays(n, arrays));
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

} // namespace md
#endif
