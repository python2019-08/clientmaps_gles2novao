//#include "glcanvas/renderState.h"
#include <cstdarg>
#include <md/util/logtool.hpp>
#include <md/drawengine/transform_state.hpp>
#include "md/drawengine/glshaders/glshd_uc.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define STB_EASY_FONT_IMPLEMENTATION
#include "md/drawengine/rscmgr/stb_easy_font.h"
 
namespace clientmap {

#define VERTEX_BUFFER_SIZE  99999

GlShdUColor::GlShdUColor()
{
    //================================================
    // source of line.vertex.glsl:
    //================================================
    m_vertShaderSource = R"RAW_GLSL(
        uniform   mat4 uMVPMat; 
        attribute vec4 aPos; 
        void main() { 
            gl_Position = uMVPMat * aPos; 
            gl_PointSize = 2.0;
        } 
    )RAW_GLSL";
    // gl_PointSize = 4.0;
    
    //================================================
    // source of line.fragment.glsl:
    //================================================
    m_fragShaderSource = R"RAW_GLSL(
		precision mediump float;
        uniform vec4 uColor; 
        void main() {  
            gl_FragColor = uColor; 
        } 
    )RAW_GLSL";

    // An optional shader description printed on compile failure
    m_description = "GlShd_uc";

	m_vertexBuf.resize(VERTEX_BUFFER_SIZE);
}

GlShdUColor::~GlShdUColor()
{ 
     
}

// virtual function 
void GlShdUColor::initLocation() 
{
    //---get attribute locations
    m_aPosLoc   = MDGL_CHECK(md::glGetAttribLocation(m_progId, "aPos"));

    //---get uniform locations
    m_uMVPMatLoc= MDGL_CHECK(md::glGetUniformLocation(m_progId, "uMVPMat"));
    m_uColorLoc = MDGL_CHECK(md::glGetUniformLocation(m_progId, "uColor"));
} 

void GlShdUColor::enableVertexAttrib()
{
    //md::glEnableVertexAttribArray(m_aPosLoc); 
}
 
int GlShdUColor::drawLineStrip(
    const TransformState&  aCamera,
    float*     aVertArray, 
    uint16_t*  aIndices,
    int32_t    aIndexNum,
    const ColorF& aColor)
{
    if ( aVertArray == NULL ||  aIndices == NULL || aIndexNum == 0)
    {
        return -1;
    }

    if (!this->use())
    {
        return  -1;
    }

    MDGL_CHECK(md::glEnable(GL_BLEND));
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDisable(GL_CULL_FACE));

#ifdef  USE_ViewCamera_AS_TransformState
    auto mat = aCamera.getVMat() * aCamera.getMMatS();
#else
    auto mat = aCamera.getVMat();
#endif
    mat = aCamera.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(m_uMVPMatLoc, 1, 
        GL_FALSE, &mat[0][0])) ;
 
    MDGL_CHECK(md::glUniform4f(m_uColorLoc, 
        aColor.r, aColor.g, aColor.b, aColor.a)  );
    // Ω¯––‰÷»æ
    MDGL_CHECK(md::glEnableVertexAttribArray(m_aPosLoc) );
    MDGL_CHECK(md::glVertexAttribPointer(m_aPosLoc, 2, 
        GL_FLOAT, GL_FALSE, 0, aVertArray) );

    //md::glFrontFace(GL_CCW);
    MDGL_CHECK(md::glDrawElements(GL_LINE_STRIP, aIndexNum, 
        GL_UNSIGNED_SHORT, aIndices) );
     
    MDGL_CHECK(md::glDisableVertexAttribArray(m_aPosLoc));

    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST) );
    MDGL_CHECK(md::glDisable(GL_CULL_FACE) );
    MDGL_CHECK(md::glEnable(GL_BLEND)) ;
    return 0;
}


