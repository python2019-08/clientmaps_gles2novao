//#pragma once
#ifndef MD_DRAWENGINE_PAINTERS_GLPaint_FACE_H_INCLUDED
#define MD_DRAWENGINE_PAINTERS_GLPaint_FACE_H_INCLUDED
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
    class GLPaintFace
    {
    public:
        GLPaintFace(const GlShaders&  aGlShaders,
                     const GlTexMgr&   aGlTexMgr );
        ~GLPaintFace();
         
        void  drawPolygon(const CPaintContext& aDC,
			const ColorI& aColor,
			const Pt2D&   aOrigin,
			double   aScale,
			float*   aVertArr,
			int16_t* aIdxArr,
			int32_t  aIdxCnt );
		  
    private: 

        const GlShaders&       _glShaders;
        const GlTexMgr&        _glTexMgr;
    };
}// namespace clientmap {
#endif // !MD_DRAWENGINE_PAINTERS_GLPaint_FACE_H_INCLUDED
