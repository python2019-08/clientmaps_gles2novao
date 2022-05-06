#include <algorithm>

#include "md/util/logtool.hpp"
#include "md/md_global.h" 

#include "fontmgr.h"

/* TODO: Extract Font Class. */
// gaojian modify 修改文字太大造成的BUG
//#define MAKE_HASH(_ch_,_size_,_bold_)(_ch_*100 + _size_ + _bold_)
#define MAKE_HASH(_ch_,_size_,_bold_)((_ch_ << 12) | (_size_ << 1) | _bold_)
#define ACTUAL_FONT_SIZE(size) ((size / 10 +1) * 10)

/* TODO: Extract Font Class. */
#define MAKE_KEYS(_ch_,_size_,_bold_,_color1_,_color2_) \
    (_ch_*100 + _size_ + _bold_ + _color1_ + _color2_)


namespace clientmap {

int FontMgr::init()
{
	m_fontSize = 0;

	for (int i = 0; i < GLES_ATLAS_MAX_TREES; i++) 
	{
		s_atlasRoots[i].rootNode = NULL;
		s_atlasRoots[i].hint[0] = 0;
		s_atlasRoots[i].texture = 0;
	}
	m_atlasIsFull = 0; // for fix 地图随机缺字BUG

	return 0;
}


void FontMgr::done()
{
	clear();
}


void FontMgr::clear()
{
	/* Clean hashmap & Release memory. */
	FontHash::iterator iter= m_fontHash.begin();
	for (;iter != m_fontHash.end();iter++)
	{
		FontImage *pFontImage = iter->second;
        if (!pFontImage)
        {
            continue;
        }

		if (pFontImage->image) 
		{
			if(pFontImage->image->buf != NULL)
			{
				free(pFontImage->image->buf);
				pFontImage->image->buf = NULL;
			}
			GLuint uTexcVBO = pFontImage->image->uTexcVBO;
			if ( uTexcVBO && glIsTexture(uTexcVBO) ) 
			{
				//MN_LOG( "[ReleaseFont]", "vboID: %d", uTexcVBO );
				md::glDeleteBuffers(1, &uTexcVBO);
			}
			free(pFontImage->image);
			pFontImage->image = NULL;
		}

		if (pFontImage->outlineImage)
		{
			if(pFontImage->outlineImage->buf != NULL)
			{
				free(pFontImage->outlineImage->buf);
				pFontImage->outlineImage->buf = NULL;
			}
			GLuint uTexcVBO = pFontImage->outlineImage->uTexcVBO;
			if ( uTexcVBO && glIsTexture(uTexcVBO) ) 
			{
				//MN_LOG( "[ReleaseFont]", "vboID: %d", uTexcVBO );
				md::glDeleteBuffers(1, &uTexcVBO);
			}
			free(pFontImage->outlineImage);
			pFontImage->outlineImage = NULL;
		}


		free(pFontImage);
		pFontImage = NULL;		 
	}
	m_fontHash.clear();
	AtlasClean("font");
}


bool  FontMgr::checkFontTexture()
{
     if (m_atlasIsFull)// for fix 地图随机缺字BUG
     {
         clear();

         m_atlasIsFull =0;

         return true;
     }

     return false;
}

void FontMgr::SetFontSize(int size, int bold)
{
	if (m_fontSize != size)
	{
		MV_FONT font;
		FT_Face face = GetFont( font );
		FT_Set_Pixel_Sizes( face, size, size );
		m_fontSize = size;
	}
}
namespace {

const char* s_fontFileNames[] = {
"NotoSans-Regular.ttf",
"NotoNaskh-Regular.ttf",
};

}// namespace {
/*------------------------- 初始化字体部分 -----------------------------------*/
void FontMgr::InitFont(const std::string& str )
{
#ifdef USE_HASHMAP_TO_STORE_FONT
    if ( !m_mpFont.empty() )
    {
        return;
    }
#else
    if ( !m_arrFont.empty() )
    {
        return;
    }
#endif    

    FT_Init_FreeType( &m_ftLib );
    
    int fontCnt = sizeof(s_fontFileNames) / sizeof(s_fontFileNames[0]);
    for (int  i = 0; i < fontCnt; i++)
    {
        const char* fontFN = s_fontFileNames[i];
        std::string  fontPath = fontFN; 
        fontPath = CMdGlobal::unique.m_fontDir + fontPath;
        FT_Face ft_face = NULL;
        if (FT_New_Face(m_ftLib, fontPath.c_str(), 0, &ft_face) != FT_Err_Ok)
        {
            continue;
        }
        Log::Debug("CPainter: load font:%s,%s(%d)",
            ft_face->family_name, ft_face->style_name,
            ft_face->style_flags
        );


    #ifdef USE_HASHMAP_TO_STORE_FONT            
        if (m_mpFont.count(fontFN) == 0)
        {
            m_mpFont.insert({ fontFN, ft_face } );
        }
    #else
		auto itFind= std::find_if(m_arrFont.begin(), m_arrFont.end(), 
			[&fontPath](const  MVNamedFontFace&  curFace)
		{ 
			return (curFace.mName == fontPath);
		}); 
        if (itFind == m_arrFont.end())
        {
            MVNamedFontFace  namedFace;
            namedFace.mName = fontPath;
            namedFace.mFace = ft_face;
            m_arrFont.push_back(namedFace);
        }
    #endif
    }    
}

void FontMgr::DoneFont( void )
{
    /* TODO: memory leaks */
#ifdef USE_HASHMAP_TO_STORE_FONT    
    std::map<std::string, FT_Face>::iterator it = m_mpFont.begin();
    for ( ; it != m_mpFont.end(); it++)
    { 
        FT_Face ft_face = it->second;
        FT_Done_Face( ft_face );
    }
    m_mpFont.clear();
#else   
    int32_t  fceCnt=m_arrFont.size();
    int32_t  iFce=0;
    for (iFce=0;iFce< fceCnt;iFce++)
    {
        MVNamedFontFace& curNameFce = m_arrFont.at(iFce);
        FT_Done_Face( curNameFce.mFace);
    } 
    m_arrFont.clear();
#endif    
    FT_Done_FreeType( m_ftLib );
	m_ftLib = nullptr;
}

FT_Face FontMgr::GetFont( const MV_FONT& font )
{
    /* 按ttf字体名称索引 */
    FT_Face ft_face = NULL;
#ifdef USE_HASHMAP_TO_STORE_FONT      
    if (!m_mpFont.empty())
    {
        std::map<std::string, FT_Face>::iterator it = m_mpFont.begin();
        ft_face = it->second;
    } else {
        Log::Debug( "FontMgr: [GetFont] ===> FT_Face not exit !" );
    }
#else
    if ( !m_arrFont.empty() ){
        ft_face = m_arrFont[0].mFace;
    }
    else{
        Log::Debug("FontMgr: [GetFont] ===> FT_Face not exit !");
    }
#endif

    return( ft_face );
}

/*------------------------- 渲染两次文字部分 -----------------------------------*/
// FontImage *FontMgr::GetFontItem( wchar_t ch, int size, int bold )
// {
// 	int hash = MAKE_HASH(ch, size, bold);
// 	if ( m_fontHash.contains(hash) )
// 	{
// 		return m_fontHash[hash];
// 	}
//     
// 	return NULL;
// }

/* 写入 png 文件 */  
void write_png(const char *file_name, 
    unsigned char *image, 
    int width, int height, 
    int bytes_per_pixel, 
    int bit_depth
    )  
{  
   //FILE *fp;  
   //png_structp png_ptr;  
   //png_infop info_ptr;  
   ////png_colorp palette;  
  
   ///* 打开需要写入的文件 */  
   //fp = fopen(file_name, "wb");  
   //if (fp == NULL)  
   //   return ;  
  
   ///* 创建并初始化 png_struct 及其所需的错误处理函数，如果你想使用默 
   // * 认的 stderr 和 longjump() 方法，你可以将最后三个参数设为 NULL， 
   // * 在使用动态链接库的情况下，我们也会检测函数库版本是否与在编译时 
   // * 使用的版本是否兼容。（必要） 
   // */  
   //png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);  
  
   //if (png_ptr == NULL)  
   //{  
   //   fclose(fp);  
   //   return ;  
   //}  
  
   ///* 分配内存并初始化图像信息数据。（必要）*/  
   //info_ptr = png_create_info_struct(png_ptr);  
   //if (info_ptr == NULL)  
   //{  
   //   fclose(fp);  
   //   png_destroy_write_struct(&png_ptr,  NULL);  
   //   return ;  
   //}  
  
   ///* 设置错误处理。如果你在调用 png_create_write_struct() 时没 
   // * 有设置错误处理函数，那么这段代码是必须写的。*/  
   //if (setjmp(png_jmpbuf(png_ptr)))  
   //{  
   //   /* 如果程序跑到这里了，那么写入文件时出现了问题 */  
   //   fclose(fp);  
   //   png_destroy_write_struct(&png_ptr, &info_ptr);  
   //   return ;  
   //}  
  
   ///* 设置输出控制，如果你使用的是 C 的标准 I/O 流 */  
   //png_init_io(png_ptr, fp);  
  
   ///* 这是一种复杂的做法 */  
  
   ///* （必需）在这里设置图像的信息，宽度、高度的上限是 2^31。 
   // * bit_depth 取值必需是 1、2、4、8 或者 16, 但是可用的值也依赖于 color_type。 
   // * color_type 可选值有： PNG_COLOR_TYPE_GRAY、PNG_COLOR_TYPE_GRAY_ALPHA、 
   // * PNG_COLOR_TYPE_PALETTE、PNG_COLOR_TYPE_RGB、PNG_COLOR_TYPE_RGB_ALPHA。 
   // * interlace 可以是 PNG_INTERLACE_NONE 或 PNG_INTERLACE_ADAM7, 
   // * 而 compression_type 和 filter_type 目前必需是 PNG_COMPRESSION_TYPE_BASE 
   // * 和 and PNG_FILTER_TYPE_BASE。 
   // */  
   //png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, PNG_COLOR_TYPE_RGB_ALPHA,  
   //   PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);  
  
   ///* 写入文件头部信息（必需） */  
   //png_write_info(png_ptr, info_ptr);  
  
   //png_uint_32 k;  
  
   ///* 在这个示例代码中，"image" 是一个一维的字节数组（每个元素占一个字节空间） */  
   ////png_byte *image = new png_byte[height*width*bytes_per_pixel];  
  
   //png_bytep *row_pointers = new png_bytep[height];  
  
   //if (height > PNG_UINT_32_MAX/(sizeof (png_bytep)))  
   //  png_error (png_ptr, "Image is too tall to process in memory");  
  
   ///* 将这些像素行指针指向你的 "image" 字节数组中对应的位置，即：指向每行像素的起始处 */  
   //for (k = 0; k < height; k++)  
   //  row_pointers[k] = image + k*width*bytes_per_pixel;  
  
   ///* 一次调用就将整个图像写进文件 */  
   //png_write_image(png_ptr, row_pointers);  
   ///* 必需调用这个函数完成写入文件其余部分 */  
   //png_write_end(png_ptr, info_ptr);  
   ///* 写完后清理并释放已分配的内存 */  
   //png_destroy_write_struct(&png_ptr, &info_ptr);  
   ///* 关闭文件 */  
   //fclose(fp);  
  
   //delete [] row_pointers;

   ///* That's it */  
   //return ;  
}  

FontImage* FontMgr::GetFontTexture( wchar_t ch, int size, int bold)
{
	/* get from hashmap*/
	FontImage* fontImage = NULL;
	//size *=  s_fPaintScale;
	fontImage = m_fontHash[MAKE_HASH(ch, size, bold)];//GetFontItem( ch, size, bold );
	if ( fontImage != NULL )
	{
		return fontImage;
	}

	MV_FONT font;
	FT_Face face = GetFont( font );	
	if ( face == NULL )/* Exception. */
	{
		return NULL;
	}

	SetFontSize(size, bold);

    /*------------------------- load glyph -----------------------------------*/
	int error;
	FT_Glyph glyph;
	error = FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT );
	if (error) {
		Log::Debug( "[GLES_Canvas]: FT_Load_Glyph() failed. Error: %d", error );
		return NULL;
	}

