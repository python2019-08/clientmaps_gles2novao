#pragma once
#ifndef _MAPWORLD_DISPLAYCORE_TEXTPAINTER_H__
#define _MAPWORLD_DISPLAYCORE_TEXTPAINTER_H__

#include "display/painter/PainterBase.h"

namespace mapworld {


class MWTextSize {
public:
	MWTextSize() : w(0), h(0), descent(0)
	{}
public:
	int32 w;		// 文字宽度
	int32 h;		// 文字高度
	int32 descent;	// 文字下沉高度
};


class MVImage;
class TextPainter : public PainterBase
{
public:
	TextPainter(const IObj_Painter *_context);

public:
	// 字体
	void setFont(const MV_FONT &_font) {
		m_font = _font;
	}
	MV_FONT getFont() const {
		return m_font;
	}

public:
	/**
	 * 获取文字矩形范围
	 * @param _text			文字
	 * @param _descent
	 */
	MWTextSize getTextRect(const String& _text) const;

	/**
	 * 绘制文字
	 * @param _text			文字
	 * @param _x			基准点坐标
	 * @param _y			基准点坐标
	 * @param _font			字体
	 * @param _mat			绘制矩阵
	 */
	void drawText(const String &_text, float _x, float _y, const CMat4 &_mat, bool _erect = false) const;

	/**
	 * 绘制文字2D
	 * @param _x			基准点坐标
	 * @param _y			基准点坐标
	 * @param _text			文字
	 */
	void drawText2D(const String &_text, float _x, float _y, const CMat4 &_mat = CMat4::identity) const;

	/**
	 * 绘制文字3D
	 * @param _x			基准点坐标
	 * @param _y			基准点坐标
	 * @param _text			文字
	 * @param _angle		旋转角度 单位：角度
	 */
	void drawText3D(const String& _text, float _x, float _y, int16 _angle = 0, float _z = 0.0f) const;

	/* *
	 * 3D 直立的文本的绘制
	 * @param aText				文字
	 * @param aX				起始位置
	 * @param aY				起始位置
	 * @param aZ				起始位置
	 * @param aAngle			旋转角度 单位：角度
	 * @param align_mode		对齐模式
	 * @param aLineLength		如果是多行的单行最大长度 单位: base
	 * @param aLineTexture		底线前景材质
	 * @param aLineBackTexture	底线背景材质
	 * @param aPitch			倾斜角度 单位：角度
	 * @param aRoll				滚动角度 单位：角度
	 * @param aScale			缩放比例
	 */
	void drawTextErect(
		const String& aText,
		float aX, float aY, float aZ,
		int16 aAngle = 0,
		ALIGN_MODE align_mode = ALIGN_MIDDLE_BOTTOM,
		double aLineLength = 0.0f,
		uint32 aLineTexture = DC_TEXTURE_INVALID,
		uint32 aLineBackTexture = DC_TEXTURE_INVALID,
		int16 aPitch = 0,
		int16 aRoll = 0,
		double aScale = 1.0
	) const;

	/**
	 * 获取文字3D变换矩阵
	 * @param _x			基准点坐标
	 * @param _y			基准点坐标
	 * @param _text			文字
	 * @param _angle		旋转角度 单位：角度
	 * @return 变换矩阵
	 */
	CMat4 getText3DMat(const String& _text, float _x, float _y, int16 _angle = 0, float _z = 0.0f) const;
	
private:
	/**
	 * 获取文字矩形范围
	 * @param _text			文字
	 * @param _descent
	 */
	MWTextSize getTextRectInner(const String& _text) const;

	/**
	 * 将文字分割成行
	 * @param _text			[in ] 文字
	 * @param _font			[in ] 字体
	 * @param _maxLength	[in ] 一行最大长度 0xFFFFFFFF为无上限
	 * @param _lines		[out] 分割结果
	 * @return 文字尺寸
	 */
	MWTextSize breakTextIntoLine(const String &_text, double _maxLength, StringArray &_lines) const;

	// 绘制字体材质图片
	void drawFontImage(MVImage *image, float _x, float _y, float _z = 0) const;

	// 显示文字处理
	String textForShow(const String &_text) const;

	// 字符串颠倒
	String textReverse(const String &_text, int32 _startIdx, int32 _endIdx) const;

	// 阿拉伯语变换
	String textArabic(const String &_text, int32 _startIdx, int32 _endIdx) const;

protected:
	MV_FONT m_font;	// 字体
};


} // namespace mapworld

#endif // _MAPWORLD_DISPLAYCORE_TEXTPAINTER_H__
