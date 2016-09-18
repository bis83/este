#include "Direct2D.h"
#include "Game.h"

Geometry::Geometry(Drawer *pDrawer) : DrawObject(pDrawer), m_pGeometry(NULL), pBrush(NULL) {
}

Geometry::Geometry(Drawer *pDrawer, Pass *pPasses, int nPass) :
DrawObject(pDrawer),
m_pGeometry(NULL),
pBrush(NULL) {
	if(!Initialize(pPasses, nPass)) {
		MESSAGE_BOX(L"�W�I���g���̏������Ɏ��s");
	}
}

Geometry::Geometry(Drawer *pDrawer, float left, float right, float top, float bottom) :
DrawObject(pDrawer),
m_pGeometry(NULL),
pBrush(NULL) {
	if(!Initialize(left, right, top, bottom)) {
		MESSAGE_BOX(L"�W�I���g���̏������Ɏ��s");
	}
}

Geometry::Geometry(Drawer *pDrawer, float ox, float oy, float rx, float ry, void *NoUse) :
DrawObject(pDrawer),
m_pGeometry(NULL),
pBrush(NULL) {
	if(!Initialize(ox, oy, rx, ry, NULL)) {
		MESSAGE_BOX(L"�W�I���g���̏������Ɏ��s");
	}
}

Geometry::Geometry(Drawer *pDrawer, float left, float right, float top, float bottom, float rx, float ry) :
DrawObject(pDrawer),
m_pGeometry(NULL),
pBrush(NULL) {
	if(!Initialize(left, right, top, bottom, rx, ry)) {
		MESSAGE_BOX(L"�W�I���g���̏������Ɏ��s");
	}
}

Geometry::Geometry(Drawer *pDrawer, int fill, Geometry **ppGeometries, int nGeometries) :
DrawObject(pDrawer),
m_pGeometry(NULL),
pBrush(NULL) {
	if(!Initialize(fill, ppGeometries, nGeometries)) {
		MESSAGE_BOX(L"�W�I���g���̏������Ɏ��s");
	}
}

Geometry::Geometry(Drawer *pDrawer, Geometry *pGeometry, Translation *pTrans) :
DrawObject(pDrawer),
m_pGeometry(NULL),
pBrush(NULL) {
	if(!Initialize(pGeometry, pTrans)) {
		MESSAGE_BOX(L"�W�I���g���̏������Ɏ��s");
	}
}

Geometry::~Geometry() {
	SAFE_RELEASE(m_pGeometry);
	SAFE_RELEASE(pBrush);
}

bool Geometry::Initialize(Pass *pPasses, int nPass) {
	// �p�X�W�I���g���̐���
	ID2D1PathGeometry *pG = NULL;
	if(FAILED(pDrawer->GetD2D1Factory()->CreatePathGeometry(&pG))) {
		FITAL_ERROR(L"�p�X�W�I���g���̐����Ɏ��s");
		return false;
	}

	// �W�I���g���V���N�̎擾
	ID2D1GeometrySink *pSink = NULL;
	if(FAILED(pG->Open(&pSink))) {
		FITAL_ERROR(L"�W�I���g���V���N�̎擾�Ɏ��s");
		SAFE_RELEASE(pG);
		return false;
	}
	
	// �W�I���g���p�X�̓���
	for(int i=0; i<nPass; ++i) {
		switch(pPasses[i].type) {
		case PT_START:
			pSink->BeginFigure(D2D1::Point2F(
				pPasses[i].data.PassStart.x, pPasses[i].data.PassStart.y),
				(D2D1_FIGURE_BEGIN)pPasses[i].data.PassStart.filled);
			break;
		case PT_END:
			pSink->EndFigure((D2D1_FIGURE_END)pPasses[i].data.PassEnd.close);
			break;
		case PT_LINE:
			pSink->AddLine(D2D1::Point2F(pPasses[i].data.PassLine.x, pPasses[i].data.PassLine.y));
			break;
		case PT_BEZIER:
			pSink->AddBezier(D2D1::BezierSegment(
				D2D1::Point2F(pPasses[i].data.PassBezier.c1x, pPasses[i].data.PassBezier.c1y),
				D2D1::Point2F(pPasses[i].data.PassBezier.c2x, pPasses[i].data.PassBezier.c2y),
				D2D1::Point2F(pPasses[i].data.PassBezier.x, pPasses[i].data.PassBezier.y)));
			break;
		case PT_QUAD_BEZIER:
			pSink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
				D2D1::Point2F(pPasses[i].data.PassQuadBezier.cx, pPasses[i].data.PassQuadBezier.cy),
				D2D1::Point2F(pPasses[i].data.PassQuadBezier.x, pPasses[i].data.PassQuadBezier.y)));
			break;
		case PT_ARC:
			pSink->AddArc(D2D1::ArcSegment(
				D2D1::Point2F(pPasses[i].data.PassArc.x, pPasses[i].data.PassArc.y),
				D2D1::SizeF(pPasses[i].data.PassArc.rx, pPasses[i].data.PassArc.ry),
				pPasses[i].data.PassArc.a,
				(D2D1_SWEEP_DIRECTION)pPasses[i].data.PassArc.clockwise,
				(D2D1_ARC_SIZE)pPasses[i].data.PassArc.large));
			break;
		default:
			MESSAGE_BOX(L"�����ȃp�X���܂܂�Ă��܂�");
			break;
		}
	}

	// �W�I���g���V���N�����
	if(FAILED(pSink->Close())) {
		FITAL_ERROR(L"�W�I���g���V���N�̃N���[�Y�Ɏ��s");
		SAFE_RELEASE(pSink);
		SAFE_RELEASE(pG);
		return false;
	}

	SAFE_RELEASE(pSink);

	// ���������W�I���g���������o�ϐ��ɐݒ�
	m_pGeometry = (ID2D1Geometry*)pG;

	return true;
}

