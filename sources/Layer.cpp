#include "Direct2D.h"
#include "Game.h"

Layer::Layer(Drawer *pDrawer) :
DrawObject(pDrawer),
m_pLayer(NULL),
bounds(D2D1::InfiniteRect()),
pGeometry(NULL),
matrix(D2D1::IdentityMatrix()),
a(1.0f),
pBrush(NULL) {
	if(!Initialize()) {
		MESSAGE_BOX(L"ƒŒƒCƒ„[‚Ì‰Šú‰»‚ÉŽ¸”s");
	}
}

Layer::Layer(Layer &l) :
DrawObject(l.pDrawer),
m_pLayer(l.m_pLayer),
bounds(l.bounds),
pGeometry(l.pGeometry),
matrix(l.matrix),
a(l.a),
pBrush(l.pBrush) {
	if(m_pLayer) {
		m_pLayer->AddRef();
	}
}

Layer::~Layer() {
	SAFE_RELEASE(m_pLayer);
	SAFE_RELEASE(pGeometry);
	SAFE_RELEASE(pBrush);
}

bool Layer::Initialize() {
	if(FAILED(pDrawer->GetRenderTarget()->CreateLayer(&m_pLayer))) {
		FITAL_ERROR(L"ƒŒƒCƒ„[‚Ì¶¬‚ÉŽ¸”s");
		return false;
	}
	return true;
}

void Layer::SetBounds(float left, float right, float top, float bottom) {
	bounds.left = left;
	bounds.right = right;
	bounds.top = top;
	bounds.bottom = bottom;
}

void Layer::SetGeometricMask(Geometry *pGeometry) {
	SAFE_RELEASE(this->pGeometry);
	this->pGeometry = pGeometry->GetGeometry();
	if(this->pGeometry) {
		this->pGeometry->AddRef();
	}
}

void Layer::SetTranslation(Translation *pTrans) {
	matrix = pTrans->GetMatrix();
}

void Layer::SetOpacity(float alpha) {
	a = alpha;
}

void Layer::SetOpacityBrush(Brush *pBrush) {
	SAFE_RELEASE(this->pBrush);
	this->pBrush = pBrush->GetBrush();
	if(this->pBrush) {
		this->pBrush->AddRef();
	}
}

void Layer::Begin() {
	pDrawer->GetRenderTarget()->PushLayer(D2D1::LayerParameters(bounds,
		pGeometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, matrix,
		a, pBrush), m_pLayer);
}

void Layer::End() {
	pDrawer->GetRenderTarget()->PopLayer();
}
