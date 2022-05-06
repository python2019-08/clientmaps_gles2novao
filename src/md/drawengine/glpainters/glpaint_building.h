//#pragma once
#ifndef MD_DRAWENGINE_PAINTERS_GLPaint_BUILDING_H_INCLUDED
#define MD_DRAWENGINE_PAINTERS_GLPaint_BUILDING_H_INCLUDED
#include <glm/glm.hpp>
#include <md/util/geom.hpp>
#include <md/drawengine/transform_state.hpp>

namespace clientmap { // define namespace in ".h" file,
    //----
    template <typename T> struct SBox2D;
    using Box2D = SBox2D<double>;
    class TileID;
    class GlShaders;
    class GlTexMgr;
    class CPaintContext;

    //class TransformState;
    struct ColorI;
    //----
    class GLPaintBuilding
    {
    public:
        GLPaintBuilding(const GlShaders&  aGlShaders,
                     const GlTexMgr&   aGlTexMgr );
        ~GLPaintBuilding();
        //================================
        void  drawRoof(const CPaintContext& aDC,
            const ColorI& aColor,
            const Pt2D& aOrigin,
            double   aScale,
            float* aVertArr,
            int16_t* aIdxArr,
            int32_t  aIdxCnt);
        void  drawSide(const CPaintContext& aDC,
            const ColorI& aColor,
            const Pt2D& aOrigin,
            double   aScale,
            float*   aVertArr, 
            int32_t  aVertCnt);

        //===============================================================
        // prepare to draw depth  buffer
        void prepareDrawDepth(const CPaintContext& aDC);
        // prepare to draw color  buffer
        void prepareDrawColor(const CPaintContext& aPC);
        // finish drawing depth&color  buffer  
        void finishDraw();
        //===============================================================
        //===============================================================

        // draw depth buffer
        void  drawRoofDepth(const CPaintContext& aDC,
            const ColorI& aColor,
            const Pt2D& aOrigin,
            double   aScale,
            float* aVertArr,
            int16_t* aIdxArr,
            int16_t  aIdxCnt);

        void  drawSideDepth(const CPaintContext& aDC,
            const ColorI& aColor,
            const Pt2D& aOrigin,
            double   aScale,
            float* aVertArr,
            int16_t* aIdxArr,
            int16_t  aIdxCnt); 

        // draw color buffer
        void  drawRoofColor(const CPaintContext& aDC,
            const ColorI& aColor,
            const Pt2D& aOrigin,
            double   aScale,
            float* aVertArr,
            int16_t* aIdxArr,
            int16_t  aIdxCnt);
        void  drawSideColor(const CPaintContext& aDC,
            const ColorI& aColor,
            const Pt2D& aOrigin,
            double   aScale,
            float* aVertArr,
            int16_t* aIdxArr,
            int16_t  aIdxCnt); 
    private: 

        GlShaders&       _glShaders;
        GlTexMgr&        _glTexMgr;
    };
}// namespace clientmap {
#endif // !MD_DRAWENGINE_PAINTERS_GLPaint_BUILDING_H_INCLUDED
