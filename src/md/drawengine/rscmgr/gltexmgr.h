 
#ifndef MD_DRAWENGINE_RSCMGRS_GLTEXTURE_MANAGER_H_INCLUDED
#define MD_DRAWENGINE_RSCMGRS_GLTEXTURE_MANAGER_H_INCLUDED

#include <list>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <stack>

#include "glm/glm.hpp"  
#include "platformbase/gl_interface.h"
 
#define CIRCLE_PARTITION_SIZE 360
#define CIRCLE_PONTS_SIZE ((CIRCLE_PARTITION_SIZE+1)*2 + 1)
 
class   SkBitmap;

enum GLClassIndex {
    RINDEX_RESERVE,
    RINDEX_VBO,
    RINDEX_TEXTURE,
    RINDEX_BUTT
};
 
namespace clientmap {
  
    typedef struct RItem {
    public:
        operator GLuint() {
            return _id;
        }
        
        void invalid() {
            _valid = false;
        }
        
    public:
        GLuint _id = 0;
        bool _valid = true;
    } *R;
    
    class SkBitmap;

    // Stable GlTextures, not changed during app runing 
    class GlTexMgr {
    public:
        GlTexMgr( );
        
        virtual ~GlTexMgr();
        
        void initContext();  
        
        void recycle();

        void bindTexture(const std::string &code, ::SkBitmap &bmp); 
        void bindDashTex(const std::vector<float>& aDashArr);
        void bindLAA_alphaTex();
		void bindLAA_rgbaTex();
        /* *
         *  bind高斯预过滤纹理。
         */
        void bindLGaussAATexture(); 
        
        void bindGridTexture();
        void bindBuildingRoofTex();
        void bindBuildingSideTex();
        void bindArrow();
        void bindDoubleYellow();
        
        bool bindCircleVBO();        
 
        /// blencFunc -- 防止重复置位gl状态  
        void blendFunc(int src, int dst); 
        
        
    private:
        bool _tryBindTexByCode(const std::string &code);
        bool _tryBindTexByDash(std::size_t  aHash); 

        R allocVBO();
        R allocTEXTURE();

        GLuint genTexture_notMipmap(
            int aImgWidth, int aImgHeight,
            GLvoid* aImgPixels,
            GLenum swrap, GLenum twrap,
            GLint  aFmt = GL_RGBA);
        GLuint genTexture_mipmap(
            int aImgWidth, int aImgHeight,
            GLvoid* aImgPixels,
            GLenum swrap,
            GLenum twrap,
            GLint aFmt = GL_RGBA);
        /* *
         *  获取一维高斯过滤权值。。
         *
         *  @aCount   一维高斯过滤器的总长度
         *  @aIdx     一维高斯过滤器某索引值。
         *  @return  小于0，错误；大于等于0，ok。
         *  @see参考类、方法等
         *  @deprecate
         */
        double getGaussFilterWeight(int aCount, int aIdx);


 
        std::list<RItem> _objList[RINDEX_BUTT];
        std::map<std::string, GLuint> _texTable; // 存储根据样式生成的纹理 -- dash/箭头，不响应样式变化
         
        std::map<size_t, GLuint>      _hash2dashTex;
        
        short _blendSrc = 0;
        short _blendDst = 0;
        
        R _circleDataId = nullptr;
        R _circleElmId = nullptr; 

        static  unsigned char s_lineAA_alphaTex32x32[ 32*32 ];
        static  unsigned char s_lineAA_alphaTex16x16[ 16*16 ];
    };
}// namespace clientmap

#endif //!MD_DRAWENGINE_RSCMGRS_GLTEXTURE_MANAGER_H_INCLUDED
