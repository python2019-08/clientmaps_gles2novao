//#include "glcanvas/renderState.h"
#include <md/util/logtool.hpp>
#include "md/drawengine/glshaders/glshaders.h"
#include "md/drawengine/rscmgr/gltexmgr.h"
#include "md/drawengine/glpainters/glpaint_background.h"
#include "md/drawengine/glpainters/glpaint_debug.h"
#include "md/drawengine/glpainters/glpaint_face.h"
#include "md/drawengine/glpainters/glpaint_line.h"
#include "md/drawengine/glpainters/glpaint_building.h"

#include "md/drawengine/glpainters/glpainters.h"
 
using namespace clientmap;//  using namespace in ".cpp" file

GlPainters::GlPainters( )
{
    // create shaders
    mShaders = std::make_unique<GlShaders>();
    mTexMgr = std::make_unique<GlTexMgr>( );

    // create painters
    mGLCvsBg = std::make_unique<GLPaintBackground>( *mShaders, *mTexMgr );
    mGLCvsDbg = std::make_unique<GLPaintDebug>(*mShaders, *mTexMgr);
	mGLCvsFace = std::make_unique<GLPaintFace>(*mShaders, *mTexMgr);
	mGLCvsLine = std::make_unique<GLPaintLine>(*mShaders, *mTexMgr);
    mGLCvsBd = std::make_unique<GLPaintBuilding >(*mShaders, *mTexMgr);
}

GlPainters::~GlPainters()
{ 
     
}

void GlPainters::bgDrawClear(const CPaintContext& aDC, 
    const ColorI& aBgColor)
{
    mGLCvsBg->drawClear(aBgColor);
}

void GlPainters::dbgDrawBound(const CPaintContext& aDC,
    const ColorI& aGridColor)
{
    mGLCvsDbg->drawGroundBound(aDC, aGridColor);
}

void GlPainters::bgDrawGrid(const CPaintContext& aDC,
    const ColorI& aGridColor)
{
    mGLCvsBg->drawGrid(aDC, aGridColor);
} 

void  GlPainters::dbgDrawTileBorder(const CPaintContext& aDC,
    const TileID&  aTLid,
    const ColorI& aColor)
{
#if 1
    mGLCvsDbg->drawTileBorder(aDC, aTLid, aColor);
#else
    mGLCvsDbg->drawTileBorder1(aDC, aTLid, aColor);
#endif
}

void GlPainters::dbgDraw2dText(const CPaintContext& aDC,
    int aPosx,
    int aPosy,
    const ColorI& aColor,
    const std::string& aText)
{
    mGLCvsDbg->draw2dText(aDC, aPosx, aPosy, aColor, aText);
}

void GlPainters::dbgDrawSolidLineInPx(const CPaintContext& aDC,
    const ColorI& aColor,
    double        aLineWidth,
    float* aVertArr,
    int32_t  aVertCnt
)
{
    mGLCvsLine->drawSolidLineInPx(aDC, aColor, aLineWidth,
        aVertArr, aVertCnt);
}

void GlPainters::dbgDrawThinSolidLineInPx(const CPaintContext& aDC,
    const ColorI& aColor,
    double        aLineWidth,
    float* aVertArr,
    int32_t  aVertCnt
)
{
    mGLCvsLine->drawThinSolidLineInPx(aDC, aColor, aLineWidth,
        aVertArr, aVertCnt);
}

//-------------------------------
//-------------------------------
void GlPainters::faceDrawPolygon(
	const CPaintContext& aPC,
	const ColorI& aColor,
	const Pt2D&   aOrigin,
	double   aScale,
	float*   aVertArr,
	int16_t* aIdxArr,
	int32_t  aIdxCnt
)
{
    assert(aIdxCnt > 0);

	mGLCvsFace->drawPolygon( aPC, aColor, 
		aOrigin, aScale,  aVertArr, aIdxArr, aIdxCnt);
}

//-------------------------------
//-------------------------------
void GlPainters::drawPoints(
    const CPaintContext& aPC,
    const ColorI& aColor, 
    const Pt2D& aOrigin,
    double   aScale,
    float*   aVertArr,
    int32_t  aVertCnt
)
{

}
 

void GlPainters::drawSolidLine(
	const CPaintContext& aPC,
	const ColorI& aColor,
    double        aLineWidth,
	const Pt2D&   aOrigin,
	double   aScale,
	float*   aVertArr,
	int32_t  aVertCnt
)
{
    if (aLineWidth > 1.0) {
        mGLCvsLine->drawSolidLine(aPC, aColor, aLineWidth,
            aOrigin, aScale, aVertArr, aVertCnt);
    }
    else {
        mGLCvsLine->drawThinSolidLine(aPC, aColor, aLineWidth,
            aOrigin, aScale, aVertArr, aVertCnt);

        mGLCvsLine->drawPoints(aPC, aColor,
            aOrigin, aScale, aVertArr, aVertCnt);
    }

}

void GlPainters::drawDashLine(
    const CPaintContext& aPC,
    const std::vector<float>& aDashArr,
    const ColorI& aColor,
    const Pt2D&   aOrigin,
    double   aScale,
    float*   aVertArr,
    int32_t  aVertCnt
)
{
    mGLCvsLine->drawDashLine(aPC, aDashArr, aColor,
        aOrigin, aScale, aVertArr, aVertCnt);
}

//-------------------------------
//-------------------------------
void GlPainters::drawBuildingRoof(
    const CPaintContext& aPC,
    const ColorI& aColor,
    const Pt2D& aOrigin,
    double   aScale,
    float* aVertArr,
    int16_t* aIdxArr,
    int32_t  aIdxCnt
)
{
    mGLCvsBd->drawRoof(aPC, aColor,
        aOrigin, aScale, aVertArr, aIdxArr, aIdxCnt);
}

void GlPainters::drawBuildingSide(
    const CPaintContext& aPC,
    const ColorI& aColor,
    const Pt2D& aOrigin,
    double   aScale,
    float*   aVertArr,
    int32_t  aVertCnt)
{
    mGLCvsBd->drawSide(aPC, aColor,
        aOrigin, aScale, aVertArr, aVertCnt);
}

void GlPainters::prepareDrawDepthOfBuilding(const CPaintContext& aPC)
{
    mGLCvsBd->prepareDrawDepth(aPC );
}


void GlPainters::prepareDrawColorOfBuilding(const CPaintContext& aPC)
{
    mGLCvsBd->prepareDrawColor(aPC);
}