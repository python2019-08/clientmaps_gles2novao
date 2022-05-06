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

#include "md/drawengine/glpainters/glpaint_face.h"

using namespace clientmap;//  using namespace in ".cpp" file

GLPaintFace::GLPaintFace(const GlShaders&  aGlShaders,
    const GlTexMgr&   aGlTexMgr)
    :_glShaders(aGlShaders), _glTexMgr(aGlTexMgr)
{

}
GLPaintFace::~GLPaintFace() = default;
 

void GLPaintFace::drawPolygon(const CPaintContext& aDC, 
    const ColorI& aColor,
	const Pt2D&   aOrigin,
	double   aScale,
	float*   aVertArr,
	int16_t* aIdxArr,
	int32_t  aIdxCnt
	)
{
    assert(aVertArr && aIdxArr && aIdxCnt > 0);
    auto&   cam = aDC._transState;  
     
    
    //MDGL_CHECK(md::glEnable(GL_BLEND) );
    //MDGL_CHECK(md::glDisable(GL_DEPTH_TEST) );
    //MDGL_CHECK(md::glDisable(GL_CULL_FACE) );
    auto& shd = (const_cast<GlShaders&>(_glShaders)).getUColorShader();
    assert(shd.use());
	 
    auto tmp = cam.bpx2RelativeBpxX(aOrigin);
#ifdef  USE_ViewCamera_AS_TransformState
    auto mat = cam.getVMat() * cam.getMMatS();
#else
    auto mat = cam.getVMat();
#endif
    mat = glm::translate(mat, glm::vec3(tmp.x, tmp.y, 0.01));
	mat = glm::scale(mat, glm::vec3(aScale, aScale, 1));

    mat = cam.getPMat() * mat;
    MDGL_CHECK(md::glUniformMatrix4fv(shd.m_uMVPMatLoc, 1, 
        GL_FALSE, &mat[0][0]) );

    ColorF c = aColor.toColorF();
    MDGL_CHECK(md::glUniform4f(shd.m_uColorLoc, c.r, c.g, c.b, c.a));

    MDGL_CHECK(md::glEnableVertexAttribArray(shd.m_aPosLoc));
    MDGL_CHECK(md::glVertexAttribPointer(shd.m_aPosLoc, 2, GL_FLOAT, 
        GL_FALSE, 0, aVertArr));

    MDGL_CHECK(md::glFrontFace(GL_CCW) );
    MDGL_CHECK(md::glDrawElements(GL_TRIANGLES, aIdxCnt, 
        GL_UNSIGNED_SHORT, aIdxArr));

    MDGL_CHECK(md::glDisableVertexAttribArray(shd.m_aPosLoc) );

    //MDGL_CHECK(md::glDisable(GL_DEPTH_TEST));
    //MDGL_CHECK(md::glDisable(GL_CULL_FACE));
    //MDGL_CHECK(md::glEnable(GL_BLEND));

}

  