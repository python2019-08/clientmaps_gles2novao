#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <md/util/merc_prj.hpp>
#include <md/util/math_tools.hpp>
#include <md/util/color.hpp>

#include <md/util/tile_cover_bound.hpp>

#include <md/drawengine/transform_state.hpp>

#include "md/drawengine/glshaders/glshd_tex_uc.h"  
#include "md/drawengine/glshaders/glshd_uc.h"  
#include "md/drawengine/glshaders/glshaders.h"
#include "md/drawengine/rscmgr/gltexmgr.h"
#include "md/drawengine/drawcontext.h"

#include "md/mapengine/scenelayer/scenelayer_obj_def.hpp"

#include "md/drawengine/glpainters/glpaint_line.h"

using namespace clientmap;//  using namespace in ".cpp" file

GLPaintLine::GLPaintLine(const GlShaders&  aGlShaders,
    const GlTexMgr&   aGlTexMgr)
    : _glShaders(const_cast<GlShaders&>(aGlShaders) )
	, _glTexMgr( const_cast<GlTexMgr&>(aGlTexMgr) )
{

}
GLPaintLine::~GLPaintLine() = default;
 


//draw thick line
void GLPaintLine::drawPoints(const CPaintContext& aDC,
    const ColorI& aColor, 
    const Pt2D&   aOrigin,
    double   aScale,
    float*   aVertArr,
    int32_t  aVertCnt
)
{ 
    return;
    auto& cam = const_cast<TransformState&>(aDC._transState);
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getUColorShader();
    assert(shd.use());

    MDGL_CHECK(md::glEnable(GL_BLEND));
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glEnable(GL_CULL_FACE));

#ifdef  USE_ViewCamera_AS_TransformState
    glm::dvec3 tmp = cam.bpx2GroundPx(Pt3D{ aOrigin.x,aOrigin.y,0 });

    auto mat = cam.getVMat();
    mat = glm::translate(mat, glm::vec3(tmp.x, tmp.y, 0.1));
    mat = glm::scale(mat, glm::vec3(aScale, aScale, aScale));

    mat = cam.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]));


    ColorF c = aColor.toColorF();
    //MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a));     
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, 1.0,0.0,  0.0, 1.0));
  

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 2, GL_FLOAT,
        GL_FALSE, sizeof(XyUv), (void*)aVertArr));
     
     
    MDGL_CHECK(md::glDrawArrays(GL_POINTS, 0, aVertCnt));
    //MDGL_CHECK(md::glFrontFace(GL_CCW));
     
    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc));

    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
#endif
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    MDGL_CHECK(md::glEnable(GL_BLEND));
}