	error = FT_Get_Glyph( face->glyph, &glyph );
	if(error) {
        Log::Debug( "[GLES_Canvas]: FT_Get_Glyph() failed. Error: %d", error );
		return NULL;
	}

	error = FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	if(error) {
        Log::Debug( "[GLES_Canvas]: FT_Glyph_To_Bitmap() failed. Error: %d", error );
		return NULL;
	}

    fontImage = (FontImage*)malloc(sizeof(FontImage));
	//check_allocated (fontImage);
    
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
	fontImage->left = bitmap_glyph->left;
	fontImage->top  = bitmap_glyph->top;
	fontImage->bottom = fontImage->top  - bitmap_glyph->bitmap.rows;
	fontImage->advanceX=  bitmap_glyph->root.advance.x >> 16;

	int otherAdvance = fontImage->left + bitmap_glyph->bitmap.width;

	if(fontImage->advanceX < otherAdvance)
	    fontImage->advanceX = otherAdvance;

	FT_Bitmap bitmap=bitmap_glyph->bitmap;

	MVImage *image = NULL;
	int i, j;
	int width = bitmap.width;
	int height = bitmap.rows;
	if (width && height) 
	{
		GLubyte* temp_buf = (GLubyte*)malloc(4 * width * height);
		//check_allocated (temp_buf);
		for( j = 0; j < height; j++) 
		{
			for( i = 0; i < width; i++)
			{
				temp_buf[4*(i+j*width)] 
                    = temp_buf[4*(i+j*width) +1] 
                    = temp_buf[4*(i+j*width) + 2]
                    = 0;
				temp_buf[4*(i+j*width) + 3] = bitmap.buffer[i + bitmap.width*j];
			}
		}

		image = (MVImage*)malloc(sizeof(MVImage));
		//check_allocated (image);
		image->width = bitmap.width;
		image->height = bitmap.rows;
		image->buf = temp_buf;
		image->frameBuf = 0;
/*
		static int i = 0;
		String strTest("D:\\test_FONT\\test_");
		if(ch < 'a')
		{
			strTest << "big_" << ch << "_" << size << "_" << bold << ".png";
		}
		else
		{
			strTest << ch << "_" << size << "_" << bold << ".png";
		}
		
		write_png(strTest.toRawUTF8(), (unsigned char *)image->buf, image->width, image->height, 4, 8);
*/
		/* cache font in texture altas. */
#if ATLAS_TEXTURE
		if (!AtlasInsert ("font", image, GL_LINEAR, GL_LINEAR)) {
            Log::Debug( "[GLES_Canvas]: Could not cache font in texture atlas !" );
			fontImage->image = NULL;
			free (image->buf);
			image->buf = NULL;
			free (image);
		}
		else 
		{
			fontImage->image = image;
			free (image->buf);
			image->buf = NULL;
		}
#else
		/* TODO: free image buffer */
		fontImage->image = image;
#endif
	} else {
		fontImage->image = NULL;
	}

