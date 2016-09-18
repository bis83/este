#include "Direct2D.h"
#include "Game.h"

Bitmap::Bitmap(Drawer *pDrawer) : DrawObject(pDrawer), m_pBitmap(NULL) {
}

Bitmap::Bitmap(Drawer *pDrawer, LPCWSTR filename) : DrawObject(pDrawer), m_pBitmap(NULL) {
	if(!Initialize(filename)) {
		MESSAGE_BOX(L"ビットマップの初期化に失敗");
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

	// 必要な各オブジェクトの定義
	IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

	// デコーダの生成
	HRESULT hr;
	if(FAILED(hr= pDrawer->GetWICFactory()->CreateDecoderFromFilename(
        filename,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
        )))
	{
		FITAL_ERROR(L"デコーダの生成に失敗");
	}

	// フレーム情報の取得
	if (SUCCEEDED(hr)) {
        if(FAILED(hr = pDecoder->GetFrame(0, &pSource))) {
			FITAL_ERROR(L"フレーム情報の取得に失敗");
		}
    }

	// コンバータの生成
	if (SUCCEEDED(hr)) {
		if(FAILED(hr = pDrawer->GetWICFactory()->CreateFormatConverter(&pConverter))) {
			FITAL_ERROR(L"コンバータの生成に失敗");
		}
    }

	// ソースのコンバート
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
			FITAL_ERROR(L"コンバートに失敗");
		}
    }
	
	// ビットマップの生成
	if (SUCCEEDED(hr)) {
		if(FAILED(hr = pDrawer->GetRenderTarget()->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            &m_pBitmap
            )))
		{
			FITAL_ERROR(L"ビットマップの生成に失敗");
		}
    }

	// 使用したオブジェクトの開放
    SAFE_RELEASE(pDecoder);
    SAFE_RELEASE(pSource);
    SAFE_RELEASE(pStream);
    SAFE_RELEASE(pConverter);
    SAFE_RELEASE(pScaler);

	// 結果のリターン
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
