#include "Direct2D.h"
#include "Game.h"

TextFormat::TextFormat(Drawer *pDrawer) : DrawObject(pDrawer), m_pFormat(NULL), pBrush(NULL) {
}

TextFormat::TextFormat(Drawer *pDrawer, LPCWSTR font, int weight, int style, int stretch, float size, LPCWSTR locale) :
DrawObject(pDrawer), m_pFormat(NULL), pBrush(NULL) {
	if(!Initialize(font, weight, style, stretch, size, locale)) {
		MESSAGE_BOX(L"テキストフォーマットの初期化に失敗");
	}
}

TextFormat::TextFormat(TextFormat &t) : DrawObject(pDrawer), m_pFormat(t.m_pFormat), pBrush(t.pBrush) {
	if(m_pFormat) {
		m_pFormat->AddRef();
	}
}

TextFormat::~TextFormat() {
	SAFE_RELEASE(m_pFormat);
	SAFE_RELEASE(pBrush);
}

bool TextFormat::Initialize(LPCWSTR font, int weight, int style, int stretch, float size, LPCWSTR locale) {
	if(FAILED(pDrawer->GetDWriteFactory()->CreateTextFormat(font, NULL,
		(DWRITE_FONT_WEIGHT)weight,
		(DWRITE_FONT_STYLE)style,
		(DWRITE_FONT_STRETCH)stretch,
		size, locale, &m_pFormat)))
	{
		FITAL_ERROR(L"テキストフォーマットの生成に失敗");
		return false;
	}
	return true;
}

void TextFormat::SetBrush(Brush *pBrush) {
	SAFE_RELEASE(this->pBrush);
	this->pBrush = pBrush->GetBrush();
	if(this->pBrush) {
		this->pBrush->AddRef();
	}
}

void TextFormat::Draw(wstring text) {
	D2D1_SIZE_F size = pDrawer->GetRenderTarget()->GetSize();
	pDrawer->GetRenderTarget()->DrawText(text.c_str(), text.length(), m_pFormat, D2D1::RectF(0, 0, size.width, size.height), pBrush);
}


TextLayout::TextLayout(Drawer *pDrawer) : DrawObject(pDrawer), m_pLayout(NULL), pBrush(NULL) {
}

TextLayout::TextLayout(Drawer *pDrawer, wstring text, TextFormat *pFormat) : DrawObject(pDrawer), m_pLayout(NULL), pBrush(NULL) {
	if(!Initialize(text, pFormat)) {
		MESSAGE_BOX(L"テキストレイアウトの初期化に失敗");
	}
}

TextLayout::TextLayout(TextLayout &t) : DrawObject(pDrawer), m_pLayout(t.m_pLayout), pBrush(t.pBrush) {
	if(m_pLayout) {
		m_pLayout->AddRef();
	}
}

TextLayout::~TextLayout() {
	SAFE_RELEASE(m_pLayout);
	SAFE_RELEASE(pBrush);
}

bool TextLayout::Initialize(wstring text, TextFormat *pFormat) {
	D2D1_SIZE_F size = pDrawer->GetRenderTarget()->GetSize();
	if(FAILED(pDrawer->GetDWriteFactory()->CreateTextLayout(text.c_str(), text.length(), pFormat->GetTextFormat(), size.width, size.height, &m_pLayout))) {
		FITAL_ERROR(L"テキストレイアウトの生成に失敗");
		return false;
	}
	return true;
}

void TextLayout::SetDefaultBrush(Brush *pBrush) {
	SAFE_RELEASE(this->pBrush);
	this->pBrush = pBrush->GetBrush();
	if(this->pBrush) {
		this->pBrush->AddRef();
	}
}

void TextLayout::SetBrush(Brush *pBrush, unsigned int start, unsigned int length) {
	DWRITE_TEXT_RANGE range = { start, length };
	m_pLayout->SetDrawingEffect((IUnknown*)pBrush->GetBrush(), range);
}

void TextLayout::SetFont(LPCWSTR fontname, unsigned int start, unsigned int length) {
	DWRITE_TEXT_RANGE range = { start, length };
	m_pLayout->SetFontFamilyName(fontname, range);
}

void TextLayout::SetSize(float size, unsigned int start, unsigned int length) {
	DWRITE_TEXT_RANGE range = { start, length };
	m_pLayout->SetFontSize(size, range);
}

void TextLayout::SetStretch(int stretch, unsigned int start, unsigned int length) {
	DWRITE_TEXT_RANGE range = { start, length };
	m_pLayout->SetFontStretch((DWRITE_FONT_STRETCH)stretch, range);
}

void TextLayout::SetWeight(int weight, unsigned int start, unsigned int length) {
	DWRITE_TEXT_RANGE range = { start, length };
	m_pLayout->SetFontWeight((DWRITE_FONT_WEIGHT)weight, range);
}

void TextLayout::SetStrikethrough(bool strinkethrough, unsigned int start, unsigned int length) {
	DWRITE_TEXT_RANGE range = { start, length };
	m_pLayout->SetStrikethrough(strinkethrough, range);
}

void TextLayout::SetUnderline(bool underline, unsigned int start, unsigned int length) {
	DWRITE_TEXT_RANGE range = { start, length };
	m_pLayout->SetUnderline(underline, range);
}

void TextLayout::Draw() {
	pDrawer->GetRenderTarget()->DrawTextLayout(D2D1::Point2F(), m_pLayout, pBrush);
}