	FT_Done_Glyph(glyph);
    
    /*------------------------- load outline -----------------------------------*/
    FT_Stroker stroker;
    if (FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT )) {
        Log::Debug( "[GLES_Canvas]: FT_Load_Glyph() failed" );
        return NULL;
    }
    
    if (FT_Get_Glyph( face->glyph, &glyph )) {
        Log::Debug( "[GLES_Canvas]: FT_Get_Glyph() failed" );
        return NULL;
    }
    
    if (FT_Stroker_New( GetFreeTypeLib(), &stroker)){
        Log::Debug( "[GLES_Canvas]: FT_Stroker_New() failed" );
        return NULL;
    }
    
    FT_Stroker_Set(stroker,
                   (int)(/*ADJ_SCALE*/(1) * 64), // TODO:...
                   FT_STROKER_LINECAP_ROUND,
                   FT_STROKER_LINEJOIN_ROUND,
                   0);
    
    if (FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1) ){
        Log::Debug( "[GLES_Canvas]: FT_Glyph_StrokeBorder() failed" );
        return NULL;
    }
    FT_Stroker_Done(stroker);
    
    if( ( FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 ) ) ) {
        Log::Debug( "[GLES_Canvas]: FT_Glyph_To_Bitmap() failed. Error: %d" );
        return NULL;
    }
    
    bitmap_glyph = (FT_BitmapGlyph)glyph;
    bitmap = bitmap_glyph->bitmap;
    
    width = bitmap.width;
    height = bitmap.rows;
    if (width && height)
    {
        GLubyte* temp_buf = (GLubyte*)malloc(4 * width * height);
        //check_allocated (temp_buf);
        for( j = 0; j < height; j++)
        {
            for( i = 0; i < width; i++)
            {
                temp_buf[4*(i+j*width)] = 255;
                temp_buf[4*(i+j*width) + 1] = 255;
                temp_buf[4*(i+j*width) + 2] = 255;
                temp_buf[4*(i+j*width) + 3] = bitmap.buffer[i + bitmap.width*j];
            }
        }
        
        image = (MVImage*)malloc(sizeof(MVImage));
        //check_allocated (image);
        image->width = bitmap.width;
        image->height = bitmap.rows;
        image->buf = temp_buf;
        image->frameBuf = 0;
        
#if ATLAS_TEXTURE
        if (!AtlasInsert ("font", image, GL_LINEAR, GL_LINEAR)) {
            Log::Debug( "[GLES_Canvas]: Could not cache font in texture atlas !" );
            fontImage->outlineImage = NULL;
            free (image->buf);
            image->buf = NULL;
            free (image);
        }
        else
        {
            fontImage->outlineImage = image;
            free (image->buf);
            image->buf = NULL;
        }
        
#else
        fontImage->outlineImage = image;
#endif
    } else { //Not an error if the character is <space>
        fontImage->outlineImage = NULL;
    }
    
    FT_Done_Glyph(glyph);

	fontImage->hash = MAKE_HASH(ch, size, bold);
    m_fontHash.insert({ fontImage->hash, fontImage } );
	
	return fontImage;
}

