#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <md/util/merc_prj.hpp>
#include <md/util/math_tools.hpp>
#include <md/util/logtool.hpp>
#include <md/util/color.hpp>

#include <md/util/tile_cover_bound.hpp>

#include <md/drawengine/transform_state.hpp>

#include "md/drawengine/glshaders/glshd_tex_uc.h"  
#include "md/drawengine/glshaders/glshd_uc.h"  
#include "md/drawengine/glshaders/glshaders.h"
#include "md/drawengine/rscmgr/gltexmgr.h"
#include "md/drawengine/drawcontext.h"

#include "md/drawengine/glpainters/glpaint_debug.h"

using namespace clientmap;//  using namespace in ".cpp" file

GLPaintDebug::GLPaintDebug(const GlShaders&  aGlShaders,
    const GlTexMgr&   aGlTexMgr)
    :_glShaders(aGlShaders), _glTexMgr(aGlTexMgr)
{

}
GLPaintDebug::~GLPaintDebug() = default;
 

void GLPaintDebug::drawTileBorder(const CPaintContext& aDC,
    const TileID&  aTLid,
    const ColorI& aColor)
{
    auto&   cam = const_cast<TransformState&>(aDC._transState);

#if 1     
    // Add four vertices to draw the outline of the tile in red
    double  mbw = MercPrj::tileBpxWidth(aTLid.z);
    float vertices[] = {
                0.f,     0.f, 
                1.f*mbw, 0.f,
                1.f*mbw, 1.f*mbw,
                0.f,     1.f*mbw,
    };
    int16_t idxArr[] = { 0, 1, 2, 3, 0 };
    int16_t idxCnt = 5;
 

    //MDGL_CHECK(md::glEnable(GL_BLEND) );
    //MDGL_CHECK(md::glDisable(GL_DEPTH_TEST) );
    //MDGL_CHECK(md::glDisable(GL_CULL_FACE) );
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getUColorShader();
    assert(shd.use());

    Pt2D tileOrigin= MercPrj::tileOriginInBpx(aTLid);
    ScreenPt scrPt = cam.screenFromBpx(Pt3D{ tileOrigin.x,tileOrigin.y,0 });// for debug

    auto tmp = cam.bpx2RelativeBpxX(tileOrigin);
#ifdef  USE_ViewCamera_AS_TransformState
    auto mat = cam.getVMat() * cam.getMMatS();
#else
    auto mat = cam.getVMat();
#endif
    mat = glm::translate(mat, glm::vec3(tmp.x, tmp.y, 0.0));

	//double scaleDiff = glm::pow(2, cam.getZoom() - aTLid.z);
	//mat = glm::scale(mat, glm::vec3(scaleDiff, scaleDiff, 1));

    mat = cam.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]) );

    ColorF c = aColor.toColorF();
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 2, GL_FLOAT,
        GL_FALSE, 0, vertices) );

    MDGL_CHECK(md::glFrontFace(GL_CCW));
    MDGL_CHECK(md::glDrawElements(GL_LINE_STRIP, idxCnt, GL_UNSIGNED_SHORT, 
        idxArr) );

    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc) );
#else
    // Add four vertices to draw the outline of the tile in red
    double  mbw = MercPrj::tilePxWidth(aTLid.z);
    float vertices[] = {
                0.f,     0.f,
                1.f*mbw, 0.f,
                1.f*mbw, 1.f*mbw,
                0.f,     1.f*mbw,
    };
    int16_t idxArr[] = { 0, 1, 2, 3, 0 };
    int16_t idxCnt = 5;


    //md::glEnable(GL_BLEND);
    //md::glDisable(GL_DEPTH_TEST);
    //md::glDisable(GL_CULL_FACE);
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getUColorShader();
    assert(shd.use());

    //Pt2D tileOrigin = MercPrj::tileOriginInBpx(aTLid);     
    //tileOrigin = cam.bpx2RelativeBpxX(tileOrigin);
    //glm::mat4  mmat = cam.getMMatS();
    //glm::dvec4 tileOri= mmat * glm::dvec4(tileOrigin.x, tileOrigin.y, 0, 1.0);
    //Log::Debug("tileOri(%lf, %lf)\n", tileOri.x, tileOri.y);
 
    auto mat = cam.getVMat(); 
    //mat = glm::translate(mat, glm::vec3(tileOri.x, tileOri.y, 0.0));


    mat = cam.getPMat() * mat;
    md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE, &mat[0][0]);

    ColorF c = aColor.toColorF();
    md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a);

    md::glEnableVertexAttribArray(shd.m_aPosLoc);
    md::glVertexAttribPointer(shd.m_aPosLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);

    md::glFrontFace(GL_CCW);
    md::glDrawElements(GL_LINE_STRIP, idxCnt, GL_UNSIGNED_SHORT, idxArr);

    md::glDisableVertexAttribArray(shd.m_aPosLoc);
