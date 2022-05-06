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

#include "md/drawengine/glpainters/glpaint_background.h"

using namespace clientmap;//  using namespace in ".cpp" file

GLPaintBackground::GLPaintBackground(const GlShaders&  aGlShaders,
    const GlTexMgr&   aGlTexMgr)
    :_glShaders(aGlShaders), _glTexMgr(aGlTexMgr)
{

}
GLPaintBackground::~GLPaintBackground() = default;

void  GLPaintBackground::drawClear(const ColorI& aBgColor)
{
    // {167,217,252,255}
    ColorF c = aBgColor.toColorF();
    MDGL_CHECK( md::glClearColor(c.r, c.g, c.b, c.a) );
    MDGL_CHECK(md::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
}

void GLPaintBackground::drawGrid(const CPaintContext& aDC,
    const ColorI& aGridColor)
{
	auto& cam = aDC._transState;

    auto bound = _getGroundBound(aDC._transState);
	cam.bpx2RelativeBpx(bound);
    float vertices[] = {
        bound.mMinX, bound.mMinY,
        bound.mMaxX, bound.mMinY,
        bound.mMinX, bound.mMaxY,
        bound.mMaxX, bound.mMaxY
    };

    auto bpm = MercPrj::bpxsPerPx(aDC._transState.getZoom());
    float x = bound.width() / (64 * bpm);
    float y = bound.height() / (64 * bpm);
    float textCoords[] = {
        0, y,
        x, y,
        0, 0,
        x, 0
    };

    const_cast<GlTexMgr&>(_glTexMgr).bindGridTexture();


    auto& prog = (const_cast<GlShaders&>(_glShaders)).getTexUCShader();
    assert(prog.use() );

    auto tmat = glm::mat4();
#ifdef  USE_ViewCamera_AS_TransformState
    auto mat = cam.getPMat() * cam.getVMat() * cam.getMMatS();
#else
    auto mat = cam.getPMat() * cam.getVMat();
#endif
    MDGL_CHECK(md::glUniformMatrix4fv(prog.m_uMVPMatLoc , 1, GL_FALSE,
        &mat[0][0]) );
    //glUniformMatrix4fv(prog->loc(kLocIdx_TexMat), 1, GL_FALSE, &tmat[0][0]);

    ColorF  gc = aGridColor.toColorF();
    MDGL_CHECK(md::glUniform4f(prog.m_uColorLoc, 
        gc.r, gc.g, gc.b, gc.a) );

    MDGL_CHECK(md::glEnableVertexAttribArray(prog.m_aPosLoc) );
    MDGL_CHECK(md::glEnableVertexAttribArray(prog.m_aUVLoc) );
    MDGL_CHECK(md::glVertexAttribPointer(prog.m_aPosLoc, 2, GL_FLOAT,
        GL_FALSE, 0, vertices) );    
    MDGL_CHECK(md::glVertexAttribPointer(prog.m_aUVLoc, 2, GL_FLOAT, 
        GL_FALSE, 0, textCoords) );

    MDGL_CHECK(md::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4) );
    assert(md::glGetError() == GL_NO_ERROR);

    MDGL_CHECK(md::glDisableVertexAttribArray(prog.m_aPosLoc) );
    MDGL_CHECK(md::glDisableVertexAttribArray(prog.m_aUVLoc) );
}
 

//167, 217, 252
Box2D GLPaintBackground::_getGroundBound(const TransformState& aDrawCamera)
{
    auto bound = aDrawCamera.getGroundBound().getBound();

    auto tileLen = MercPrj::tileBpxWidth(aDrawCamera.getZoom());

    auto minx = (int)(bound.mMinX / tileLen) * tileLen;
    auto miny = (int)(bound.mMinY / tileLen) * tileLen;
    auto maxx = (int)(bound.mMaxX / tileLen) * tileLen + tileLen;
    auto maxy = (int)(bound.mMaxY / tileLen) * tileLen + tileLen;

    bound.set(minx, miny, maxx, maxy); 

    return bound;
}
 