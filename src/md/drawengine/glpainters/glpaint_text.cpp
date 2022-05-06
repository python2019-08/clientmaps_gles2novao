#include "display/painter/TextPainter.h"
#include "../shader/ShaderTexturePure.h"
#include "PainterGLES.h"
#include "FontMgr.h"

#include <set>


namespace mapworld {

TextPainter::TextPainter(const IObj_Painter *_context) : PainterBase(_context)
{
	setShader(eShaderTexturePure);
}
	
// 获取文字矩形范围
MWTextSize TextPainter::getTextRect(const String& _text) const
{
	if (m_context == nullptr) {
		return {};
	}
	// 文字显示预处理
	String text = textForShow(_text);
	return getTextRectInner(text);
}

// 获取文字矩形范围
MWTextSize TextPainter::getTextRectInner(const String& _text) const
{
	if (m_context == nullptr) {
		return {};
	}

	const ScopedLock locker(m_context->m_gles->m_csFont);
	FT_Face face = m_context->m_gles->m_fontMgr.GetFont(m_font);
	if (face == nullptr) {
		return {};
	}
	int size = roundDoubleToInt(m_context->m_gles->m_pCamera->point2pixel(m_font.m_PointSize));

	MWTextSize sz;
	int count = _text.length();
	for (int i = 0; i < count; i++)
	{
		juce_wchar charCode = _text[i];
		if (charCode == 0x20) {
			sz.w += size / 2;
			continue;
		}
		FontImage *textImage = m_context->m_gles->m_fontMgr.GetFontTexture(charCode, size, 1);
		if (textImage == nullptr || textImage->image == nullptr || textImage->outlineImage == nullptr) {
			continue;
		}
		sz.w += textImage->advanceX;
		if (textImage->image->height > sz.h) {
			sz.h = textImage->image->height;
		}
	}

	//int ascent = (int)(m_face->ascender * size / m_face->height);
	sz.descent = std::abs((int)(face->descender * size / face->height));

	return sz;
}

// 绘制文字
void TextPainter::drawText(const String &_text, float _x, float _y, const CMat4 &_mat, bool _erect/* = false*/) const
{
	if (m_context == nullptr) {
		return;
	}
	if (m_settings.usingShader != eShaderTexturePure) {
		return;
	}

	ShaderTexturePure &shader = ShaderTexturePure::Instance();

	const ScopedLock locker(m_context->m_gles->m_csFont);
	FT_Face face = m_context->m_gles->m_fontMgr.GetFont(m_font);
	if (face == nullptr) {
		return;
	}
	int32 fontSize = roundDoubleToInt(m_context->m_gles->m_pCamera->point2pixel(m_font.m_PointSize));
	double erectOffset = m_context->m_gles->m_pCamera->point2pixel(2.0);

	glEnable(GL_BLEND);

	glUniformMatrix4fv(shader.slotUMat, 1, GL_FALSE, _mat);
	glEnableVertexAttribArray(shader.slotAVertCoords);
	glEnableVertexAttribArray(shader.slotATexCoords);

	int count = _text.length();

	float x = _x, y = _y;
	if (m_font.m_bStroke)
	{
		glUniform4fv(shader.slotUColor, 1, m_font.m_StrokeColor);
		for (int i = 0; i < count; i++)
		{
			juce_wchar charCode = _text[i];

			// If the character is <space>
			if (charCode == 32 || charCode == 10)
			{
				x += fontSize / 2;
				continue;
			}

			FontImage *textImage = m_context->m_gles->m_fontMgr.GetFontTexture(charCode, fontSize, 1);
			if (textImage == nullptr || textImage->image == nullptr || textImage->outlineImage == nullptr) {
				MN_LOG_DEBUG("[GLES_CANVAS]", "charCode=%d", charCode);
				continue;
			}

			if (_erect) {
				drawFontImage(textImage->image, x + textImage->left, y - textImage->top, erectOffset);
			}
			else {
				int offsetX = (textImage->outlineImage->width - textImage->image->width) / 2;
				int offsetY = (textImage->outlineImage->height - textImage->image->height) / 2;
				drawFontImage(textImage->outlineImage, x + textImage->left - offsetX, y - textImage->top - offsetY);
			}

			x += textImage->advanceX;
		}
	}

	x = _x; y = _y;
	glUniform4fv(shader.slotUColor, 1, m_font.m_Color);
	for (int i = 0; i < count; i++)
	{
		juce_wchar charCode = _text[i];

		// If the character is <space>
		if (charCode == 32 || charCode == 10)
		{
			x += fontSize / 2;
			continue;
		}

		FontImage *textImage = m_context->m_gles->m_fontMgr.GetFontTexture(charCode, fontSize, 1);
		if (textImage == nullptr || textImage->image == nullptr || textImage->outlineImage == nullptr) {
			MN_LOG_DEBUG("[GLES_CANVAS]", "charCode=%d", charCode);
			continue;
		}

		drawFontImage(textImage->image, x + textImage->left, y - textImage->top);

		x += textImage->advanceX;
	}

	glDisableVertexAttribArray(shader.slotAVertCoords);
	glDisableVertexAttribArray(shader.slotATexCoords);

	ShaderBase::Instance().use();
}

// 绘制文字2D
void TextPainter::drawText2D(const String &_text, float _x, float _y, const CMat4 &_mat/* = CMat4::identity*/) const
{
	if (m_context == nullptr) {
		return;
	}

	CMat4 mat = m_context->m_gles->m_mvpMat2D;
	if (_mat != (const CMat4)CMat4::identity) {
		mat = mat * _mat;
	}

	String text = textForShow(_text);
	bool _mulitline = false;
	if (_mulitline)
	{
		StringArray sa;
		uint32 _maxLength = m_font.m_PointSize * 5;
		uint32 maxLength = roundDoubleToInt(m_context->m_gles->m_pCamera->point2pixel(_maxLength));
		breakTextIntoLine(text, /*0xFFFFFFFF*/maxLength, sa);
		int32 lineHeight = roundDoubleToInt(m_context->m_gles->m_pCamera->point2pixel(m_font.m_PointSize));
		for (int32 i = 0; i < sa.size(); i++) {
			drawText(sa.getReference(i), _x, _y + lineHeight * i, mat);
		}
	}
	else
	{
		drawText(text, _x, _y, mat);
	}
}

// 绘制文字3D
void TextPainter::drawText3D(const String& _text, float _x, float _y, int16 _angle/* = 0*/, float _z/* = 0.0f*/) const
{
	if (m_context == nullptr) {
		return;
	}

	// 文字显示预处理
	String text = textForShow(_text);

	double scale = m_context->m_gles->m_pCamera->pixel2point(1.0);
	CMat4 mat = m_context->m_gles->m_mvpMat;
	mat *= DisplayUtils::createModelMatrix(_x, _y, _z, -_angle, 270.0, 0.0, scale);

	MWTextSize sz = getTextRectInner(text);
	drawText(text, -sz.w / 2, 0, mat);
}

// 绘制文字立面
void TextPainter::drawTextErect(
	const String& _text,
	float vertX, float vertY, float vertZ,
	int16 _azimuth /*= 0*/,
	ALIGN_MODE align_mode/* = ALIGN_MODE::ALIGN_MIDDLE_BOTTOM*/,
	double lineLength/*  = 0*/,
	uint32 _lineTexture/*  = DC_TEXTURE_INVALID*/,
	uint32 _lineBackTexture/*  = DC_TEXTURE_INVALID*/,
	int16 _pitch /*= 0*/,
	int16 _roll /*= 0*/,
	double _scale /*= 1.0*/
) const
{
	if (m_context == nullptr) {
		return;
	}

	// 文字显示预处理
	String text = textForShow(_text);

	// 缩放
	double scale = _scale * m_context->m_gles->m_pCamera->getScale() / m_context->m_gles->m_pCamera->GetScrnScale();

	// 字体
	MV_FONT font = m_font;
	font.m_Color.a *= 1.6;
	font.m_StrokeColor.a *= 1.6;
	float fontSize = m_context->m_gles->m_pCamera->point2pixel(font.m_PointSize);

	// 矩阵
	double pitch = _pitch + 180.0;
	CMat4 mat = m_context->m_gles->m_mvpMat;
	mat *= DisplayUtils::createModelMatrix(vertX, vertY, vertZ, _azimuth, pitch, _roll, scale);

	const ScopedLock locker(m_context->m_gles->m_csFont);
	FT_Face face = m_context->m_gles->m_fontMgr.GetFont(font);
	if (face == NULL) {
		return;
	}

	/* Set Param */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);              // 深度缓存为 只读
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	/**
	 * 文字渲染
	 */
	MWTextSize sz;
	if (align_mode == ALIGN_MIDDLE_BOTTOM_MULTLINES)
	{
		double maxLength = m_context->m_gles->m_pCamera->point2pixel(lineLength);
		StringArray saLines;
		sz = breakTextIntoLine(text, maxLength, saLines);
		uint32 lineCnt = saLines.size();
		for (int32 i = 0; i < lineCnt; i++)
		{
			String &t = saLines.getReference(i);
			MWTextSize szLine = getTextRectInner(t);
			drawText(t, -szLine.w / 2.0f, -fontSize * (lineCnt - i - 1) - fontSize / 4, mat, true);
		}
	}
	else
	{
		sz = getTextRectInner(text);
		drawText(text, -sz.w / 2.0f, -fontSize / 4, mat, true);
	}