/*------------------------- 渲染一次文字部分 -----------------------------------*/
FontImage *FontMgr::GetFontItem( wchar_t ch,
    int size, 
    int bold,
    unsigned int color1,
    unsigned int color2,
    bool stroke 
    )
{
    if (stroke)
    {
        int hash = MAKE_KEYS(ch, size, bold, color1, color2);
        if ( m_fontHash.count(hash)>0 )
        {
            return m_fontHash[hash];
        }
    }
    else
    {
        int hash = MAKE_HASH(ch, size, bold);
        if ( m_fontHash.count(hash) > 0)
        {
            return m_fontHash[hash];
        }
    }
    
	return NULL;
}

FontImage *FontMgr::GetFontTexture( wchar_t ch,
    int size, 
    int bold, 
    unsigned int color1,
    unsigned int color2,
    bool stroke
    )
{
	MV_FONT font;
	FT_Face face = GetFont( font );
	/* Exception. */
	if ( face == NULL )
	{
		return NULL;
	}
    
	/* get from hashmap*/
	FontImage *fontImage = NULL;
	//size *=  s_fPaintScale;
    fontImage = GetFontItem( ch, size, bold, color1, color2, stroke );
    if ( fontImage != NULL )
    {
        return fontImage;
    }
    
    SetFontSize(size, bold);
    
    if (stroke)
    {
        /*------------------------- load outline -----------------------------------*/
        Word word;
        word.m_code = ch;
        bool bOK = LoadText( face, word, color1, color2, bold );
        if (bOK)
        {
            fontImage = (FontImage*)malloc(sizeof(FontImage));
            fontImage->left = word.m_drawX;
            fontImage->top = word.m_drawY;
            fontImage->advanceX= word.m_advanceX;
            
            MVImage *image = NULL;
            image = (MVImage*)malloc(sizeof(MVImage));
            //check_allocated (image);
            image->width = word.m_width;
            image->height = word.m_height;
            image->buf = word.m_buffer;
            image->frameBuf = 0;
            
#if ATLAS_TEXTURE
            if (!AtlasInsert ("font", image, GL_LINEAR, GL_LINEAR)) {
                Log::Debug( "[GLES_Canvas]: Could not cache font in texture atlas !" );
                fontImage->outlineImage = NULL;
                free (image->buf);
                image->buf = NULL;
                free (image);
            }
            else
            {
                fontImage->outlineImage = image;
                free (image->buf);
                image->buf = NULL;
            }
            
#else
            fontImage->outlineImage = image;
#endif
        } else { //Not an error if the character is <space>
            fontImage->outlineImage = NULL;
        }
        
        fontImage->hash = MAKE_KEYS(ch, size, bold, color1, color2);
        m_fontHash.insert({ fontImage->hash, fontImage });
    }
    else
    {
        /*------------------------- load glyph ---------------------------------*/
        Word word;
        word.m_code = ch;
        bool bOK = LoadText( face, word );
        if (bOK)
        {
            fontImage = (FontImage*)malloc(sizeof(FontImage));
            fontImage->left = word.m_drawX;
            fontImage->top = word.m_drawY;
            fontImage->advanceX= word.m_advanceX;
            
            MVImage *image = NULL;
            image = (MVImage*)malloc(sizeof(MVImage));
            //check_allocated (image);
            image->width = word.m_width;
            image->height = word.m_height;
            image->buf = word.m_buffer;
            image->frameBuf = 0;
            
            /* cache font in texture altas. */
#if ATLAS_TEXTURE
            if (!AtlasInsert ("font", image, GL_LINEAR, GL_LINEAR)) {
                Log::Debug( "[GLES_Canvas]: Could not cache font in texture atlas !" );
                fontImage->image = NULL;
                free (image->buf);
                image->buf = NULL;
                free (image);
            }
            else
            {
                fontImage->image = image;
                free (image->buf);
                image->buf = NULL;
            }
#else
            /* TODO: free image buffer */
            fontImage->image = image;
#endif
        } else {
            fontImage->image = NULL;
        }
        
        fontImage->hash = MAKE_HASH(ch, size, bold);
        m_fontHash.insert({ fontImage->hash, fontImage } );
    }

	return fontImage;
}

