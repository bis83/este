#include "Direct2D.h"
#include "Game.h"

Brush::Brush(Drawer *pDrawer) : DrawObject(pDrawer), m_pBrush(NULL) {
}

Brush::Brush(Drawer *pDrawer, float r, float g, float b, float a) :
DrawObject(pDrawer), m_pBrush(NULL)
{
	if(!Initialize(r, g, b, a)) {
		MESSAGE_BOX(L"�u���V�̏������Ɏ��s");
	}
}

Brush::Brush(Drawer *pDrawer, Gradient *pGradient, float sx, float sy, float ex, float ey) :
DrawObject(pDrawer),
m_pBrush(NULL) {
	if(!Initialize(pGradient, sx, sy, ex, ey)) {
		MESSAGE_BOX(L"�u���V�̏������Ɏ��s");
	}
}

Brush::Brush(Drawer *pDrawer, Gradient *pGradient, float cx, float cy, float bx, float by, float rx, float ry) :
DrawObject(pDrawer),
m_pBrush(NULL) {
	if(!Initialize(pGradient, cx, cy, bx, by, rx, ry)) {
		MESSAGE_BOX(L"�u���V�̏������Ɏ��s");
	}
}

Brush::Brush(Drawer *pDrawer, Bitmap *pBitmap, int ex_x, int ex_y) :
DrawObject(pDrawer), m_pBrush(NULL) {
	if(!Initialize(pBitmap, ex_x, ex_y)) {
		MESSAGE_BOX(L"�u���V�̏������Ɏ��s");
	}
}

Brush::Brush(Brush &b) : DrawObject(b.pDrawer), m_pBrush(b.m_pBrush) {
	if(m_pBrush) {
		m_pBrush->AddRef();
	}
}

Brush::~Brush() {
	SAFE_RELEASE(m_pBrush);
}

bool Brush::Initialize(float r, float g, float b, float a) {
	if(FAILED(pDrawer->GetRenderTarget()->CreateSolidColorBrush(&D2D1::ColorF(r, g, b, a), NULL, (ID2D1SolidColorBrush**)&m_pBrush))) {
		FITAL_ERROR(L"�P�F�J���[�u���V�̐����Ɏ��s");
		return false;
	}
	return true;
}

bool Brush::Initialize(Gradient *pGradient, float sx, float sy, float ex, float ey) {
	// �O���f�[�V�����R���N�V�����̍쐬
	D2D1_GRADIENT_STOP *stops = new D2D1_GRADIENT_STOP[pGradient->nPass];
	for(int i=0; i<pGradient->nPass; ++i) {
		stops[i].color = D2D1::ColorF(
			pGradient->pPassArray[i].r,
			pGradient->pPassArray[i].g,
			pGradient->pPassArray[i].b,
			pGradient->pPassArray[i].a);
		stops[i].position = pGradient->pPassArray[i].pos;
	}

	ID2D1GradientStopCollection *pCollection = NULL;
	HRESULT hr;

	// �O���f�[�V�����X�g�b�v�R���N�V�����̐���
	if(FAILED(hr = pDrawer->GetRenderTarget()->CreateGradientStopCollection(
		stops, pGradient->nPass, &pCollection)))
	{
			FITAL_ERROR(L"�O���f�[�V�����X�g�b�v�R���N�V�����̐����Ɏ��s");
	}

	if(SUCCEEDED(hr)) {
		// ���j�A�O���f�[�V�����u���V�̐���
		if(FAILED(hr = pDrawer->GetRenderTarget()->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties(D2D1::Point2F(sx, sy), D2D1::Point2F(ex, ey)),
			pCollection, (ID2D1LinearGradientBrush**)&m_pBrush)))
		{
			FITAL_ERROR(L"���`�O���f�[�V�����u���V�̐����Ɏ��s");
		}
	}

	SAFE_RELEASE(pCollection);
	delete[] stops;

	if(SUCCEEDED(hr)) {
		return true;
	} else {
		return false;
	}
}

