//#pragma once
#ifndef MD_DRAWENGINE_PAINTERS_GLPaint_LINE_H_INCLUDED
#define MD_DRAWENGINE_PAINTERS_GLPaint_LINE_H_INCLUDED
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
    class GLPaintLine
    {
    public:
        GLPaintLine(const GlShaders&  aGlShaders,
                     const GlTexMgr&   aGlTexMgr );
        ~GLPaintLine();
         
        void drawPoints(const CPaintContext& aDC,
            const ColorI& aColor,
            const Pt2D& aOrigin,
            double   aScale,
            float*   aVertXyArr,
            int32_t  aVertXyCnt
        );

        void drawSolidLineInPx(const CPaintContext& aDC,
            const ColorI& aColor,
            double        aLineWidth,
            float*   aVertArr,
            int32_t  aVertCnt
        );

        void drawThinSolidLineInPx(const CPaintContext& aDC,
            const ColorI& aColor,
            double        aLineWidth,
            float* aVertXyArr,
            int32_t  aVertXyCnt
        );

        void  drawSolidLine(const CPaintContext& aDC,
			const ColorI& aColor,
            double        aLineWidth,
			const Pt2D&   aOriginInBpx,
			double   aScale,
			float*   aVertArr,
            int32_t  aVertCnt);

        void drawThinSolidLine(const CPaintContext& aDC,
            const ColorI& aColor,
            double        aLineWidth,
            const Pt2D& aOrigin,
            double   aScale,
            float* aVertArr,
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
    private: 

        GlShaders&       _glShaders;
        GlTexMgr&        _glTexMgr;
    };
}// namespace clientmap {
#endif // !MD_DRAWENGINE_PAINTERS_GLPaint_LINE_H_INCLUDED