int GlShdUColor::drawLineStrip(
    const TransformState&  aCamera,
    float*     aVertArray, 
    int32_t    aVertNum,
    const ColorF& aColor)
{
    if (aVertArray == NULL || aVertNum <= 0)
    {
        return -1;
    }

    if (!this->use())
    {
        return  -1;
    }

#ifdef  USE_ViewCamera_AS_TransformState
	glm::mat4  matS = glm::scale(glm::mat4(1), glm::vec3(0.999, 0.999, 0.999));
    auto mat = aCamera.getVMat() * aCamera.getMMatS()*matS;
#else
    auto mat = aCamera.getVMat();
#endif
    mat = aCamera.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]) );

    MDGL_CHECK(md::glUniform4f(m_uColorLoc, 
        aColor.r, aColor.g, aColor.b, aColor.a) );

    MDGL_CHECK(md::glEnableVertexAttribArray(m_aPosLoc) );
    MDGL_CHECK(md::glVertexAttribPointer(m_aPosLoc, 2, GL_FLOAT,
        GL_FALSE, 0, aVertArray) );
     
    MDGL_CHECK(md::glDrawArrays(GL_LINE_STRIP, 0, aVertNum) );

    MDGL_CHECK(md::glDisableVertexAttribArray(m_aPosLoc) );
    return 0;
}


void GlShdUColor::draw3dText(const TransformState&  aCamera,
    int aBpxPosx,
    int aBpxPosy,
    const ColorF& aColor,
    const std::string& _text
)
{
    if (!this->use())
    {
        return;
    }

    MDGL_CHECK(md::glDisable(GL_CULL_FACE) );
    MDGL_CHECK(md::glDisable(GL_BLEND) );
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST) );
    MDGL_CHECK(md::glDepthMask(GL_FALSE) );

    GLint boundbuffer = -1;
    MDGL_CHECK(md::glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &boundbuffer) );
    MDGL_CHECK(md::glBindBuffer(GL_ARRAY_BUFFER, 0) );

    TransformState&  cam = const_cast<TransformState&>(aCamera);
    glm::dvec3 pos = cam.bpx2GroundPx(Pt3D{ (double)aBpxPosx, (double)aBpxPosy });

    glm::mat4 sMat = glm::scale(glm::mat4(1), glm::vec3(1.2, 1.2, 1.2));
    glm::mat4 tMat = glm::translate(glm::mat4(1), glm::vec3(pos.x, pos.y, 1));
    glm::mat4 vpMat = aCamera.getVPMat();// 3D-projection-matrix * vMat
    vpMat = vpMat * tMat * sMat;
    MDGL_CHECK(md::glUniformMatrix4fv(m_uMVPMatLoc, 1, GL_FALSE, 
        &vpMat[0][0]) );

    // Display Tangram info messages 
    MDGL_CHECK(md::glUniform4f(m_uColorLoc, 
        aColor.r, aColor.g, aColor.b, aColor.a) );
    //-----------------------------------------
    std::vector<glm::vec2> vertices;
    int nquads;

    nquads = stb_easy_font_print(0, 0, (char*)_text.c_str(), NULL,
        m_vertexBuf.data(), VERTEX_BUFFER_SIZE);

    float* data = reinterpret_cast<float*>(m_vertexBuf.data());

    vertices.reserve(nquads * 6);
    for (int quad = 0, stride = 0; quad < nquads; ++quad, stride += 16)
    {
        vertices.push_back({ data[(0 * 4) + stride], data[(0 * 4) + 1 + stride] });
        vertices.push_back({ data[(1 * 4) + stride], data[(1 * 4) + 1 + stride] });
        vertices.push_back({ data[(2 * 4) + stride], data[(2 * 4) + 1 + stride] });
        vertices.push_back({ data[(2 * 4) + stride], data[(2 * 4) + 1 + stride] });
        vertices.push_back({ data[(3 * 4) + stride], data[(3 * 4) + 1 + stride] });
        vertices.push_back({ data[(0 * 4) + stride], data[(0 * 4) + 1 + stride] });
    }
    //vertexLayout.enable(rs, *m_shader, 0, (void*)vertices.data());
    MDGL_CHECK(md::glEnableVertexAttribArray(m_aPosLoc) );
    MDGL_CHECK(md::glVertexAttribPointer(m_aPosLoc, 2, GL_FLOAT, GL_FALSE,
        0, vertices.data()) );

    MDGL_CHECK(md::glDrawArrays(GL_TRIANGLES, 0, nquads * 6) );

    MDGL_CHECK(md::glDisableVertexAttribArray(m_aPosLoc) );
    //-----------------------------------------
    MDGL_CHECK(md::glBindBuffer(GL_ARRAY_BUFFER, boundbuffer) );

    MDGL_CHECK(md::glEnable(GL_CULL_FACE));
    MDGL_CHECK(md::glEnable(GL_BLEND));
    MDGL_CHECK(md::glEnable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDepthMask(GL_TRUE));
}