	/**
	 * 装饰线
	 */
	if (_lineTexture != DC_TEXTURE_INVALID)
	{
		float rcMinX = -sz.w / 2;
		float rcMinY = 0;
		float rcMaxX = sz.w / 2;
		float rcMaxY = sz.w * 40.0 / 500.0;

		// 纹理坐标
		GLfloat t[2 * 4] = {
			0.1, 0.9, 0.1, 0.1,
			0.9, 0.9, 0.9, 0.1,
		};

		// 顶点坐标
		GLfloat v[3 * 4] = {
			rcMinX, rcMaxY, 0,
			rcMinX, rcMinY, 0,
			rcMaxX, rcMaxY, 0,
			rcMaxX, rcMinY, 0,
		};

		CMat4 matLine = m_context->m_gles->m_mvpMat;
		matLine *= DisplayUtils::createModelMatrix(vertX, vertY, vertZ, _azimuth, _pitch + 270.0, _roll, scale);

		glUniform1i(ShaderBase::Instance().m_Is2DCam, 0);
		glUniform1i(ShaderBase::Instance().m_Is2DPos, 0);
		glUniform1i(ShaderBase::Instance().m_SlotFragOp, eFsMode_onlyTex);
		glUniformMatrix4fv(ShaderBase::Instance().m_SlotMVP, 1, GL_FALSE, matLine);
		glEnableVertexAttribArray(ShaderBase::Instance().m_SlotATexST);
		glEnableVertexAttribArray(ShaderBase::Instance().m_SlotAPos);

		glVertexAttribPointer(ShaderBase::Instance().m_SlotATexST, 2, GL_FLOAT, 0, 0, t);
		glVertexAttribPointer(ShaderBase::Instance().m_SlotAPos, 3, GL_FLOAT, 0, 0, v);

		glBindTexture(GL_TEXTURE_2D, _lineBackTexture);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindTexture(GL_TEXTURE_2D, _lineTexture);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(ShaderBase::Instance().m_SlotATexST);
		glDisableVertexAttribArray(ShaderBase::Instance().m_SlotAPos);

		glDisableVertexAttribArray(ShaderBase::Instance().m_SlotATexST);
		glDisableVertexAttribArray(ShaderBase::Instance().m_SlotAPos);
		glUniform1i(ShaderBase::Instance().m_SlotFragOp, eFsMode_onlyUClr);
		glUniformMatrix4fv(ShaderBase::Instance().m_SlotMVP, 1, GL_FALSE, m_context->m_gles->m_mvpMat3D);
		glUniform1i(ShaderBase::Instance().m_Is2DPos, 1);
		glUniform1i(ShaderBase::Instance().m_IsMulMv, 0);
	}