bool Geometry::Initialize(float left, float right, float top, float bottom) {
	if(FAILED(pDrawer->GetD2D1Factory()->CreateRectangleGeometry(
		D2D1::RectF(left, top, right, bottom),
		(ID2D1RectangleGeometry**)&m_pGeometry))) {
			FITAL_ERROR(L"��`�W�I���g���̐����Ɏ��s");
			return false;
	}

	return true;
}

bool Geometry::Initialize(float ox, float oy, float rx, float ry, void *pNoUse) {
	if(FAILED(pDrawer->GetD2D1Factory()->CreateEllipseGeometry(
		D2D1::Ellipse(D2D1::Point2F(ox, oy), rx, ry),
		(ID2D1EllipseGeometry**)&m_pGeometry))) {
			FITAL_ERROR(L"�ȉ~�W�I���g���̐����Ɏ��s");
			return false;
	}

	return true;
}

bool Geometry::Initialize(float left, float right, float top, float bottom, float rx, float ry) {
	if(FAILED(pDrawer->GetD2D1Factory()->CreateRoundedRectangleGeometry(
		D2D1::RoundedRect(D2D1::RectF(left, top, right, bottom), rx, ry),
		(ID2D1RoundedRectangleGeometry**)&m_pGeometry))) {
			FITAL_ERROR(L"�ۊp��`�W�I���g���̐����Ɏ��s");
			return false;
	}

	return true;
}

bool Geometry::Initialize(int fill, Geometry **ppGeometries, int nGeometries) {
	ID2D1Geometry **ppGeos = new ID2D1Geometry*[nGeometries];
	for(int i=0; i<nGeometries; ++i) {
		ppGeos[i] = ppGeometries[i]->GetGeometry();
	}
	if(FAILED(pDrawer->GetD2D1Factory()->CreateGeometryGroup((D2D1_FILL_MODE)fill,
		ppGeos, nGeometries, (ID2D1GeometryGroup**)&m_pGeometry))) {
			FITAL_ERROR(L"�W�I���g���O���[�v�̐����Ɏ��s");
			delete[] ppGeos;
			return false;
	}

	delete[] ppGeos;
	return true;
}

bool Geometry::Initialize(Geometry *pGeometry, Translation *pTrans) {
	if(FAILED(pDrawer->GetD2D1Factory()->CreateTransformedGeometry(
		pGeometry->GetGeometry(),
		pTrans->GetMatrix(),
		(ID2D1TransformedGeometry**)&m_pGeometry))) {
			FITAL_ERROR(L"�ϊ��ς݃W�I���g���̐����Ɏ��s");
			return false;
	}
	return true;
}

void Geometry::SetBrush(Brush *pBrush) {
	SAFE_RELEASE(this->pBrush);
	this->pBrush = pBrush->GetBrush();
	if(this->pBrush) {
		this->pBrush->AddRef();
	}
}

void Geometry::Draw() {
	pDrawer->GetRenderTarget()->DrawGeometry(m_pGeometry, pBrush);
}

void Geometry::Draw(float size) {
	pDrawer->GetRenderTarget()->DrawGeometry(m_pGeometry, pBrush, size);
}

void Geometry::Fill() {
	pDrawer->GetRenderTarget()->FillGeometry(m_pGeometry, pBrush);
}

void Geometry::Fill(Brush *pOpacityBrush) {
	pDrawer->GetRenderTarget()->FillGeometry(m_pGeometry, pBrush, pOpacityBrush->GetBrush());
}
