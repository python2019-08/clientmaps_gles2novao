//#pragma once
#ifndef MD_DRAWENGINE_PAINTERS_GLPaintBackground_H_INCLUDED
#define MD_DRAWENGINE_PAINTERS_GLPaintBackground_H_INCLUDED
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
    class GLPaintBackground
    {
    public:
        GLPaintBackground(const GlShaders&  aGlShaders,
                          const GlTexMgr&   aGlTexMgr );
        ~GLPaintBackground();

        void drawClear(const ColorI& aBgColor);

        void drawGrid(const CPaintContext& aDC, 
            const ColorI& aGridColor);
          
    private:
        Box2D _getGroundBound(const TransformState& aDrawCamera);

        const GlShaders&       _glShaders;
        const GlTexMgr&        _glTexMgr;
    };
}// namespace clientmap {
#endif // !MD_DRAWENGINE_PAINTERS_GLPaintBackground_H_INCLUDED
