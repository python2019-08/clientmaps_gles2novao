#pragma once
#ifndef CLIENT_MAP_MD_DRAWENGINE_FONTMGR_H
#define CLIENT_MAP_MD_DRAWENGINE_FONTMGR_H

#include <cstdint> 
#include <string> 
#include <map> 
#include <vector>

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftstroke.h"

#include "platformbase/gl_interface.h"
#include "md/util/geom.hpp"
#include "md/util/color.hpp"


namespace clientmap {

//#define USE_HASHMAP_TO_STORE_FONT 1
#define ATLAS_TEXTURE 1
#define GLES_ATLAS_MAX_TREES 20
#define GLES_ATLAS_TEX_SIZE 512 // 256


// 字体
class MV_FONT
{
public:
    MV_FONT() 
        : m_PointSize(12), 
        m_Style(0), 
        m_bStroke(false), 
        m_Color(0, 0, 0)
    {}
    MV_FONT(ColorF _clr) 
        : m_PointSize(12), m_Style(0), 
        m_bStroke(false), m_Color(_clr)
    {}
    MV_FONT(ColorF _clr, ColorF _strokeClr)
        : m_PointSize(12), m_Style(0), 
        m_bStroke(true), m_Color(_clr), 
        m_StrokeColor(_strokeClr)
    {}
    MV_FONT(uint16_t _size) 
        : m_PointSize(_size), 
        m_Style(0), m_bStroke(false), 
        m_Color(0, 0, 0)
    {}
    MV_FONT(uint16_t _size, ColorF _clr)
        : m_PointSize(_size), 
        m_Style(0), 
        m_bStroke(false), 
        m_Color(_clr)
    {}
    MV_FONT(uint16_t _size, ColorF _clr, ColorF _strokeClr)
        : m_PointSize(_size), 
        m_Style(0), 
        m_bStroke(true), 
        m_Color(_clr), 
        m_StrokeColor(_strokeClr)
    {}
public:
    uint16_t	m_PointSize;
    uint8_t		m_Style;
    bool		m_bStroke;
    ColorF	m_Color;
    ColorF	m_StrokeColor;
};

typedef struct  MVNamedFontFace{
    std::string  mName;
    FT_Face mFace;
    MVNamedFontFace()
    {}
    ~MVNamedFontFace()
    {}
}MVNamedFontFace;

 
class MVImage {
public:
	GLuint			texture;
	int				offsetX;
	int				offsetY;
	int				width;
	int				height;
	void			*buf; 		// temporary buffer to hold image data
	unsigned long	frameBuf;
	int				isValid;
	GLuint			uTexcVBO;
};
 

class FontImage {
public:
	MVImage	*image;
	MVImage	*outlineImage;
	float	advanceX;
	int		left;
	int		top;
	int		bottom;
	int		hash;
};

typedef std::map<int, FontImage*> FontHash;


/* 文字描述 */
typedef struct{
	wchar_t m_code;	 // 字符 unicode 编码
	int m_height;	 // 字符高度
	int m_width;	 // 字符宽度
	int m_drawX;	 // 字符绘制的水平偏移
	int m_drawY;	 // 字符绘制基线上方偏移
	int m_advanceX;	 // 字符步进
	unsigned char* m_buffer;	// 字符像素信息缓存
} Word;

typedef struct node_t{
	struct node_t* child[2];
    SBox2D<int32_t> rect;
	MVImage *image;
} AtlasNode;

typedef struct root_t {
	struct node_t* rootNode;
	char		   hint[256];
	GLuint	   texture;
} AtlasRoot;


class FontMgr {
public:
	FontMgr() 
        : m_ftLib(nullptr) 
    {
		init();
	}
public:
	int init();
	void done();

	void clear();

public: 
	void SetFontSize(int size, int bold);
	FontImage *GetFontTexture( wchar_t ch, int size, int bold );
	//FontImage *GetFontItem( wchar_t ch, int size, int bold );
     
	void InitFont( const std::string& str );
	void DoneFont( void );
	FT_Face GetFont( const MV_FONT& font );
	FT_Library GetFreeTypeLib( void )	{ return( m_ftLib ); };
	FT_Library				m_ftLib;

#ifdef USE_HASHMAP_TO_STORE_FONT
	std::map<std::string, FT_Face> m_mpFont;	/* 加载到内存中的truetype文件 */
#else
	std::vector<MVNamedFontFace>   m_arrFont;	/* 加载到内存中的truetype文件 */
#endif 
public:
	bool  checkFontTexture();

protected:
	FontImage *GetFontTexture( wchar_t ch, 
		int size, 
		int bold, 
		unsigned int color1, 
		unsigned int color2, 
		bool stroke 
		);
	FontImage *GetFontItem( wchar_t ch, 
		int size, 
		int bold, 
		unsigned int color1, 
		unsigned int color2, 
		bool stroke 
		);
	bool LoadText(FT_Face face,
		Word& word, 
		unsigned int color1, 
		unsigned int color2, 
		int border);
	bool LoadText(FT_Face face,Word& word);

	/* Texture atlas */
	AtlasNode *CreateAtlasNode(void);
	AtlasRoot CreateAtlasRoot(const char *hint, int minFilter, int magFilter);
	int GetRootIndex(const char *hint, int startIndex, int minFilter, int magFilter);
	AtlasNode *Insert(MVImage *image, AtlasNode *node);
	bool AtlasInsert(const char* hint, MVImage *image, int minFilter, int magFilter);
	void AtlasClean(const char* hint);
	void RecursiveCleanNode(node_t* rootNode);

private:
	/* Atlas Texture. */
	AtlasRoot s_atlasRoots[GLES_ATLAS_MAX_TREES];
	int       m_atlasIsFull; // for fix“地图随机缺字BUG”，在s_atlasRoots满时，清空它

private:	// font
	int					m_fontSize;
	FontHash			m_fontHash;
};

} // namespace clientmap

#endif // !CLIENT_MAP_MD_DRAWENGINE_FONTMGR_H