void extrudeLine_01( double  aLineWidth,
    float* aVertXyArr,
    int32_t  aVertXyCnt,
    std::vector<XyUv>& aTriStripVerts
)
{
    if (aVertXyCnt < 2)
    {
        return;
    }
    //--------layout
    XyUv   tmpPts[10];
    double totalLen = 0.0;
    glm::dvec2 e;
    glm::vec2 N;
    glm::vec2 S;
    glm::vec2 NE;
    glm::vec2 NW;
    glm::vec2 SW;
    glm::vec2 SE;

    std::vector<XyUv>& triStripVerts= aTriStripVerts;

    glm::vec2* ptArr = (glm::vec2*)aVertXyArr;
    for (int i = 0; i < aVertXyCnt -1; i++)
    {
        glm::vec2  p1 = ptArr[i];
        glm::vec2  p2 = ptArr[i + 1];

        //---- construct thick line
        //---- ( round LineJoin ) 
        e = p2 - p1;
        double curSegLen = glm::length(e);
        totalLen += curSegLen;
        e = glm::normalize(e);
        e = e * aLineWidth * 0.5;
        N = { -e.y , e.x };
        S = { -N.x, -N.y };//S = -N;
        NE = { N.x + e.x, N.y + e.y };//NE =N+e;
        NW = { N.x - e.x, N.y - e.y };//NW =N-e;
        SW = { -NE.x, -NE.y }; // SW = -NE;
        SE = { -NW.x, -NW.y }; // SE = -NW;


        int iVert = 0;
        if (i == 0) {
            tmpPts[iVert++] = { p1.x + SW.x, p1.y + SW.y, 0.0, 0.0 };// p1+SW,1+
            tmpPts[iVert++] = { p1.x + SW.x, p1.y + SW.y, 0.0, 0.0 };// p1+SW,1
            tmpPts[iVert++] = { p1.x + NW.x, p1.y + NW.y, 0.0, 1.0 };// p1+NW,2
        }
        tmpPts[iVert++] = { p1.x + S.x,  p1.y + S.y, 0.5, 0.0 };// p1+S,3
        tmpPts[iVert++] = { p1.x + N.x,  p1.y + N.y, 0.5, 1.0 };// p1+N,4
        tmpPts[iVert++] = { p2.x + S.x,  p2.y + S.y, 0.5, 0.0 };// p2+S,5
        tmpPts[iVert++] = { p2.x + N.x,  p2.y + N.y, 0.5, 1.0 };// p2+N,6
        if (i == aVertXyCnt - 2) {
            tmpPts[iVert++] = { p2.x + SE.x, p2.y + SE.y, 1.0, 0.0 };// p2+SE,7
            tmpPts[iVert++] = { p2.x + NE.x, p2.y + NE.y, 1.0, 1.0 };// p2+NE,8
            tmpPts[iVert++] = { p2.x + NE.x, p2.y + NE.y, 0.0, 0.0 };// p2+NE,8+
        }

        triStripVerts.insert(triStripVerts.end(), tmpPts, tmpPts + iVert);
    }

}
  

void GLPaintLine::drawSolidLineInPx(const CPaintContext& aDC,
    const ColorI& aColor,
    double        aLineWidth,
    float*   aVertXyArr,
    int32_t  aVertXyCnt
)
{
    if (aVertXyCnt < 2)
    {
        return;
    }
    //--------layout      
    std::vector<XyUv> triStripVerts;
    extrudeLine_01(aLineWidth, aVertXyArr, aVertXyCnt,
        triStripVerts);

    //--------draw 
    auto& cam = const_cast<TransformState&>(aDC._transState);
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getATexUCShader(); 
    assert(shd.use());

    MDGL_CHECK(md::glEnable(GL_BLEND));
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glEnable(GL_CULL_FACE));

#ifdef  USE_ViewCamera_AS_TransformState 
    auto mat = cam.getVPMat();
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]));

    ColorF c = aColor.toColorF();
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a));

    //if (aLineWidth > 6.01) 
    {
        //MDGL_CHECK(md::glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
        MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        _glTexMgr.bindLAA_alphaTex();
    }
    //else { 
    //    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //    _glTexMgr.bindLGaussAATexture();
    //}

    float* pXyUv = (float*)triStripVerts.data();
    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 2, GL_FLOAT,
        GL_FALSE, sizeof(XyUv), (void*)pXyUv));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aUVLoc, 2, GL_FLOAT,
        GL_FALSE, sizeof(XyUv), (void*)(pXyUv + 2)));

    MDGL_CHECK(md::glFrontFace(GL_CCW));
    MDGL_CHECK(md::glDrawArrays(GL_TRIANGLE_STRIP, 0, triStripVerts.size()));
    //MDGL_CHECK(md::glFrontFace(GL_CCW));


    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc));

    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
#endif//#ifdef  USE_ViewCamera_AS_TransformState 
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    MDGL_CHECK(md::glEnable(GL_BLEND));

}