	glDepthMask(GL_TRUE);              // 深度缓存为 读/写

	/* Reset Param */
	glFrontFace(GL_CCW);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}


// 绘制文字3D
CMat4 TextPainter::getText3DMat(const String& _text, const float _x, float _y, int16 _angle/* = 0*/, float _z/* = 0.0f*/) const
{
	if (m_context == nullptr) {
		return {};
	}
	double scale = m_context->m_gles->m_pCamera->pixel2point(1.0);
	return DisplayUtils::createModelMatrix(_x, _y, _z, -_angle, 270.0, 0.0, scale);
}

MWTextSize TextPainter::breakTextIntoLine(const String &_text, double _maxLength, StringArray &_lines) const
{
	if (m_context == nullptr) {
		return {};
	}

	MWTextSize textSize;
	int32 fontSize = roundDoubleToInt(m_context->m_gles->m_pCamera->point2pixel(m_font.m_PointSize));

	// 循环分行
	int count = _text.length();
	float wordLen = 0.0f, lineLen = 0.0f;
	int startIdx = 0, lastWordEndIdx = 0;
	for (int endIdx = 0; endIdx <= count; ++endIdx)
	{
		uint32 charCode = endIdx < count ? _text[endIdx] : 0;

		// If the character is <space> / <LF> / <NULL>
		if (charCode == 32 || charCode == 10 || charCode == 0)
		{
			// 空格号时 如果长度未超过 则不换行
			if (charCode == 32)
			{
				wordLen += fontSize / 2;

				// 如果超过最大则换行 不包含最后语句
				if (lineLen + wordLen >= _maxLength && startIdx < lastWordEndIdx)
				{
					_lines.add(_text.substring(startIdx, lastWordEndIdx).trim());
					if (textSize.w < lineLen) {
						textSize.w = lineLen;
					}
					textSize.h += fontSize;
					lineLen = 0.0f;
					startIdx = lastWordEndIdx;
				}

				lineLen += wordLen;
				wordLen = 0.0f;
			}
			else if (charCode == 10 || charCode == 0)
			{
				// 超长时先进行换行 如果超过最大则换行 不包含最后语句
				if (lineLen + wordLen >= _maxLength && startIdx < lastWordEndIdx)
				{
					_lines.add(_text.substring(startIdx, lastWordEndIdx).trim());
					if (textSize.w < lineLen) {
						textSize.w = lineLen;
					}
					textSize.h += fontSize;
					lineLen = 0.0f;
					startIdx = lastWordEndIdx;
				}

				// 换行
				lineLen += wordLen;
				_lines.add(_text.substring(startIdx, endIdx).trim());
				if (textSize.w < lineLen) {
					textSize.w = lineLen;
				}
				textSize.h += fontSize;
				lineLen = 0.0f;
				wordLen = 0.0f;
				startIdx = endIdx + 1;
			}

			lastWordEndIdx = endIdx + 1;
			continue;
		}

		FontImage* textImage = m_context->m_gles->m_fontMgr.GetFontTexture(charCode, fontSize, 1);
		if (textImage == nullptr || 
			textImage->image == nullptr || 
			textImage->outlineImage == nullptr) 
		{
			MN_LOG_DEBUG("[GLES_CANVAS]", "charCode=%d", charCode);
			continue;
		}

		wordLen += textImage->advanceX;
	}

	return textSize;
}