/*------------------------- load glyph -----------------------------------*/
bool FontMgr::LoadText( FT_Face face, Word& word )
{
    FT_Glyph glyph;
    if ( FT_Load_Glyph( face, FT_Get_Char_Index( face, word.m_code ), FT_LOAD_DEFAULT ))
    {
        Log::Debug( "[GLES_Canvas]: FT_Load_Glyph() failed. " );
        return false;
    }
    
    if ( FT_Get_Glyph( face->glyph, &glyph )) 
    {
        Log::Debug( "[GLES_Canvas]: FT_Get_Glyph() failed. " );
        return false;
    }
    
    if ( FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 )) 
    {
        Log::Debug( "[GLES_Canvas]: FT_Glyph_To_Bitmap() failed. " );
        return false;
    }
    
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
    FT_Bitmap bitmap = bitmap_glyph->bitmap;
    
    int width = bitmap.width;
    int height = bitmap.rows;
    
    word.m_width = width;
    word.m_height= height;
    word.m_drawX = bitmap_glyph->left;
    word.m_drawY = bitmap_glyph->top;
    word.m_advanceX= face->glyph->advance.x >> 6;
    word.m_buffer = new unsigned char[word.m_width * word.m_height * 4];
    memset(word.m_buffer, 0, word.m_width * word.m_height * 4);
    
    unsigned char* buffer = word.m_buffer;
    if (width && height)
    {
        for( int j = 0; j < height; j++)
        {
            for( int i = 0; i < width; i++)
            {
                buffer[4*(i+j*width)] 
                    = buffer[4*(i+j*width) +1] 
                    = buffer[4*(i+j*width) + 2]= 0;
                buffer[4*(i+j*width) + 3] = bitmap.buffer[i + bitmap.width*j];
            }
        }
    }
    
    FT_Done_Glyph(glyph);
    
    return true;
}