void GLPaintLine::drawThinSolidLineInPx(const CPaintContext& aDC,
    const ColorI& aColor,
    double        aLineWidth, 
    float*   aVertXyArr,
    int32_t  aVertXyCnt
)
{ 
    if (aVertXyCnt < 2)
    {
        return;
    }
    //--------layout      
    std::vector<XyUv> triStripVerts;
    extrudeLine_01(aLineWidth, aVertXyArr, aVertXyCnt,
        triStripVerts); 
     
    //--------draw 
    auto& cam = const_cast<TransformState&>(aDC._transState);
    //auto& shd = (const_cast<GlShaders&>(_glShaders)).getATexUCShader();
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getThinLineShader();
    assert(shd.use());

    MDGL_CHECK(md::glEnable(GL_BLEND));
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glEnable(GL_CULL_FACE));

#ifdef  USE_ViewCamera_AS_TransformState 
    auto mat = cam.getVPMat();  
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]));

    ColorF c = aColor.toColorF();
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a));
 
    //if (aLineWidth > 6.01) 
    {
        //MDGL_CHECK(md::glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
        MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        _glTexMgr.bindLAA_alphaTex();
    }
    //else { 
    //    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //    _glTexMgr.bindLGaussAATexture();
    //}
     
    float* pXyUv =(float*)triStripVerts.data();
    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 2, GL_FLOAT,
        GL_FALSE, sizeof(XyUv), (void*)pXyUv));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aUVLoc, 2, GL_FLOAT,
        GL_FALSE, sizeof(XyUv), (void*)(pXyUv + 2)));

    MDGL_CHECK(md::glFrontFace(GL_CCW));
    MDGL_CHECK(md::glDrawArrays(GL_TRIANGLE_STRIP, 0, triStripVerts.size()));
    //MDGL_CHECK(md::glFrontFace(GL_CCW));
 

    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc));

    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
#endif//#ifdef  USE_ViewCamera_AS_TransformState 
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    MDGL_CHECK(md::glEnable(GL_BLEND));

}

//draw thick line
void GLPaintLine::drawSolidLine(const CPaintContext& aDC,
    const ColorI& aColor,
    double        aLineWidth,
    const Pt2D&   aOrigin,
    double   aScale,
    float*   aVertArr,
    int32_t  aVertCnt
)
{
    //if (aLineWidth < 1.0) {
    //    return;
    //}
    auto&   cam = const_cast<TransformState&>(aDC._transState);
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getATexUCShader();
    assert(shd.use());

    MDGL_CHECK(md::glEnable(GL_BLEND) );
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glEnable(GL_CULL_FACE));

#ifdef  USE_ViewCamera_AS_TransformState
    glm::dvec3 tmp = cam.bpx2GroundPx(Pt3D{ aOrigin.x,aOrigin.y,0 });

    auto mat = cam.getVMat();
    mat = glm::translate(mat, glm::vec3(tmp.x, tmp.y, 0.1));
    mat = glm::scale(mat, glm::vec3(aScale, aScale, aScale));

    mat = cam.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]) );
  

	ColorF c = aColor.toColorF();
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a));
 
	
    //if (aLineWidth > 1.01) 
    { 
        //MDGL_CHECK(md::glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
        MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        _glTexMgr.bindLAA_alphaTex();
    }
    //else { 
    //    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //    _glTexMgr.bindLGaussAATexture();
    //}
	      
    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 2, GL_FLOAT, 
        GL_FALSE, sizeof(XyUv), (void *)aVertArr));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aUVLoc, 2, GL_FLOAT, 
        GL_FALSE, sizeof(XyUv), (void *)(aVertArr + 2)) );
	 

	//MDGL_CHECK(md::glFrontFace(GL_CCW)); 
    MDGL_CHECK(md::glDrawArrays(GL_TRIANGLE_STRIP, 0 , aVertCnt));
	//MDGL_CHECK(md::glFrontFace(GL_CCW));

    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc));

    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
#endif
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    MDGL_CHECK(md::glEnable(GL_BLEND) );

}