void GlShdUColor::draw2dText(const TransformState&  aCamera,
    int aPosx,
    int aPosy,
    const ColorF& aColor,
	const std::string& _text
)
{ 
	if (!this->use())
	{
		return;
	}

    MDGL_CHECK(md::glDisable(GL_CULL_FACE) );
    MDGL_CHECK(md::glDisable(GL_BLEND) );
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST) );
    MDGL_CHECK(md::glDepthMask(GL_FALSE) );

	GLint boundbuffer = -1;
    MDGL_CHECK(md::glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &boundbuffer) );
    MDGL_CHECK(md::glBindBuffer(GL_ARRAY_BUFFER, 0) );

    glm::mat4 sMat  = glm::scale(glm::mat4(1), glm::vec3(1.2, 1.2, 1.2));
    glm::mat4 tMat = glm::translate(glm::mat4(1), glm::vec3(aPosx, aPosy, 1));
	glm::mat4 orthoProj = aCamera.getPMatHUD();
    orthoProj = orthoProj * tMat * sMat;
    MDGL_CHECK(md::glUniformMatrix4fv(m_uMVPMatLoc, 1, GL_FALSE, 
        &orthoProj[0][0]) );

	// Display Tangram info messages 
    MDGL_CHECK(md::glUniform4f(m_uColorLoc, 
        aColor.r, aColor.g, aColor.b, aColor.a) );
	//-----------------------------------------
	std::vector<glm::vec2> vertices;
	int nquads;

	nquads = stb_easy_font_print(aPosx, aPosy, (char*)_text.c_str(), NULL,
		m_vertexBuf.data(), VERTEX_BUFFER_SIZE);

	float* data = reinterpret_cast<float*>(m_vertexBuf.data());

	vertices.reserve(nquads * 6);
	for (int quad = 0, stride = 0; quad < nquads; ++quad, stride += 16)
	{
		vertices.push_back({ data[(0 * 4) + stride] - aPosx,
                             data[(0 * 4) + 1 + stride] - aPosy
                            });
		vertices.push_back({ data[(1 * 4) + stride] - aPosx,
                             data[(1 * 4) + 1 + stride] - aPosy });
		vertices.push_back({ data[(2 * 4) + stride] - aPosx,
                             data[(2 * 4) + 1 + stride] - aPosy });
		vertices.push_back({ data[(2 * 4) + stride] - aPosx,
                             data[(2 * 4) + 1 + stride] - aPosy });
		vertices.push_back({ data[(3 * 4) + stride] - aPosx,
                             data[(3 * 4) + 1 + stride] - aPosy });
		vertices.push_back({ data[(0 * 4) + stride] - aPosx,
                             data[(0 * 4) + 1 + stride] - aPosy });
	}
	//vertexLayout.enable(rs, *m_shader, 0, (void*)vertices.data());
    MDGL_CHECK(md::glEnableVertexAttribArray(m_aPosLoc)  );
    MDGL_CHECK(md::glVertexAttribPointer(m_aPosLoc, 2, GL_FLOAT, 
        GL_FALSE, 0, vertices.data()) );

    MDGL_CHECK(md::glDrawArrays(GL_TRIANGLES, 0, nquads * 6) );

    MDGL_CHECK(md::glDisableVertexAttribArray(m_aPosLoc) );
	//-----------------------------------------
    MDGL_CHECK(md::glBindBuffer(GL_ARRAY_BUFFER, boundbuffer));

    MDGL_CHECK(md::glEnable(GL_CULL_FACE));
    MDGL_CHECK(md::glEnable(GL_BLEND));
    MDGL_CHECK(md::glEnable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDepthMask(GL_TRUE));
}
 

}// namespace clientmap {