/*------------------------- load outline ---------------------------------*/
bool FontMgr::LoadText( FT_Face face, 
    Word& word, 
    unsigned int color1,
    unsigned int color2,
    int border
    )
{
    //Font outline
	FT_Glyph glyph;
	FT_Stroker stroker;
	if (FT_Load_Glyph( face, FT_Get_Char_Index( face, word.m_code ), FT_LOAD_NO_BITMAP )) {
        Log::Debug( "[GLES_Canvas]: FT_Load_Glyph() failed" );
		return false;
	}
    
	if (FT_Get_Glyph( face->glyph, &glyph )) {
        Log::Debug( "[GLES_Canvas]: FT_Get_Glyph() failed" );
		return false;
	}
    
	if (FT_Stroker_New( GetFreeTypeLib(), &stroker)){
        Log::Debug( "[GLES_Canvas]: FT_Stroker_New() failed" );
		return false;
	}
    
	FT_Stroker_Set(stroker,
                   (int)(/*ADJ_SCALE*/(border) * 64), // TODO:...
                   FT_STROKER_LINECAP_ROUND,
                   FT_STROKER_LINEJOIN_ROUND,
                   0);
    
	if (FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1) ){
        Log::Debug( "[GLES_Canvas]: FT_Glyph_StrokeBorder() failed" );
		return false;
    }
    
    FT_Outline *outline = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
    
    FT_BBox bbox;
    FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);
    
    int width = (bbox.xMax - bbox.xMin) >> 6;
    int rows  = (bbox.yMax - bbox.yMin) >> 6;
    
    word.m_width    = width;
    word.m_height   = rows;
    word.m_drawX    = face->glyph->metrics.horiBearingX >> 6;
    word.m_drawY    = face->glyph->metrics.horiBearingY >> 6;
    word.m_advanceX = face->glyph->metrics.horiAdvance  >> 6;
    word.m_buffer = new unsigned char[word.m_width * word.m_height * 4];
    memset(word.m_buffer, 0, word.m_width * word.m_height * 4);
    
    unsigned char* buffer = word.m_buffer;
    
    FT_Raster_Params params;
    FT_Bitmap bmp;
    
    bmp.buffer = new unsigned char[width * rows];
    memset(bmp.buffer, 0, width * rows);
    bmp.width       = width;
    bmp.rows        = rows;
    bmp.pitch       = width;
    bmp.pixel_mode  = FT_PIXEL_MODE_GRAY;
    bmp.num_grays   = 256;
    
    memset(&params, 0, sizeof (params));
    params.source   = outline;
    params.target   = &bmp;
    params.flags    = FT_RASTER_FLAG_AA;
    FT_Outline_Translate(outline,-bbox.xMin,-bbox.yMin);
    FT_Outline_Render( GetFreeTypeLib(), outline, &params);
    unsigned char* buffer1 = bmp.buffer;
    
    FT_BBox bbox_in;
    FT_Glyph glyph_fg;
    FT_Get_Glyph(face->glyph, &glyph_fg);
    FT_Glyph_Get_CBox(glyph_fg, FT_GLYPH_BBOX_GRIDFIT,&bbox_in);
    
    bmp.buffer = new unsigned char[width * rows];
    memset(bmp.buffer, 0, width * rows);
    bmp.width       = width;
    bmp.rows        = rows;
    bmp.pitch       = width;
    bmp.pixel_mode  = FT_PIXEL_MODE_GRAY;
    bmp.num_grays   = 256;
    outline = &reinterpret_cast<FT_OutlineGlyph>(glyph_fg)->outline;
    memset(&params, 0, sizeof (params));
    params.source   = outline;
    params.target   = &bmp;
    params.flags = FT_RASTER_FLAG_AA;
    FT_Outline_Translate(outline,-bbox.xMin,-bbox.yMin);
    FT_Outline_Render(GetFreeTypeLib(), outline, &params);
    unsigned char* buffer2 = bmp.buffer;
    
    /* 原底色 */
    unsigned char sr = (color1 & 0xFF0000) >> 16,
    sg = (color1 & 0xFF00  ) >> 8,
    sb = (color1 & 0xFF    );
    
    /* 目标色 */
    unsigned char dr = (color2 & 0xFF0000) >> 16,
    dg = (color2 & 0xFF00  ) >> 8,
    db = (color2 & 0xFF    );
    
    int pitch = width;
    for (int yy = 0; yy < rows; ++yy)
    {
        for (int xx = 0; xx < width; ++xx)
        {
            int si = yy * word.m_width * 4 + xx * 4;
            int alpha1 = buffer1[yy * pitch + xx];
            int alpha2 = buffer2[yy * pitch + xx];

            if (alpha1)
            {
                buffer[si + 0] = sr;
                buffer[si + 1] = sg;
                buffer[si + 2] = sb;
                buffer[si + 3] = alpha1;
            }

            if (alpha2)
            {
                buffer[si + 0] = sr + ( dr - sr) * alpha2 / 255.0f;
                buffer[si + 1] = sg + ( dg - sg) * alpha2 / 255.0f;
                buffer[si + 2] = sb + ( db - sb) * alpha2 / 255.0f;
                buffer[si + 3] = std::min(255, alpha1 + alpha2);
            }
        }
    }
    
    delete [] buffer1;
    delete [] buffer2;
    
    FT_Stroker_Done(stroker);
    FT_Done_Glyph(glyph);
    
    return true;
}