void TextPainter::drawFontImage(MVImage *image, 
								float _x, float _y, 
								float _z/* = 0*/) const
{
	if (m_context == nullptr) {
		return;
	}

	GLfloat texX = (GLfloat)image->offsetX / (GLfloat)GLES_ATLAS_TEX_SIZE;
	GLfloat texY = (GLfloat)image->offsetY / (GLfloat)GLES_ATLAS_TEX_SIZE;
	GLfloat texW = (GLfloat)image->width / (GLfloat)GLES_ATLAS_TEX_SIZE;
	GLfloat texH = (GLfloat)image->height / (GLfloat)GLES_ATLAS_TEX_SIZE;

	GLfloat texcs[8] = {
		texX + texW, texY,
		texX + texW, texY + texH,
		texX, texY,
		texX, texY + texH,
	};
	GLfloat verts[12] = {
		_x + image->width, _z, _y + 0,
		_x + image->width, _z, _y + image->height,
		_x + 0, _z, _y + 0,
		_x + 0, _z, _y + image->height,
	};

	glBindTexture(GL_TEXTURE_2D, image->texture);
	glVertexAttribPointer(ShaderTexturePure::Instance().slotATexCoords, 2, GL_FLOAT, 0, 0, texcs);
	glVertexAttribPointer(ShaderTexturePure::Instance().slotAVertCoords, 3, GL_FLOAT, 0, 0, verts);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


// 判断语言
#define IS_UNICODE_HEBREW(c) ((c) >= 0x0590 && (c) <= 0x05FF)
#define IS_UNICODE_ARABIC(c) ((c) >= 0x0600 && (c) <= 0x06FF) || ((c) >= 0x0750 && (c) <= 0x077F)

// 语言处理情况
enum ELangCase {
	eLangCaseNone = 0,			// 初始状态
	eLangCaseNormal,			// 一般情况 左向右书写
	eLangCaseArabic,			// 阿拉伯语 右向左书写 需要变换
	eLangCaseHebrew,			// 希伯来语 右向左书写
};

// 显示文字处理
String TextPainter::textForShow(const String &_text) const
{
	String show;

	int32 length = _text.length();
	int32 startIdx = 0, endIdx = 0;
	ELangCase langLast = eLangCaseNone;
	for (; endIdx < length; ++endIdx)
	{
		uint32 code = _text[endIdx];

		// 判断语言
		ELangCase langCur = eLangCaseNone;
		if (IS_UNICODE_ARABIC(code)) {
			langCur = eLangCaseArabic;
		}
		else if (IS_UNICODE_HEBREW(code)) {
			langCur = eLangCaseHebrew;
		}
		else if (code == 0x20) {
			// 空格不改变语言
			langCur = langLast;
		}
		else {
			langCur = eLangCaseNormal;
		}

		// 如果起始位置字符没有语言 则跟随后续语言
		if (langCur != langLast)
		{
			if (langLast == eLangCaseNone && endIdx > 0) {
				langLast = langCur;
			}
		}

		// 语言切换处理
		if (langCur != langLast)
		{
			switch (langLast)
			{
			case eLangCaseNone:
				break;
			case eLangCaseNormal:
				show << _text.substring(startIdx, endIdx);
				break;
			case eLangCaseArabic:
				show << textArabic(_text, startIdx, endIdx);
				break;
			case eLangCaseHebrew:
				show << textReverse(_text, startIdx, endIdx);
				break;
			default:
				break;
			}

			// 进入下一段子句
			langLast = langCur;
			startIdx = endIdx;
		}
	}

	switch (langLast)
	{
	case eLangCaseNone:
		break;
	case eLangCaseNormal:
		show << _text.substring(startIdx, endIdx);
		break;
	case eLangCaseArabic:
		show << textArabic(_text, startIdx, endIdx);
		break;
	case eLangCaseHebrew:
		show << textReverse(_text, startIdx, endIdx);
		break;
	default:
		break;
	}

	return show;
}


// 字符串颠倒
String TextPainter::textReverse(const String &_text, int32 _startIdx, int32 _endIdx) const
{
	String out;
	for (int32 i = _endIdx - 1; i >= _startIdx; --i) {
		out << _text[i];
	}
	return out;
}


// 阿拉伯语处理
// first, last, middle, alone
const uint16 kArbicPosition[][4] = {
	{0xfe80, 0xfe80, 0xfe80, 0xfe80},  // 0x621
	{0xfe82, 0xfe81, 0xfe82, 0xfe81},
	{0xfe84, 0xfe83, 0xfe84, 0xfe83},
	{0xfe86, 0xfe85, 0xfe86, 0xfe85},
	{0xfe88, 0xfe87, 0xfe88, 0xfe87},
	{0xfe8a, 0xfe8b, 0xfe8c, 0xfe89},
	{0xfe8e, 0xfe8d, 0xfe8e, 0xfe8d},
	{0xfe90, 0xfe91, 0xfe92, 0xfe8f},  // 0x628
	{0xfe94, 0xfe93, 0xfe93, 0xfe93},
	{0xfe96, 0xfe97, 0xfe98, 0xfe95},  // 0x62A
	{0xfe9a, 0xfe9b, 0xfe9c, 0xfe99},
	{0xfe9e, 0xfe9f, 0xfea0, 0xfe9d},
	{0xfea2, 0xfea3, 0xfea4, 0xfea1},
	{0xfea6, 0xfea7, 0xfea8, 0xfea5},
	{0xfeaa, 0xfea9, 0xfeaa, 0xfea9},
	{0xfeac, 0xfeab, 0xfeac, 0xfeab},  // 0x630  
	{0xfeae, 0xfead, 0xfeae, 0xfead},
	{0xfeb0, 0xfeaf, 0xfeb0, 0xfeaf},
	{0xfeb2, 0xfeb3, 0xfeb4, 0xfeb1},
	{0xfeb6, 0xfeb7, 0xfeb8, 0xfeb5},
	{0xfeba, 0xfebb, 0xfebc, 0xfeb9},
	{0xfebe, 0xfebf, 0xfec0, 0xfebd},
	{0xfec2, 0xfec3, 0xfec4, 0xfec1},
	{0xfec6, 0xfec7, 0xfec8, 0xfec5},  // 0x638
	{0xfeca, 0xfecb, 0xfecc, 0xfec9},
	{0xfece, 0xfecf, 0xfed0, 0xfecd},  // 0x63A
	{0x063b, 0x063b, 0x063b, 0x063b},
	{0x063c, 0x063c, 0x063c, 0x063c},
	{0x063d, 0x063d, 0x063d, 0x063d},
	{0x063e, 0x063e, 0x063e, 0x063e},
	{0x063f, 0x063f, 0x063f, 0x063f},
	{0x0640, 0x0640, 0x0640, 0x0640},  // 0x640
	{0xfed2, 0xfed3, 0xfed4, 0xfed1},
	{0xfed6, 0xfed7, 0xfed8, 0xfed5},
	{0xfeda, 0xfedb, 0xfedc, 0xfed9},
	{0xfede, 0xfedf, 0xfee0, 0xfedd},
	{0xfee2, 0xfee3, 0xfee4, 0xfee1},
	{0xfee6, 0xfee7, 0xfee8, 0xfee5},
	{0xfeea, 0xfeeb, 0xfeec, 0xfee9},
	{0xfeee, 0xfeed, 0xfeee, 0xfeed},  // 0x648
	{0xfef0, 0xfeef, 0xfef0, 0xfeef},
	{0xfef2, 0xfef3, 0xfef4, 0xfef1},  // 0x64A

	{0xfb57, 0xfb58, 0xfb59, 0xfb56},	// 0x067e 42
	{0xfb7b, 0xfb7c, 0xfb7d, 0xfb7a},	// 0x0686 43
	{0xfb8b, 0xfb8a, 0xfb8b, 0xfb8a},	// 0x0698 44
	{0xfb8f, 0xfb90, 0xfb91, 0xfb8e},	// 0x06a9 45
	{0xfb93, 0xfb94, 0xfb95, 0xfb92},	// 0x06af 46
	{0xfbfd, 0xfbfe, 0xfbff, 0xfbfc},	// 0x06cc 47
};

const std::set<uint16> kArbicFirstSet = {
	0x626, 0x628, 0x62a, 0x62b, 0x62c,
	0x62d, 0x62e, 0x633, 0x634, 0x635,
	0x636, 0x637, 0x638, 0x639, 0x63a,
	0x641, 0x642, 0x643, 0x644, 0x645,
	0x646, 0x647, 0x64a,

	// Persian
	0x67e, 0x686, 0x698, 0x6a9, 0x6af, 0x6cc,
};

const std::set<uint16> kArbicLastSet = {
	0x622, 0x623, 0x624, 0x625, 0x626,
	0x627, 0x628, 0x629, 0x62a, 0x62b,
	0x62c, 0x62d, 0x62e, 0x62f, 0x630,
	0x631, 0x632, 0x633, 0x634, 0x635,
	0x636, 0x637, 0x638, 0x639, 0x63a,
	0x641, 0x642, 0x643, 0x644, 0x645,
	0x646, 0x647, 0x648, 0x649, 0x64a,

	// Persian
	0x67e, 0x686, 0x698, 0x6a9, 0x6af, 0x6cc,
};

// 阿拉伯语变换
String TextPainter::textArabic(const String &_text, int32 _startIdx, int32 _endIdx) const
{
	String out;

	juce_wchar prev = 0;
	juce_wchar curr = 0;
	juce_wchar next = _text[_startIdx];
	for (int32 i = _startIdx; i < _endIdx; ++i)
	{
		curr = next;
		next = i >= _endIdx ? 0 : _text[i + 1];

		// 规则2
		bool bRule2 = false;
		if (curr == 0x644)
		{
			bRule2 = true;
			switch (next) {
			case 0x622:
				out << (juce_wchar)(kArbicFirstSet.count(prev) == 0 ? 0xFEF5 : 0xFEF6);
				break;
			case 0x623:
				out << (juce_wchar)(kArbicFirstSet.count(prev) == 0 ? 0xFEF7 : 0xFEF8);
				break;
			case 0x625:
				out << (juce_wchar)(kArbicFirstSet.count(prev) == 0 ? 0xFEF9 : 0xFEFA);
				break;
			case 0x627:
				out << (juce_wchar)(kArbicFirstSet.count(prev) == 0 ? 0xFEFB : 0xFEFC);
				break;
			default:
				bRule2 = false;
				break;
			}
			if (bRule2) {
				// 向前推进一次
				prev = curr;
				i++;
				curr = next;
				next = i >= _endIdx ? 0 : _text[i + 1];
			}
		}

		// 规则1
		if (!bRule2)
		{
			// 变体索引位置
			int32 posIdx = -1;
			if (curr >= 0x621 && curr <= 0x64A) {
				// 阿拉伯语
				posIdx = curr - 0x621;
			}
			else {
				// 波斯语
				switch (curr) {
				case 0x067e: posIdx = 42; break;
				case 0x0686: posIdx = 43; break;
				case 0x0698: posIdx = 44; break;
				case 0x06a9: posIdx = 45; break;
				case 0x06af: posIdx = 46; break;
				case 0x06cc: posIdx = 47; break;
				default:
					break;
				}
			}

			juce_wchar code = curr;
			if (posIdx >= 0)
			{
				// 前连判断
				bool prevConn = kArbicFirstSet.count(prev);
				// 后连判断
				bool nextConn = kArbicLastSet.count(next);
				// 索引
				if (prevConn && nextConn) { // first
					code = kArbicPosition[posIdx][2];
				}
				else if (prevConn) { // last
					code = kArbicPosition[posIdx][0];
				}
				else if (nextConn) { // middle
					code = kArbicPosition[posIdx][1];
				}
				else { // alone
					code = kArbicPosition[posIdx][3];
				}
			}

			out << code;
		}
		else {
			out << curr;
		}
		prev = curr;
	}

	return textReverse(out, 0, out.length());
}


} // namespace mapworld