 
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

 
 
#include "glm/glm.hpp"
#include "glm/gtc/round.hpp"
#include "md/util/cpp_misc.hpp"
#include "md/util/logtool.hpp"
#include "md/drawengine/rscmgr/stb_image.h"
#include "md/md_global.h"
#include "md/drawengine/rscmgr/dash_array.h"

#include "md/drawengine/rscmgr/gltexmgr.h"

 
#define INTERNAL_CODE_LAA       "LAA_A"
#define INTERNAL_CODE_LAA_RGBA  "LAA_RGBA"
#define INTERNAL_CODE_LGAUSSAA  "LGAUSSAA"
#define INTERNAL_CODE_GRID      "tile_grid.png"
#define INTERNAL_CODE_BuildingRoof      "building_roof.png"
#define INTERNAL_CODE_BuildingSide      "building_side.png"
#define INTERNAL_CODE_ARROW     "ARROW"
 
namespace clientmap
{
 
    unsigned char GlTexMgr::s_lineAA_alphaTex32x32[32 * 32] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,6,75,150,198,234,249,249,234,198,150,75,6,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,6,120,234,255,255,255,255,255,255,255,255,255,255,234,120,6,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,81,228,255,255,255,255,255,255,255,255,255,255,255,255,255,255,228,81,0,0,0,0,0,0,0,
0,0,0,0,0,0,132,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,132,0,0,0,0,0,0,
0,0,0,0,0,168,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,168,0,0,0,0,0,
0,0,0,0,135,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,135,0,0,0,0,
0,0,0,84,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,84,0,0,0,
0,0,9,225,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,225,9,0,0,
0,0,117,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,117,0,0,
0,0,234,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,234,0,0,
0,63,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,63,0,
0,135,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,135,0,
0,201,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,201,0,
0,204,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,204,0,
0,250,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,250,0,
0,250,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,250,0,
0,204,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,204,0,
0,201,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,201,0,
0,135,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,135,0,
0,63,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,63,0,
0,0,234,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,234,0,0,
0,0,117,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,117,0,0,
0,0,9,228,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,228,9,0,0,
0,0,0,87,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,87,0,0,0,
0,0,0,0,135,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,135,0,0,0,0,
0,0,0,0,0,168,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,168,0,0,0,0,0,
0,0,0,0,0,0,135,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,135,0,0,0,0,0,0,
0,0,0,0,0,0,0,81,228,255,255,255,255,255,255,255,255,255,255,255,255,255,255,228,81,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,9,120,234,255,255,255,255,255,255,255,255,255,255,234,120,9,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,6,75,150,198,234,249,249,234,198,150,75,6,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };

 
uint8_t GlTexMgr::s_lineAA_alphaTex16x16[16 * 16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 49, 91, 91, 49, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 84, 221, 255, 255, 255, 255, 219, 83, 0, 0, 0, 0,
    0, 0, 0, 128, 255, 255, 255, 255, 255, 255, 255, 255, 128, 0, 0, 0,
    0, 0, 85, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 81, 0, 0,
    0, 1, 221, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 218, 1, 0,
    0, 52, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 49, 0,
    0, 90, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 88, 0,
    0, 89, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 88, 0,
    0, 51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 49, 0,
    0, 1, 220, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 217, 1, 0,
    0, 0, 82, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 78, 0, 0,
    0, 0, 0, 127, 255, 255, 255, 255, 255, 255, 255, 255, 126, 0, 0, 0,
    0, 0, 0, 0, 83, 217, 255, 255, 255, 255, 217, 79, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 48, 89, 89, 48, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
 
    
    GlTexMgr::GlTexMgr( )
    {
         
    }
    
    GlTexMgr::~GlTexMgr() 
    {
 
    }
    
    R GlTexMgr::allocVBO()
    {
        RItem item;
        MDGL_CHECK(md::glGenBuffers(1, &item._id));
        auto iter = _objList[RINDEX_VBO].insert(_objList[RINDEX_VBO].end(), item);
        return &*iter;
    }

    R GlTexMgr::allocTEXTURE()
    {
        RItem item;
        MDGL_CHECK(md::glGenTextures(1, &item._id));
        auto iter = _objList[RINDEX_TEXTURE].insert(
            _objList[RINDEX_TEXTURE].end(), item);
        return &*iter;
    }
  
    void GlTexMgr::recycle() 
    {
        do {
            std::vector<GLuint> buffer;
            for (auto iter = _objList[RINDEX_VBO].begin();
                iter != _objList[RINDEX_VBO].end();
                )
            {
                if (iter->_valid)
                {
                    ++iter; continue;
                }

                buffer.push_back(iter->_id);
                _objList[RINDEX_VBO].erase(iter++);
            }
            if (!buffer.empty()) {
                MDGL_CHECK(md::glDeleteBuffers(
                    (GLsizei)buffer.size(), buffer.data())
                    );
            }
        } while (0);

        do {
            std::vector<GLuint> buffer;
            for (auto iter = _objList[RINDEX_TEXTURE].begin();
                iter != _objList[RINDEX_TEXTURE].end();
                )
            {
                if (iter->_valid) {
                    ++iter; continue;
                }
                buffer.push_back(iter->_id);
                _objList[RINDEX_TEXTURE].erase(iter++);
            }
            if (!buffer.empty())
            {
                MDGL_CHECK(md::glDeleteTextures((GLsizei)buffer.size(), buffer.data()));
            }
        } while (0);
    }

    
    void GlTexMgr::bindLAA_alphaTex() 
    {
        if (_tryBindTexByCode(INTERNAL_CODE_LAA)) {
            return;
        } 
        GLsizei w = 16;
        GLsizei h = 16;   
		const unsigned char* alphaArr = s_lineAA_alphaTex16x16;  

        // 画细线时用mipmap也会导致锯齿,所以这里不用mipmap
#       if  1 

        GLenum fmt = GL_ALPHA; //GL_RGBA;
        GLuint texId = genTexture_notMipmap(w, h, (void*)alphaArr, 
                                            GL_REPEAT, GL_REPEAT, fmt); 
#       else
        GLuint texId = genTexture_mipmap(w, h, (void*)alphaArr, 
            GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_ALPHA);
#       endif 

		if (texId) {
			_texTable[INTERNAL_CODE_LAA] = texId;
		} 
    }
    
	void GlTexMgr::bindLAA_rgbaTex()
	{
		if (_tryBindTexByCode(INTERNAL_CODE_LAA_RGBA)) {
			return;
		}
#if 0		 
        ::SkBitmap bmp;
        bmp.setConfig(::SkBitmap::kARGB_8888_Config, 32, 32);
        bmp.allocPixels();
        bmp.eraseColor(0x00FFFFFF);

        ::SkPaint  paint;
        paint.setColor(0xFFFFFFFF);
        paint.setStrokeWidth(24);

        ::SkCanvas canvas(bmp);
        canvas.drawCircle(16, 16, 12, paint);

        auto tid = genTexture_mipmap(bmp.width(),bmp.height(),
            bmp.getPixels(),
            GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        if (tid) {
            _texTable[INTERNAL_CODE_LAA] = tid;
        }
#else  
        GLsizei w = 16;
        GLsizei h = 16; 
		const unsigned char* alphaArr = s_lineAA_alphaTex16x16;
        std::vector<unsigned char> img; //img(w * h * 4, 255);
        img.resize(w * h * 4);
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{ 
				img[4 * (j* w + i) + 0] = 255;
				img[4 * (j* w + i) + 1] = 255;
				img[4 * (j* w + i) + 2] = 255; 
				img[4 * (j* w + i) + 3] = alphaArr[j* w + i]; 
			} 
		}
		const unsigned char* texels = &img[0]; 
         
		// 画细线时用mipmap也会导致锯齿,所以这里不用mipmap  
        GLuint texId = genTexture_mipmap(w, h, (void*)texels,
            GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGBA);
		if (texId) {
			_texTable[INTERNAL_CODE_LAA_RGBA] = texId;
		}
#endif
	}

    
    double GlTexMgr::getGaussFilterWeight(int aCount,  int aIdx )
    {
        assert( aCount >= 0 && aIdx>=0 && aIdx<aCount);
        
        double  n = aCount;
        double  idxMin = 0.0;
        double  idxMax = (n - 1.0) * 0.5;
        double  idx    = aIdx;
        double xMin = idxMin - (n-1.0)*0.5;
        double xMax = idxMax - (n-1.0)*0.5;
        double x    = idx    - (n-1.0)*0.5;
        //double sigmaX = ((n-1.0)*0.5 - 1.0)*0.7 + 0.8;// code from open-cv
        double sigmaX = ((n-1.0)*0.5 - 1.0)*0.7 + 10.8;
        double scale2X = -0.5/(sigmaX * sigmaX);
        double tMin = exp(scale2X * xMin *xMin);
        double tMax = exp(scale2X * xMax *xMax);
        double t    = exp(scale2X * x    *x );
        double newA = (t -tMin) / (tMax - tMin);
        
        return newA*newA;// square the gaussian weight,make it converge quickly
    }
     
    void GlTexMgr::bindLGaussAATexture()
    {
        if (_tryBindTexByCode(INTERNAL_CODE_LGAUSSAA)) {
            return;
        }
 
        // 根据alpha纹理 构造GL_LUMINANCE_ALPHA纹理
        GLsizei w = 32;
        GLsizei h = 32;  
        const unsigned char* alphaArr = s_lineAA_alphaTex32x32;
        
        std::vector<unsigned char>  img;
#if 0
        img.resize( w * h * 4 );        
        for (int i = 0; i < w; i++)        
        {            
            for (int j = 0; j < h; j++)
            {
                //--对ALPHA做 Gauss过滤模糊
                int a0 = alphaArr[j * w + i];
                a0 = (int)(getGaussFilterWeight(h, i) * ((double)a0));

                //Luminance
                img[4 * (j * w + i) + 0] = 255;
                img[4 * (j * w + i) + 1] = 255;
                img[4 * (j * w + i) + 2] = 255;

                img[4 * (j * w + i) + 3] = a0;//Alpha 
                Log::Info("%d,",a0);
            }
            Log::Info("\n");
        }
        const unsigned char* texels = &img[0];
        // 画细线时用mipmap也会导致锯齿,所以这里不用mipmap 
        GLuint texId = genTexture_notMipmap(w, h, (void*)texels,
            GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGBA);
#else
        img.resize(w * h);
        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                //--对ALPHA做 Gauss过滤模糊
                int a0 = alphaArr[j * w + i];
                a0 = (int)(getGaussFilterWeight(h, i) * ((double)a0)); 

                img[j * w + i] = a0;//Alpha 
                Log::Info("%d,", a0);
            }
            Log::Info("\n");
        }
        const unsigned char* texels = &img[0];

        // 画细线时用mipmap也会导致锯齿,所以这里不用mipmap 
        GLuint texId = genTexture_notMipmap(w, h, (void*)texels,
            GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_ALPHA);
