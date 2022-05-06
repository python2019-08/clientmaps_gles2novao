//#pragma once
#ifndef MD_DRAWENGINE_GLPAINTERS_GLPAINTERS_H_INCLUDED
#define MD_DRAWENGINE_GLPAINTERS_GLPAINTERS_H_INCLUDED
 
#include <string>
#include <vector>
#include <memory> 
#include <md/util/color.hpp> 
#include <md/util/geom.hpp> 

// define namespace in ".h" file, and using namespace in ".cpp" file...
namespace clientmap {// define namespace in ".h" file,
    
class TileID;
class ColorI;
class GlShaders;
class GlTexMgr;
class CPaintContext;
class GLPaintBackground;
class GLPaintDebug;
class GLPaintFace;
class GLPaintLine;
class GLPaintBuilding;

class GlPainters final {
public:
    GlPainters( );
    ~GlPainters();
     
    // bg
    void bgDrawClear(const CPaintContext& aDC, const ColorI& aBgColor);
    void bgDrawGrid(const CPaintContext& aDC,
        const ColorI& aGridColor = { 204,204,204,250 });
    void dbgDrawSolidLineInPx(const CPaintContext& aDC,
        const ColorI& aColor,
        double        aLineWidth,
        float* aVertArr,
        int32_t  aVertCnt
    );

    void dbgDrawThinSolidLineInPx(const CPaintContext& aDC,
        const ColorI& aColor,
        double        aLineWidth,
        float* aVertArr,
        int32_t  aVertCnt
    );


    //dbg
    void dbgDrawBound(const CPaintContext& aDC,
        const ColorI& aGridColor = { 204,204,204,250 });
    void dbgDrawTileBorder(const CPaintContext& aDC,
        const TileID&  aTLid,
        const ColorI& aColor);
    void dbgDraw2dText(const CPaintContext& aDC,
        int aPosX,
        int aPosY,
        const ColorI& aColor,
        const std::string& aText);


    //face
	void faceDrawPolygon(
		const CPaintContext& aDC,
		const ColorI& aColor,
		const Pt2D&   aOrigin,
		double   aScale,
		float*   aVertArr,
		int16_t* aIdxArr,
		int32_t  aIdxCnt
	);

    void drawPoints(
        const CPaintContext& aPC,
        const ColorI& aColor,
        const Pt2D& aOrigin,
        double   aScale,
        float* aVertArr,
        int32_t  aVertCnt
    );

	//line
	void drawSolidLine(
		const CPaintContext& aDC,
		const ColorI& aColor,
        double        aLineWidth,
		const Pt2D&   aOrigin,
		double   aScale,
		float*   aVertArr,
        int32_t  aVertCnt
	);
    void drawDashLine(
        const CPaintContext& aDC,
        const std::vector<float>& aDashArr,
        const ColorI& aColor,
        const Pt2D&   aOrigin,
        double   aScale,
        float*   aVertArr,
        int32_t  aVertCnt
    );

    // building
    void drawBuildingRoof(
        const CPaintContext& aDC,
        const ColorI& aColor,
        const Pt2D& aOrigin,
        double   aScale,
        float* aVertArr,
        int16_t* aIdxArr,
        int32_t  aIdxCnt
    );

    void drawBuildingSide(
        const CPaintContext& aDC,
        const ColorI& aColor,
        const Pt2D& aOrigin,
        double   aScale,
        float*   aVertArr,
        int32_t  aVertCnt
    );

    void prepareDrawDepthOfBuilding(const CPaintContext& aPC);



    void prepareDrawColorOfBuilding(const CPaintContext& aPC);
private:
    std::unique_ptr<GLPaintBackground > mGLCvsBg;
    std::unique_ptr<GLPaintDebug >      mGLCvsDbg;
	std::unique_ptr<GLPaintFace >       mGLCvsFace;
	std::unique_ptr<GLPaintLine >       mGLCvsLine;
    std::unique_ptr<GLPaintBuilding >   mGLCvsBd;
    //GLContext& context;
    //ProgramParameters programParameters;
    std::unique_ptr<GlShaders>   mShaders;
    std::unique_ptr<GlTexMgr>    mTexMgr;
};

} // namespace clientmap
#endif // !MD_DRAWENGINE_GLPAINTERS_GLPAINTERS_H_INCLUDED


 