#endif
    //MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    //MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    //MDGL_CHECK(md::glEnable(GL_BLEND));

}


void GLPaintDebug::drawTileBorder1(const CPaintContext& aDC,
    const TileID&  aTLid,
    const ColorI& aColor)
{
    auto& cam = aDC._transState;

    Box2D   box = MercPrj::tileBoundsInBpx(aTLid);
    cam.bpx2RelativeBpx(box);
    // Add 5 vertices to draw the outline of the tile in red
    float vertices[] = {
                box.mMinX, box.mMinY,
                box.mMaxX, box.mMinY,
                box.mMaxX, box.mMaxY,
                box.mMinX, box.mMaxY,
                box.mMinX, box.mMinY
    }; 
    int16_t ptCnt = 5;
     
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getUColorShader();
    assert(shd.use());
      
    md::glEnable(GL_BLEND);
#ifdef  USE_ViewCamera_AS_TransformState
    auto mat = cam.getVMat() * cam.getMMatS();
#else
    auto mat = cam.getVMat();
#endif
    mat = cam.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, GL_FALSE,
        &mat[0][0]) );

    ColorF c = aColor.toColorF();
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 2, GL_FLOAT,
        GL_FALSE, 0, vertices));

    MDGL_CHECK(md::glFrontFace(GL_CCW));
    MDGL_CHECK(md::glDrawArrays(GL_LINE_STRIP, 0, ptCnt));

    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc));

    //MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    //MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    //MDGL_CHECK(md::glEnable(GL_BLEND));

}
 
void GLPaintDebug::drawGroundBound(const CPaintContext& aPC,
    const ColorI& aColor)
{
    auto& cam = const_cast<TransformState&>(aPC._transState);
    auto bound = cam.getGroundBound();
    std::vector<Pt2Df> vertArr;
    vertArr.reserve(5);
#if 1
    auto tmp = cam.bpx2RelativeBpxX(bound.getLeftTop());
    vertArr.emplace_back(tmp.x, tmp.y);
    tmp = cam.bpx2RelativeBpxX(bound.getRightTop());
    vertArr.emplace_back(tmp.x, tmp.y);
    tmp = cam.bpx2RelativeBpxX(bound.getRightBottom());
    vertArr.emplace_back(tmp.x, tmp.y);
    tmp = cam.bpx2RelativeBpxX(bound.getLeftBottom());
    vertArr.emplace_back(tmp.x, tmp.y);

    vertArr.emplace_back(vertArr[0]);
#else
    Box2D box = bound.getBound();
    int tileCnt =getCoveredTileCountbyBpxBox(box, cam.getZoomi());

    cam.bpx2RelativeBpx(box);
    double shrinkLen = 2* MercPrj::bpxsPerPx(cam.getZoomi());
    box.deflate( shrinkLen, shrinkLen);
    bool bEmpty = box.isEmpty();
    auto w = box.width();
    auto h = box.height();
    vertArr.emplace_back(box.southWest().x, box.southWest().y);
    vertArr.emplace_back(box.southEast().x, box.southEast().y);
    vertArr.emplace_back(box.northEast().x, box.northEast().y);
    vertArr.emplace_back(box.northWest().x, box.northWest().y);
    vertArr.emplace_back(box.southWest().x, box.southWest().y);
#endif 
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getUColorShader();

    ColorF  c = aColor.toColorF();
    shd.drawLineStrip(cam, (float* )vertArr.data(),
        vertArr.size(), c );
}

void  GLPaintDebug::draw2dText(const CPaintContext& aPC,
    int aPosx,
    int aPosy,
    const ColorI& aColor,
    const std::string& aText )
{
    auto& cam = const_cast<TransformState&>(aPC._transState);
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getUColorShader();

    ColorF  c = aColor.toColorF();
    shd.draw2dText(cam, aPosx, aPosy, c, aText);
}