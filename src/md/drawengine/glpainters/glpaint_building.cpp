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

#include "md/drawengine/glpainters/glpaint_building.h"

using namespace clientmap;//  using namespace in ".cpp" file

GLPaintBuilding::GLPaintBuilding(const GlShaders&  aGlShaders,
    const GlTexMgr&   aGlTexMgr)
    : _glShaders(const_cast<GlShaders&>(aGlShaders))
    , _glTexMgr(const_cast<GlTexMgr&>(aGlTexMgr))
{

}
GLPaintBuilding::~GLPaintBuilding() = default;
 
void GLPaintBuilding::drawRoof(const CPaintContext& aDC,
    const ColorI& aColor,
	const Pt2D&   aOrigin,
	double   aScale,
	float*   aVertArr,
	int16_t* aIdxArr,
    int32_t  aIdxCnt
	)
{
    assert(aVertArr && aIdxArr && aIdxCnt > 0);
    auto& cam = const_cast<TransformState&>(aDC._transState);
  
    MDGL_CHECK(md::glEnable(GL_BLEND) );
    MDGL_CHECK(md::glEnable(GL_DEPTH_TEST) );
    MDGL_CHECK(md::glEnable(GL_CULL_FACE) );
    //MDGL_CHECK(md::glCullFace(GL_BACK));
    auto& shd = _glShaders.getTexUCShader();
    assert(shd.use());
	     
#ifdef  USE_ViewCamera_AS_TransformState
    glm::dvec3 tmp = cam.bpx2GroundPx(Pt3D{ aOrigin.x,aOrigin.y,0 });

    auto mat = cam.getVMat();
    mat = glm::translate(mat, glm::vec3(tmp.x, tmp.y, 0.2));
    mat = glm::scale(mat, glm::vec3(aScale, aScale, aScale));

    mat = cam.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]));

    ColorF c = aColor.toColorF();
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a));

    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    _glTexMgr.bindBuildingRoofTex();

    MDGL_CHECK(md::glBindBuffer(GL_ARRAY_BUFFER, 0));
    MDGL_CHECK(md::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 3, GL_FLOAT,
        GL_FALSE, sizeof(XyzUv), (void*)aVertArr));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aUVLoc, 2, GL_FLOAT,
        GL_FALSE, sizeof(XyzUv), (void*)(aVertArr + 3)));

    MDGL_CHECK(md::glFrontFace(GL_CCW) );
    MDGL_CHECK(md::glDrawElements(GL_TRIANGLES, aIdxCnt,
        GL_UNSIGNED_SHORT, aIdxArr));

    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc));
#endif
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    MDGL_CHECK(md::glEnable(GL_BLEND));
}


void GLPaintBuilding::drawSide(const CPaintContext& aDC,
    const ColorI& aColor,
    const Pt2D& aOrigin,
    double   aScale,
    float*   aVertArr, 
    int32_t  aVertCnt
)
{
    assert(aVertArr && aVertCnt > 0);
    auto& cam = const_cast<TransformState&>(aDC._transState);

    MDGL_CHECK(md::glEnable(GL_BLEND));
    MDGL_CHECK(md::glEnable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glEnable(GL_CULL_FACE));
    auto& shd = _glShaders.getTexUCShader();
    assert(shd.use());

#ifdef  USE_ViewCamera_AS_TransformState
    glm::dvec3 tmp = cam.bpx2GroundPx(Pt3D{ aOrigin.x,aOrigin.y,0 });

    auto mat = cam.getVMat();
    mat = glm::translate(mat, glm::vec3(tmp.x, tmp.y, 0.1));
    mat = glm::scale(mat, glm::vec3(aScale, aScale, aScale));

    mat = cam.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]));

    ColorF c = aColor.toColorF();
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a));

    MDGL_CHECK(md::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    _glTexMgr.bindBuildingSideTex();

    MDGL_CHECK(md::glBindBuffer(GL_ARRAY_BUFFER, 0));
    MDGL_CHECK(md::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 3, GL_FLOAT,
        GL_FALSE, sizeof(XyzUv), (void*)aVertArr));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aUVLoc, 2, GL_FLOAT,
        GL_FALSE, sizeof(XyzUv), (void*)(aVertArr + 3)));

    MDGL_CHECK(md::glFrontFace(GL_CW) );
    MDGL_CHECK(md::glDrawArrays(GL_TRIANGLE_STRIP, 0, aVertCnt));

    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aUVLoc));
    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc));
#endif
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    MDGL_CHECK(md::glEnable(GL_BLEND));
}
//======================================================================
//======================================================================
void GLPaintBuilding::prepareDrawDepth(const CPaintContext& aDC)
{
    MDGL_CHECK(md::glEnable(GL_DEPTH_TEST) );
    MDGL_CHECK(md::glDepthFunc(GL_LEQUAL) );

    MDGL_CHECK(md::glColorMask(0, 0, 0, 0) );
    MDGL_CHECK(md::glEnable(GL_POLYGON_OFFSET_FILL) );
    MDGL_CHECK(md::glPolygonOffset(1.0, 1.0) );
}


void GLPaintBuilding::prepareDrawColor(const CPaintContext& aPC)
{
    MDGL_CHECK(md::glDisable(GL_POLYGON_OFFSET_FILL) );

    MDGL_CHECK(md::glEnable(GL_CULL_FACE) );
    MDGL_CHECK(md::glFrontFace(GL_CCW) );
    MDGL_CHECK(md::glDepthMask(0) );
    MDGL_CHECK(md::glColorMask(1, 1, 1, 1) );
}
  
void GLPaintBuilding::finishDraw()
{
    MDGL_CHECK(md::glDepthMask(1) );
    MDGL_CHECK(md::glBindBuffer(GL_ARRAY_BUFFER, 0) );
    MDGL_CHECK(md::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );

    MDGL_CHECK(md::glDisable(GL_CULL_FACE) );
    MDGL_CHECK(md::glDisable(GL_DEPTH_TEST) );
}
//===============================================================
//===============================================================
void  GLPaintBuilding::drawRoofDepth(const CPaintContext& aDC,
    const ColorI& aColor,
    const Pt2D& aOrigin,
    double   aScale,
    float* aVertArr,
    int16_t* aIdxArr,
    int16_t  aIdxCnt)
{

}

void  GLPaintBuilding::drawSideDepth(const CPaintContext& aDC,
    const ColorI& aColor,
    const Pt2D& aOrigin,
    double   aScale,
    float* aVertArr,
    int16_t* aIdxArr,
    int16_t  aIdxCnt)
{

}

void  GLPaintBuilding::drawRoofColor(const CPaintContext& aDC,
    const ColorI& aColor,
    const Pt2D& aOrigin,
    double   aScale,
    float* aVertArr,
    int16_t* aIdxArr,
    int16_t  aIdxCnt)
{

}
void  GLPaintBuilding::drawSideColor(const CPaintContext& aDC,
    const ColorI& aColor,
    const Pt2D& aOrigin,
    double   aScale,
    float* aVertArr,
    int16_t* aIdxArr,
    int16_t  aIdxCnt)
{

}