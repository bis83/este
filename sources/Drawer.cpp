#include "Direct2D.h"
#include "Game.h"

Drawer::Drawer() :
m_pFactory2D(NULL),
m_pFactoryWrite(NULL),
m_pFactoryWIC(NULL),
m_pRenderTarget(NULL) {
}

Drawer::Drawer(HWND hWnd) :
m_pFactory2D(NULL),
m_pFactoryWrite(NULL),
m_pFactoryWIC(NULL),
m_pRenderTarget(NULL) {
	if(!Initialize(hWnd)) {
		MESSAGE_BOX(L"Drawer�̏������Ɏ��s");
	}
}

Drawer::~Drawer() {
	SAFE_RELEASE(m_pFactory2D);
	SAFE_RELEASE(m_pFactoryWrite);
	SAFE_RELEASE(m_pFactoryWIC);
	SAFE_RELEASE(m_pRenderTarget);
}

bool Drawer::Initialize(HWND hWnd) {
	// D2D1�t�@�N�g���̐���
	if(FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory2D))) {
		FITAL_ERROR(L"D2D1Factory�̐����Ɏ��s");
		return false;
	}

	// DWrite�t�@�N�g���̐���
	if(FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pFactoryWrite))) {
		FITAL_ERROR(L"DirectWriteFactory�̐����Ɏ��s");
		return false;
	}

	// WICFactory�̐���
	if(FAILED(CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pFactoryWIC)
            )))
	{
		FITAL_ERROR(L"WICFactory�̐����Ɏ��s");
		return false;
	}

	// �����_�[�^�[�Q�b�g�̐���
	RECT rect;
	GetClientRect(hWnd, &rect);
	if(FAILED(m_pFactory2D->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top)),
		&m_pRenderTarget)))
	{
		FITAL_ERROR(L"RenderTarget�̐����Ɏ��s");
		return false;
	}

	// �I��
	return true;
}

void Drawer::Clear(float r, float g, float b) {
	m_pRenderTarget->SetTransform(D2D1::IdentityMatrix());
	m_pRenderTarget->Clear(D2D1::ColorF(r, g, b));
}

void Drawer::ResetTranslation() {
	m_pRenderTarget->SetTransform(D2D1::IdentityMatrix());
}

void Drawer::BeginRender() {
	m_pRenderTarget->BeginDraw();
}

void Drawer::EndRender() {
	m_pRenderTarget->EndDraw();
}


DrawObject::DrawObject(Drawer *pDrawer) : pDrawer(pDrawer) {
}

DrawObject::DrawObject(DrawObject &d) : pDrawer(d.pDrawer) {
}

DrawObject::~DrawObject() {
}
