#include "Direct2D.h"
#include "Game.h"

Bitmap::Bitmap(Drawer *pDrawer) : DrawObject(pDrawer), m_pBitmap(NULL) {
}

Bitmap::Bitmap(Drawer *pDrawer, LPCWSTR filename) : DrawObject(pDrawer), m_pBitmap(NULL) {
	if(!Initialize(filename)) {
		MESSAGE_BOX(L"�r�b�g�}�b�v�̏������Ɏ��s");
	}
}

Bitmap::Bitmap(Bitmap &b) : DrawObject(b.pDrawer), m_pBitmap(b.m_pBitmap) {
	if(m_pBitmap) {
		m_pBitmap->AddRef();
	}
}

Bitmap::~Bitmap() {
	SAFE_RELEASE(m_pBitmap);
}

bool Bitmap::Initialize(LPCWSTR filename) {

	// �K�v�Ȋe�I�u�W�F�N�g�̒�`
	IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

	// �f�R�[�_�̐���
	HRESULT hr;
	if(FAILED(hr= pDrawer->GetWICFactory()->CreateDecoderFromFilename(
        filename,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
        )))
	{
		FITAL_ERROR(L"�f�R�[�_�̐����Ɏ��s");
	}

	// �t���[�����̎擾
	if (SUCCEEDED(hr)) {
        if(FAILED(hr = pDecoder->GetFrame(0, &pSource))) {
			FITAL_ERROR(L"�t���[�����̎擾�Ɏ��s");
		}
    }

	// �R���o�[�^�̐���
	if (SUCCEEDED(hr)) {
		if(FAILED(hr = pDrawer->GetWICFactory()->CreateFormatConverter(&pConverter))) {
			FITAL_ERROR(L"�R���o�[�^�̐����Ɏ��s");
		}
    }

	// �\�[�X�̃R���o�[�g
    if (SUCCEEDED(hr)) {
        if(FAILED(hr = pConverter->Initialize(
            pSource,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut
            )))
		{
			FITAL_ERROR(L"�R���o�[�g�Ɏ��s");
		}
    }
	
	// �r�b�g�}�b�v�̐���
	if (SUCCEEDED(hr)) {
		if(FAILED(hr = pDrawer->GetRenderTarget()->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            &m_pBitmap
            )))
		{
			FITAL_ERROR(L"�r�b�g�}�b�v�̐����Ɏ��s");
		}
    }

	// �g�p�����I�u�W�F�N�g�̊J��
    SAFE_RELEASE(pDecoder);
    SAFE_RELEASE(pSource);
    SAFE_RELEASE(pStream);
    SAFE_RELEASE(pConverter);
    SAFE_RELEASE(pScaler);

	// ���ʂ̃��^�[��
	if(SUCCEEDED(hr)) {
		return true;
	} else {
		return false;
	}
}

void Bitmap::Draw() {
	pDrawer->GetRenderTarget()->DrawBitmap(m_pBitmap);
}

void Bitmap::Draw(float alpha) {
	pDrawer->GetRenderTarget()->DrawBitmap(m_pBitmap, NULL, alpha);
}

void Bitmap::Draw(float left, float right, float top, float bottom) {
	pDrawer->GetRenderTarget()->DrawBitmap(m_pBitmap, NULL, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &D2D1::RectF(left, top, right, bottom));
}

void Bitmap::Draw(float alpha, float left, float right, float top, float bottom) {
	pDrawer->GetRenderTarget()->DrawBitmap(m_pBitmap, NULL, alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &D2D1::RectF(left, top, right, bottom));
}