AtlasNode *FontMgr::CreateAtlasNode(void)
{
	AtlasNode *pNode = (AtlasNode *)malloc(sizeof(AtlasNode)); // TODO: memory leaks.
	//check_allocated(pNode);

	pNode->image = NULL;
	pNode->child[0] = NULL;
	pNode->child[1] = NULL;   

	return pNode; 
}

AtlasRoot FontMgr::CreateAtlasRoot(const char *hint, 
    int minFilter, 
    int magFilter
    )
{
	AtlasRoot root;

    Log::Debug( "[GLES_Canvas]: creating new root for hint '%s'", hint );

	GLuint texture;
	glGenTextures( 1, &texture);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	GLubyte* tempBuf = (GLubyte*)malloc(4 * GLES_ATLAS_TEX_SIZE * GLES_ATLAS_TEX_SIZE);
	//check_allocated (tempBuf);
	for( int j = 0; j < GLES_ATLAS_TEX_SIZE; j++) 
	{
		for( int i = 0; i < GLES_ATLAS_TEX_SIZE; i++)
		{
			tempBuf[4 * (i+j*GLES_ATLAS_TEX_SIZE)] = 255;
			tempBuf[4 * (i+j*GLES_ATLAS_TEX_SIZE) + 1] = 255;
			tempBuf[4 * (i+j*GLES_ATLAS_TEX_SIZE) + 2] = 255;
			tempBuf[4 * (i+j*GLES_ATLAS_TEX_SIZE) + 3] = 0;
		}
	}
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 
        GLES_ATLAS_TEX_SIZE, GLES_ATLAS_TEX_SIZE, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, tempBuf );
	free(tempBuf);

	root.rootNode = CreateAtlasNode();
	root.rootNode->rect.minx = 0;
	root.rootNode->rect.miny = 0;
	root.rootNode->rect.maxx = GLES_ATLAS_TEX_SIZE - 1;
	root.rootNode->rect.maxy = GLES_ATLAS_TEX_SIZE - 1;
	root.texture = texture;
	strcpy(root.hint, hint); // TODO: safe

	return root;
}

int FontMgr::GetRootIndex(const char *hint, 
    int startIndex, 
    int minFilter, 
    int magFilter
    )
{
	for (int i = startIndex; i < GLES_ATLAS_MAX_TREES; i++) 
	{
		if (s_atlasRoots[i].rootNode == NULL) 
        {
			s_atlasRoots[i] = CreateAtlasRoot(hint, minFilter, magFilter);
            Log::Debug( "[GLES_Canvas]: CreateAtlasRoot index = %d hint = '%s'",
                i, s_atlasRoots[i].hint );
			return i;
		}

		if (strcmp(s_atlasRoots[i].hint, hint) == 0) {
			return i;
		}
	}

	return -1;
}

AtlasNode *FontMgr::Insert(MVImage *image, 
    AtlasNode *node
    )
{
	int nodeWidth = node->rect.maxx - node->rect.minx + 1;
	int nodeHeight = node->rect.maxy - node->rect.miny + 1;
	int width = (image)->width;
	int height = (image)->height;

	AtlasNode *newNode = NULL;
	if (node->child[0] != NULL && 
        node->child[1] != NULL
        ) 
	{
		newNode = Insert(image, node->child[0]);
		if (newNode != NULL) {
			return newNode;
		}

		return Insert(image, node->child[1]);
	}

	if (node->image != NULL) 
    {
		return NULL;
	}

	if (height > nodeHeight - 1 ||
        width > nodeWidth - 1) 
    {
		return NULL;
	}

	if (height == nodeHeight - 1 &&
        width == nodeWidth - 1) 
    {
		return node;
	}

	node->child[0] = CreateAtlasNode();
	node->child[1] = CreateAtlasNode();

	int dw = nodeWidth - width;
	int dh = nodeHeight - height;
	if (dw > dh) {
		node->child[0]->rect.minx = node->rect.minx;
		node->child[0]->rect.miny = node->rect.miny;
		node->child[0]->rect.maxx = node->rect.minx + width;
		node->child[0]->rect.maxy = node->rect.maxy;
		node->child[1]->rect.minx = node->rect.minx + width + 1;
		node->child[1]->rect.miny = node->rect.miny;
		node->child[1]->rect.maxx = node->rect.maxx;
		node->child[1]->rect.maxy = node->rect.maxy;
	} else {
		node->child[0]->rect.minx = node->rect.minx;
		node->child[0]->rect.miny = node->rect.miny;
		node->child[0]->rect.maxx = node->rect.maxx;
		node->child[0]->rect.maxy = node->rect.miny + height;
		node->child[1]->rect.minx = node->rect.minx;
		node->child[1]->rect.miny = node->rect.miny + height +1;
		node->child[1]->rect.maxx = node->rect.maxx;
		node->child[1]->rect.maxy = node->rect.maxy;
	}
	return Insert(image, node->child[0]);
}