#endif

        if (texId) {
            _texTable[INTERNAL_CODE_LGAUSSAA] = texId;
        }
    }
        
    
    void GlTexMgr::bindGridTexture() 
    {
        if (_tryBindTexByCode(INTERNAL_CODE_GRID)) {
            return;
        }
    #if  0         
    #else 
        std::string imgPath = CMdGlobal::unique.m_assetPath 
                               +"/img/"+ INTERNAL_CODE_GRID;
        /* Load PNG Image pixel buffer. */
        int imgW=0, imgH =0;
        int imgStride = 0;
        uint8_t* imgBuf = stbi_load(imgPath.c_str(), &imgW, &imgH, &imgStride, 0);
        if (imgBuf == NULL)
        {
            return;
        }

        int exW = glm::ceilPowerOfTwo(imgW);
        int exH = glm::ceilPowerOfTwo(imgH);
        GLubyte *pixels = NULL;
        std::vector<GLubyte> arrUB;        
        if ( (imgW == exW) && (imgH == exH) )
        {
            pixels = imgBuf;
        }
        else 
        {
            arrUB.resize(4 * exW * exH);
            pixels = arrUB.data();
            for (int j = 0; j < exH; j++)
            {
                for (int i = 0; i < exW; i++)
                {
                    pixels[4 * (i + j * exW) + 0] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 0];
                    pixels[4 * (i + j * exW) + 1] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 1];
                    pixels[4 * (i + j * exW) + 2] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 2];
                    pixels[4 * (i + j * exW) + 3] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 3];
                }
            }
        } 
        auto tid = genTexture_mipmap( exW, exH, pixels, GL_REPEAT, GL_REPEAT);
        if (tid) {
            _texTable[INTERNAL_CODE_GRID] = tid;
        } 
        stbi_image_free(imgBuf);
    #endif
    }
    

    void GlTexMgr::bindBuildingRoofTex()
    {
        if (_tryBindTexByCode(INTERNAL_CODE_BuildingRoof)) {
            return;
        }
 
        std::string imgPath = CMdGlobal::unique.m_assetPath 
                        + "/img/" + INTERNAL_CODE_BuildingRoof;
        /* Load PNG Image pixel buffer. */
        int imgW = 0, imgH = 0;
        int imgStride = 0;
        uint8_t* imgBuf = stbi_load(imgPath.c_str(), &imgW, &imgH, &imgStride, 0);
        if (imgBuf == NULL)
        {
            assert(0);
            return;
        }

        int exW = glm::ceilPowerOfTwo(imgW);
        int exH = glm::ceilPowerOfTwo(imgH);
        GLubyte* pixels = NULL;
        std::vector<GLubyte> arrUB;
        if ((imgW == exW) && (imgH == exH))
        {
            pixels = imgBuf;
        }
        else
        {
            arrUB.resize(4 * exW * exH);
            pixels = arrUB.data();
            for (int j = 0; j < exH; j++)
            {
                for (int i = 0; i < exW; i++)
                {
                    pixels[4 * (i + j * exW) + 0] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 0];
                    pixels[4 * (i + j * exW) + 1] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 1];
                    pixels[4 * (i + j * exW) + 2] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 2];
                    pixels[4 * (i + j * exW) + 3] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 3];
                }
            }
        }
        auto tid = genTexture_mipmap(exW, exH, pixels, GL_REPEAT, GL_REPEAT);
        if (tid) {
            _texTable[INTERNAL_CODE_BuildingRoof] = tid;
        }
        stbi_image_free(imgBuf);
    }


    void GlTexMgr::bindBuildingSideTex()
    {
        if (_tryBindTexByCode(INTERNAL_CODE_BuildingSide)) {
            return;
        }

        std::string imgPath = CMdGlobal::unique.m_assetPath 
                        + "/img/" + INTERNAL_CODE_BuildingSide;
        /* Load PNG Image pixel buffer. */
        int imgW = 0, imgH = 0;
        int imgStride = 0;
        uint8_t* imgBuf = stbi_load(imgPath.c_str(), &imgW, &imgH, &imgStride, 0);
        if (imgBuf == NULL)
        {
            return;
        }

        int exW = glm::ceilPowerOfTwo(imgW);
        int exH = glm::ceilPowerOfTwo(imgH);
        GLubyte* pixels = NULL;
        std::vector<GLubyte> arrUB;
        if ((imgW == exW) && (imgH == exH))
        {
            pixels = imgBuf;
        }
        else
        {
            arrUB.resize(4 * exW * exH);
            pixels = arrUB.data();
            for (int j = 0; j < exH; j++)
            {
                for (int i = 0; i < exW; i++)
                {
                    pixels[4 * (i + j * exW) + 0] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 0];
                    pixels[4 * (i + j * exW) + 1] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 1];
                    pixels[4 * (i + j * exW) + 2] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 2];
                    pixels[4 * (i + j * exW) + 3] = (i >= imgW || j >= imgH) ? 0
                        : (int)imgBuf[4 * (i + j * imgW) + 3];
                }
            }
        }

        auto tid = genTexture_mipmap(exW, exH, pixels, GL_REPEAT, GL_REPEAT);
        if (tid) {
            _texTable[INTERNAL_CODE_BuildingSide] = tid;
        }
        stbi_image_free(imgBuf);
    }

    void GlTexMgr::bindArrow() {
        if (_tryBindTexByCode(INTERNAL_CODE_ARROW)) {
            return;
        }
    #if 0
        SkBitmap bmp;
        bmp.setConfig(SkBitmap::kARGB_8888_Config, 32, 32);
        bmp.allocPixels();
        bmp.eraseColor(0x00000000);
        
        SkPaint  paint;
        paint.setColor(0xFFFFFFFF);
        paint.setStrokeWidth(5);
        
        SkPath path;
        path.moveTo(16, 8);
        path.lineTo(24, 16);
        path.lineTo(16, 24);
        path.lineTo(32, 16);
        path.close();
        
        SkCanvas canvas(bmp);
        canvas.drawLine(0, 16, 24, 16, paint);
        canvas.drawPath(path, paint);

        auto tid = genTexture_mipmap(bmp, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        if (tid) {
            _texTable[INTERNAL_CODE_ARROW] = tid;
        }
    #endif
    }
     

    void GlTexMgr::bindDashTex(const std::vector<float>& aDashArr)
    {
        //----make a copy of aDashArr
        std::vector<float>  dashArr;
        dashArr.insert(dashArr.begin(), aDashArr.begin(), aDashArr.end());

        //----normalize dashArr
        DashArray::normalize(dashArr);

        if (dashArr.empty()) {
            assert(0);
            return;
        }

        const size_t dashHash = util_hash(DashArray::calcHash(dashArr));
        if (_tryBindTexByDash(dashHash)) {
            return;
        }
        // The OpenGL ES 2.0 spec, section 3.8.2 states:
        //
        //     Calling a sampler from a fragment shader will return 
        //     (R,G,B,A) = (0,0,0,1) if any of the following conditions
        //     are true:
        //     […]
        //     - A two-dimensional sampler is called, the corresponding 
        //       texture image is a non-power-of-two image […], and either 
        //       the texture wrap mode is not CLAMP_TO_EDGE, or the 
        //       minification filter is neither NEAREST nor LINEAR.
        //     […]
        //
        // This means that texture lookups won't work for NPOT textures unless
        // they use GL_CLAMP_TO_EDGE.We're using GL_CLAMP_TO_EDGE for the  
        // vertical direction, but GL_REPEAT for the horizontal direction,
        //  which means that we need a power-of-two texture for our line dash
        //  patterns to work on OpenGL ES 2.0 conforming implementations.
        //  Fortunately, this just means changing the height from 15 to 16,  
        //  and from 30 to 32, so we don't waste many pixels.    

        //---- dashImgRow
        std::vector<uint8_t> dashImgRow;
        int extWidth = DashArray::renderRow(dashArr, dashImgRow);

        //---- dashImg        
        std::vector<uint8_t> dashImg;
        int i  = 0; 
        for (i = 0; i < 32; i++)// 32 rows
        {
            float factor = 1.0f;
            if (i == 0 || i == 31) {// row0 or row31
                factor = 0.0f;
            }
            else if (i == 1 || i == 30)// row1 or row30
            {
                factor = 0.94f;
            }

            //dashImg.insert(dashImg.end(), dashImgRow.begin(), dashImgRow.end());
            for (int j = 0; j < (int)dashImgRow.size(); j++)// row0 ,all are 0;
            {
                float alpha = factor * ( (float)dashImgRow[j] );
                dashImg.push_back( alpha );
            }
        }        


        //---- generate texture
        GLuint tid = genTexture_mipmap(extWidth, 32, dashImg.data(),
            GL_REPEAT, GL_CLAMP_TO_EDGE, GL_ALPHA);
        if (tid) {
            _hash2dashTex[dashHash] = tid;
        }

        return;
    }
    
    void GlTexMgr::bindDoubleYellow() 
    {
    #if 0
        SkBitmap bmp;
        bmp.setConfig(SkBitmap::kARGB_8888_Config, 32, 32);
        bmp.allocPixels();
        bmp.eraseColor(0x00000000);
        
        SkPaint  paint;
        paint.setColor(0xFFFFFFFF);
        paint.setStrokeWidth(2);
        
        SkCanvas canvas(bmp);
        canvas.drawLine(0, 4, 32, 4, paint);
        canvas.drawLine(0, 28, 32, 28, paint);
    
        genTexture_mipmap(bmp, GL_REPEAT, GL_CLAMP_TO_EDGE);
    #endif
    }
    
    void GlTexMgr::bindTexture(const std::string &code, 
        ::SkBitmap &bmp) 
    {
        if (_tryBindTexByCode(code)) {
            return;
        }
    #if 0  
        auto w = bmp.width();
        auto h = bmp.height();
        auto ew = glm::ceilPowerOfTwo(w);
        auto eh = glm::ceilPowerOfTwo(h);
        
        ::SkBitmap temp;
        temp.setConfig(::SkBitmap::kARGB_8888_Config, ew, eh);
        temp.allocPixels();
        temp.eraseColor(0x00000000);
        SkCanvas canvas(temp);
        
        auto srcRect = SkIRect::MakeWH(w, h);
        auto dstRect = SkRect::MakeWH(ew, eh);
        canvas.drawBitmapRect(bmp, &srcRect, dstRect);
        auto tid = genTexture_mipmap(temp.width(), temp.height(),
            temp.getPixels(),GL_REPEAT, GL_REPEAT);
        if (tid) {
            _texTable[code] = tid;
        }
    #endif
        return;
    }
    

    bool GlTexMgr::bindCircleVBO() {
    #if 0
        if (_circleDataId) {
            glBindBuffer(GL_ARRAY_BUFFER, _circleDataId->_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _circleElmId->_id);
            return true;
        }
        
        _circleDataId = allocVBO();
        if (!_circleDataId) {
            assert(0);
            return false;
        }
        
        _circleElmId = allocVBO();
        if (!_circleElmId) {
            assert(0);
            _circleDataId->invalid();
            return false;
        }
  
        float x, y, rad;
        std::vector<Element6F> vertices;
        vertices.resize(CIRCLE_PONTS_SIZE);
        
        vertices[0] = Element6F(0, 0, 0, 0, 0, 0);
        
        for (auto i=0; i<=CIRCLE_PARTITION_SIZE; i++) {
            rad = glm::radians(i*360.0/CIRCLE_PARTITION_SIZE);
            x = glm::cos(rad);
            y = glm::sin(rad);
            
            vertices[2*i+1] = Element6F(x, y, x, y, 0.5, 0);
            vertices[2*i+2] = Element6F(x, y, -x, -y, 0.5, 1);
        }
        
        auto eoff = 0;
        std::vector<unsigned short> indices;
        indices.resize(CIRCLE_PONTS_SIZE * 2);
        for (auto i=0; i<=CIRCLE_PARTITION_SIZE; i++) {
            indices[eoff++] = 2*i+1;
            indices[eoff++] = 0;
        }
        
        for (auto i=0; i<=CIRCLE_PARTITION_SIZE; i++) {
            indices[eoff++] = 2*i+1;
            indices[eoff++] = 2*i+2;
        }
        indices.resize(eoff);
        
        glBindBuffer(GL_ARRAY_BUFFER, _circleDataId->_id);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertices[0]), 
			vertices.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _circleElmId->_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(indices[0]), 
			indices.data(), GL_STATIC_DRAW);
    #endif
        return true;
    }
     
    
    // matix
    //------------------
    void GlTexMgr::initContext() 
    {
    #if 0
        glGetError();

        auto &cc = _mapview->getUISetting().clearColor();
        glClearColor(cc.r(), cc.g(), cc.b(), cc.a());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _blendSrc = GL_SRC_ALPHA; _blendDst=GL_ONE_MINUS_SRC_ALPHA;
        
        glDisable(GL_DEPTH_TEST);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    #endif
         
    }

    void GlTexMgr::blendFunc(int src, int dst) 
    {
        if (_blendSrc==src && _blendDst==dst) {
            return;
        }
        
        _blendSrc = src;
        _blendDst = dst;
        MDGL_CHECK(md::glBlendFunc(src, dst));
        assert(md::glGetError() == GL_NO_ERROR);
    }
    

    // private
    //------------------    
    GLuint GlTexMgr::genTexture_mipmap(
        int aBmpWidth,int aBmpHeight, 
        GLvoid* aBmpPixels,
        GLenum swrap, 
        GLenum twrap,
        GLint  aFmt ) 
    {
        GLuint texId = 0;
        
        MDGL_CHECK(md::glGenTextures(1, &texId));
        if (texId == 0) {
            return 0;
        }
        MDGL_CHECK(md::glBindTexture(GL_TEXTURE_2D, texId));
        
        MDGL_CHECK(md::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 
            swrap));
        MDGL_CHECK(md::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 
            twrap));
        MDGL_CHECK(md::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
            GL_LINEAR_MIPMAP_LINEAR));
        MDGL_CHECK(md::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
            GL_LINEAR));
        
        MDGL_CHECK(md::glTexImage2D(GL_TEXTURE_2D, 0, aFmt, aBmpWidth, aBmpHeight,
            0, aFmt, GL_UNSIGNED_BYTE, aBmpPixels));
        
        MDGL_CHECK(md::glGenerateMipmap(GL_TEXTURE_2D) );
        
        return texId;
    }

    GLuint GlTexMgr::genTexture_notMipmap(
        int aImgWidth, int aImgHeight,
        GLvoid* aImgPixels,
        GLenum swrap, GLenum twrap,
        GLint  aFmt)
    {
        GLuint texId = 0;
        MDGL_CHECK(md::glGenTextures(1, &texId));
        if (texId == 0) {
            return 0;
        }
        // 画细线时用mipmap也会导致锯齿,所以这里不用mipmap
        MDGL_CHECK(md::glBindTexture(GL_TEXTURE_2D, texId));
        MDGL_CHECK(md::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, swrap));
        MDGL_CHECK(md::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, twrap));

        MDGL_CHECK(md::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        MDGL_CHECK(md::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
         
        MDGL_CHECK(md::glTexImage2D(GL_TEXTURE_2D, 0, aFmt, aImgWidth, aImgHeight,
            0, aFmt, GL_UNSIGNED_BYTE, aImgPixels));

        return texId;
    }
    
    bool GlTexMgr::_tryBindTexByCode(const std::string &code) 
    {
        if (_texTable.count(code) > 0) {
            MDGL_CHECK(md::glBindTexture(GL_TEXTURE_2D, _texTable[code]));
            return true;
        }
        
        return false;
    }
    
    bool GlTexMgr::_tryBindTexByDash(std::size_t  aHash)
    { 
        if (_hash2dashTex.count(aHash) > 0) {
            MDGL_CHECK(md::glBindTexture(GL_TEXTURE_2D, _hash2dashTex[aHash]));
            return true;
        }

        return false;
    }
}// clientmap
