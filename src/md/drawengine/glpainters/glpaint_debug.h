//#pragma once
#ifndef MD_DRAWENGINE_PAINTERS_GLPaintDebug_H_INCLUDED
#define MD_DRAWENGINE_PAINTERS_GLPaintDebug_H_INCLUDED
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
    class GLPaintDebug
    {
    public:
        GLPaintDebug(const GlShaders&  aGlShaders,
                     const GlTexMgr&   aGlTexMgr );
        ~GLPaintDebug();
         
        void drawTileBorder(const CPaintContext& aDC,
            const TileID&  aTLid,
            const ColorI& aColor);

        void  drawTileBorder1(const CPaintContext& aDC,
            const TileID&  aTLid,
            const ColorI& aColor);

        void drawGroundBound(const CPaintContext& aDC, 
                            const ColorI& aColor);

        void  draw2dText(const CPaintContext& aDC,
            int _posx,
            int _posy,
            const ColorI& aColor,
            const std::string& _text
        );
    private: 

        const GlShaders&       _glShaders;
        const GlTexMgr&        _glTexMgr;
    };
}// namespace clientmap {
#endif // !MD_DRAWENGINE_PAINTERS_GLPaintBackground_H_INCLUDED