bool FontMgr::AtlasInsert(const char* hint,
    MVImage *image,
    int minFilter,
    int magFilter
    )
{ 
	AtlasRoot *root = NULL;
	AtlasNode *node = NULL;

	int rootIndex = 0;
	while (node == NULL && rootIndex != -1) 
	{
		rootIndex = GetRootIndex(hint, rootIndex, minFilter, magFilter);
		if (rootIndex != -1) 
		{
			root = &s_atlasRoots[rootIndex];		 
			node = Insert(image, root->rootNode);
			rootIndex++;
		}
	}

	if ( node == NULL )
	{
        m_atlasIsFull = 1;// for fix 地图随机缺字BUG
		return false;
	}

	node->image = image;
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, root->texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 
        node->rect.minx, node->rect.miny, 
        image->width, image->height,
		GL_RGBA, GL_UNSIGNED_BYTE, image->buf);

	//int isFirstImage = ( ( node->rect.minx == 0 ) && ( node->rect.miny == 0 ) );
	image->offsetX = node->rect.minx;
	image->offsetY = node->rect.miny;
	image->texture = root->texture;
    
    /* texture vertices */
	GLfloat tv[8];
	GLfloat offsetX = image->offsetX / (GLfloat)GLES_ATLAS_TEX_SIZE;
	GLfloat offsetY = image->offsetY / (GLfloat)GLES_ATLAS_TEX_SIZE;
	GLfloat texWidth = image->width / (GLfloat)GLES_ATLAS_TEX_SIZE;
	GLfloat texHeight = image->height / (GLfloat)GLES_ATLAS_TEX_SIZE;
	tv[0] = offsetX + texWidth;
	tv[1] = offsetY;
	tv[2] = offsetX + texWidth;
	tv[3] = offsetY + texHeight;
	tv[4] = offsetX;
	tv[5] = offsetY;
	tv[6] = offsetX;
	tv[7] = offsetY + texHeight;
    // Generate And Bind The Vertex Buffer
    md::glGenBuffers( 1, &image->uTexcVBO );			 // Get A Valid Name
    md::glBindBuffer( GL_ARRAY_BUFFER, image->uTexcVBO ); // Bind The Buffer
    md::glBufferData( GL_ARRAY_BUFFER, sizeof(GLfloat)*8, tv, GL_STATIC_DRAW );
    md::glBindBuffer( GL_ARRAY_BUFFER, 0 );

	return true;
}

void FontMgr::RecursiveCleanNode(node_t* rootNode)
{
    if (rootNode) {
        if (rootNode->child[0])
        {
            RecursiveCleanNode(rootNode->child[0]);
        }
        if (rootNode->child[1]) 
        {
            RecursiveCleanNode(rootNode->child[1]);
        }
        free (rootNode);
        rootNode = NULL;
    }
}

void FontMgr::AtlasClean(const char* hint)
{
    Log::Debug( "[GLES_Canvas]: cleaning hint '%s'", hint );

	for (int i = 0; i < GLES_ATLAS_MAX_TREES; i++) 
	{
		if (s_atlasRoots[i].rootNode == NULL) {
			continue;
		}

		if (strcmp(s_atlasRoots[i].hint, hint) == 0) 
		{
            RecursiveCleanNode(s_atlasRoots[i].rootNode);

			s_atlasRoots[i].hint[0] = 0;

            GLuint texID = s_atlasRoots[i].texture;
            if ( texID && glIsTexture(texID) )
            {
                //MN_LOG( "[AtlasClean]", "texID: %d", texID );
				glDeleteTextures( 1, &texID );
                s_atlasRoots[i].texture = 0;
            }
			s_atlasRoots[i].rootNode = NULL;
		}
	}

    m_atlasIsFull =0;// for fix 地图随机缺字BUG
}


} // namespace mapworld