bool Brush::Initialize(Gradient *pGradient, float cx, float cy, float bx, float by, float rx, float ry) {
	// �O���f�[�V�����R���N�V�����̍쐬
	D2D1_GRADIENT_STOP *stops = new D2D1_GRADIENT_STOP[pGradient->nPass];
	for(int i=0; i<pGradient->nPass; ++i) {
		stops[i].color = D2D1::ColorF(
			pGradient->pPassArray[i].r,
			pGradient->pPassArray[i].g,
			pGradient->pPassArray[i].b,
			pGradient->pPassArray[i].a);
		stops[i].position = pGradient->pPassArray[i].pos;
	}

	ID2D1GradientStopCollection *pCollection = NULL;
	HRESULT hr;

	// �O���f�[�V�����X�g�b�v�R���N�V�����̐���
	if(FAILED(hr = pDrawer->GetRenderTarget()->CreateGradientStopCollection(
		stops, pGradient->nPass, &pCollection)))
	{
			FITAL_ERROR(L"�O���f�[�V�����X�g�b�v�R���N�V�����̐����Ɏ��s");
	}

	if(SUCCEEDED(hr)) {
		// ���j�A�O���f�[�V�����u���V�̐���
		if(FAILED(hr = pDrawer->GetRenderTarget()->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(D2D1::Point2F(cx, cy), D2D1::Point2F(bx, by), rx, ry),
			pCollection, (ID2D1RadialGradientBrush**)&m_pBrush)))
		{
			FITAL_ERROR(L"���ˏ�O���f�[�V�����u���V�̐����Ɏ��s");
		}
	}

	SAFE_RELEASE(pCollection);
	delete[] stops;

	if(SUCCEEDED(hr)) {
		return true;
	} else {
		return false;
	}
}

bool Brush::Initialize(Bitmap *pBitmap, int ex_x, int ex_y) {
	if(FAILED(pDrawer->GetRenderTarget()->CreateBitmapBrush(pBitmap->GetBitmap(), (ID2D1BitmapBrush**)&m_pBrush))) {
		FITAL_ERROR(L"�r�b�g�}�b�v�u���V�̐����Ɏ��s");
		return false;
	}
	((ID2D1BitmapBrush*)m_pBrush)->SetExtendModeX((D2D1_EXTEND_MODE)ex_x);
	((ID2D1BitmapBrush*)m_pBrush)->SetExtendModeY((D2D1_EXTEND_MODE)ex_y);
	return true;
}

void Brush::SetTranslation(Translation *pTrans) {
	if(pTrans) {
		m_pBrush->SetTransform(pTrans->GetMatrix());
	} else {
		m_pBrush->SetTransform(D2D1::IdentityMatrix());
	}
}

void Brush::SetOpacity(float alpha) {
	m_pBrush->SetOpacity(alpha);
}

void Brush::DrawLine(float size, float sx, float sy, float ex, float ey) {
	pDrawer->GetRenderTarget()->DrawLine(D2D1::Point2F(sx, sy), D2D1::Point2F(ex, ey), m_pBrush, size);
}

void Brush::DrawRect(float size, float left, float right, float top, float bottom) {
	pDrawer->GetRenderTarget()->DrawRectangle(D2D1::RectF(left, top, right, bottom), m_pBrush, size);
}

void Brush::DrawRoundedRect(float size, float left, float right, float top, float bottom, float rx, float ry) {
	pDrawer->GetRenderTarget()->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(left, top, right, bottom), rx, ry), m_pBrush, size);
}

void Brush::DrawEllipse(float size, float ox, float oy, float rx, float ry) {
	pDrawer->GetRenderTarget()->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(ox, oy), rx, ry), m_pBrush, size);
}

void Brush::FillRect(float left, float right, float top, float bottom) {
	pDrawer->GetRenderTarget()->FillRectangle(D2D1::RectF(left, top, right, bottom), m_pBrush);
}

void Brush::FillRoundedRect(float left, float right, float top, float bottom, float rx, float ry) {
	pDrawer->GetRenderTarget()->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(left, top, right, bottom), rx, ry), m_pBrush);
}

void Brush::FillEllipse(float ox, float oy, float rx, float ry) {
	pDrawer->GetRenderTarget()->FillEllipse(D2D1::Ellipse(D2D1::Point2F(ox, oy), rx, ry), m_pBrush);
}

void Brush::FillMask(Bitmap *pMask, bool text) {
	pDrawer->GetRenderTarget()->FillOpacityMask(pMask->GetBitmap(), m_pBrush, (D2D1_OPACITY_MASK_CONTENT)text);
}

void Brush::FillMask(Bitmap *pMask, bool text, float left, float right, float top, float bottom) {
	pDrawer->GetRenderTarget()->FillOpacityMask(pMask->GetBitmap(), m_pBrush, (D2D1_OPACITY_MASK_CONTENT)text, NULL, &D2D1::RectF(left, top, right, bottom));
}