//draw thin line
void GLPaintLine::drawThinSolidLine(const CPaintContext& aDC,
    const ColorI& aColor,
    double        aLineWidth,
    const Pt2D& aOrigin,
    double   aScale,
    float*   aVertArr,
    int32_t  aVertCnt
)
{
    //if (aLineWidth > 1.0){
    //    return;
    //}
    auto& cam = const_cast<TransformState&>(aDC._transState);
    //auto& shd = (const_cast<GlShaders&>(_glShaders)).getATexUCShader();
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getThinLineShader();
    assert(shd.use());

    MDGL_CHECK(md::glEnable(GL_BLEND));
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glEnable(GL_CULL_FACE));

#ifdef  USE_ViewCamera_AS_TransformState
    glm::dvec3 tmp = cam.bpx2GroundPx(Pt3D{ aOrigin.x,aOrigin.y,0 });

    auto mat = cam.getVMat();
    mat = glm::translate(mat, glm::vec3(tmp.x, tmp.y, 0.1));
    mat = glm::scale(mat, glm::vec3(aScale, aScale, aScale));

    mat = cam.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]));

    //if (aLineWidth > 1.2)
    //{
        //MDGL_CHECK(md::glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
        MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        _glTexMgr.bindLAA_alphaTex();
    //} 
    //MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //_glTexMgr.bindLGaussAATexture();

    ColorF c = aColor.toColorF();
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a)); 
     

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 2, GL_FLOAT,
        GL_FALSE, sizeof(XyUv), (void*)aVertArr));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aUVLoc, 2, GL_FLOAT,
        GL_FALSE, sizeof(XyUv), (void*)(aVertArr + 2)));     
     
    //MDGL_CHECK(md::glFrontFace(GL_CCW)); 
    MDGL_CHECK(md::glDrawArrays(GL_TRIANGLE_STRIP, 0, aVertCnt));
    //MDGL_CHECK(md::glFrontFace(GL_CCW));

    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc));

    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
#endif
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    MDGL_CHECK(md::glEnable(GL_BLEND));

}


void GLPaintLine::drawDashLine(
    const CPaintContext& aDC,
    const std::vector<float>& aDashArr,
    const ColorI& aColor,
    const Pt2D&   aOrigin,
    double   aScale,
    float*   aVertArr,
    int32_t  aVertCnt
)
{
    auto&   cam = const_cast<TransformState&>(aDC._transState);
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getATexUCShader();
    assert(shd.use());

    MDGL_CHECK(md::glEnable(GL_BLEND) );
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glEnable(GL_CULL_FACE));
#ifdef  USE_ViewCamera_AS_TransformState
    glm::dvec3 tmp = cam.bpx2GroundPx(Pt3D{ aOrigin.x,aOrigin.y,0 });

    auto mat = cam.getVMat();
    mat = glm::translate(mat, glm::vec3(tmp.x, tmp.y, 0.1) );
    mat = glm::scale(mat, glm::vec3(aScale, aScale, aScale) );

    mat = cam.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]) );

    ColorF c = aColor.toColorF();
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a));
    //MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, 1.0, 0.0, 0.0, 1.0));


    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    _glTexMgr.bindDashTex( aDashArr );

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 2, GL_FLOAT, 
        GL_FALSE, sizeof(XyUv), (void *)aVertArr));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aUVLoc, 2, GL_FLOAT, 
        GL_FALSE, sizeof(XyUv), (void *)(aVertArr + 2)) );

    //MDGL_CHECK(md::glFrontFace(GL_CCW)); 
    MDGL_CHECK(md::glDrawArrays(GL_TRIANGLE_STRIP, 0, aVertCnt));//GL_TRIANGLE_STRIP
    //MDGL_CHECK(md::glFrontFace(GL_CCW));

    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc));

    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
#endif
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    MDGL_CHECK(md::glEnable(GL_BLEND));

}