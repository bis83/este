#include "Direct2D.h"
#include "Game.h"

Mesh::Mesh(Drawer *pDrawer) : DrawObject(pDrawer), m_pMesh(NULL), pBrush(NULL) {
}

Mesh::Mesh(Drawer *pDrawer, Triangle *pTris, int nTri) : DrawObject(pDrawer), m_pMesh(NULL), pBrush(NULL) {
	if(!Initialize(pTris, nTri)) {
		MESSAGE_BOX(L"���b�V���̏������Ɏ��s");
	}
}

Mesh::Mesh(Drawer *pDrawer, Geometry *pGeometry, Translation *pTrans) :
DrawObject(pDrawer), m_pMesh(NULL), pBrush(NULL) {
	if(!Initialize(pGeometry, pTrans)) {
		MESSAGE_BOX(L"���b�V���̏������Ɏ��s");
	}
}

Mesh::Mesh(Mesh &m) : DrawObject(pDrawer), m_pMesh(m.m_pMesh), pBrush(m.pBrush) {
	if(m_pMesh) {
		m_pMesh->AddRef();
	}
}

Mesh::~Mesh() {
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(pBrush);
}

bool Mesh::Initialize() {
	if(FAILED(pDrawer->GetRenderTarget()->CreateMesh(&m_pMesh))) {
		FITAL_ERROR(L"���b�V���̐����Ɏ��s");
		return false;
	}
	return true;
}

bool Mesh::Initialize(Triangle *pTriangles, int nTriangles) {
	if(!Initialize()) {
		return false;
	}
	return AddTriangles(pTriangles, nTriangles);
}

bool Mesh::Initialize(Geometry *pGeometry, Translation *pTrans) {
	if(!Initialize()) {
		return false;
	}

	// �e�b�����[�V�����V���N�̎擾
	ID2D1TessellationSink *pSink = NULL;
	HRESULT hr;
	
	if(FAILED(hr = m_pMesh->Open(&pSink))) {
		FITAL_ERROR(L"���b�V���V���N�̎擾�Ɏ��s");
	}

	if(SUCCEEDED(hr)) {
		if(pTrans) {
			if(FAILED(hr = pGeometry->GetGeometry()->Tessellate(pTrans->GetMatrix(), pSink))) {
				FITAL_ERROR(L"�e�b�Z���[�g�̎��s");
			}
		} else {
			if(FAILED(hr = pGeometry->GetGeometry()->Tessellate(NULL, pSink))) {
				FITAL_ERROR(L"�e�b�Z���[�g�̎��s");
			}
		}
	}

	// �V���N�����
	if(SUCCEEDED(hr)) {
		if(FAILED(hr = pSink->Close())) {
			FITAL_ERROR(L"�V���N�̃N���[�Y�Ɏ��s");
		}
	}

	SAFE_RELEASE(pSink);
	
	if(SUCCEEDED(hr)) {
		return true;
	} else {
		return false;
	}
}

bool Mesh::AddTriangles(Triangle *pTriangles, int nTriangles) {
	// �e�b�����[�V�����V���N�̎擾
	ID2D1TessellationSink *pSink = NULL;
	HRESULT hr;
	if(FAILED(hr = m_pMesh->Open(&pSink))) {
		FITAL_ERROR(L"���b�V���V���N�̎擾�Ɏ��s");
	}
	
	// �O�p�`�̒ǉ�
	if(SUCCEEDED(hr)) {
		pSink->AddTriangles((D2D1_TRIANGLE*)pTriangles, nTriangles);
		if(FAILED(hr = pSink->Close())) {
			FITAL_ERROR(L"�V���N�̃N���[�Y�Ɏ��s");
		}
	}

	SAFE_RELEASE(pSink);

	if(SUCCEEDED(hr)) {
		return true;
	} else {
		return false;
	}
}

void Mesh::SetBrush(Brush *pBrush) {
	SAFE_RELEASE(this->pBrush);
	this->pBrush = pBrush->GetBrush();
	if(this->pBrush) {
		this->pBrush->AddRef();
	}
}

void Mesh::Fill() {
	pDrawer->GetRenderTarget()->FillMesh(m_pMesh, pBrush);
}
