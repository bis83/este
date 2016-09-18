#pragma once

/*

Direct2D, DirectWriteなどのDirectXまわり

*/

#include <wincodec.h>

#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")

#include <string>
using namespace std;

//=============================================================
// Class : Drawer
//-------------------------------------------------------------
// 描画システムクラス。
//=============================================================
class Drawer {
	// コピーコンストラクタの禁止
	Drawer(Drawer &d) {}

	// ファクトリ
	ID2D1Factory *m_pFactory2D;
	IDWriteFactory *m_pFactoryWrite;
	IWICImagingFactory *m_pFactoryWIC;

	// レンダーターゲット
	ID2D1HwndRenderTarget *m_pRenderTarget;

public:
	Drawer();
	Drawer(HWND hWnd);
	~Drawer();

	// 初期化関数 HWND引数付のコンストラクタは標準でこれを呼ぶ
	bool Initialize(HWND hWnd);

	// レンダリングの開始と終了
	void BeginRender();
	void EndRender();

	// レンダーターゲットのクリア
	void Clear(float r, float g, float b);
	// 図形変換のリセット
	void ResetTranslation();

	// 各DirectObjectの取得
	ID2D1Factory *GetD2D1Factory() { return m_pFactory2D; }
	IDWriteFactory *GetDWriteFactory() { return m_pFactoryWrite; }
	IWICImagingFactory *GetWICFactory() { return m_pFactoryWIC; } 
	ID2D1RenderTarget *GetRenderTarget() { return m_pRenderTarget; }
};

//=============================================================
// Class : DrawObject
//-------------------------------------------------------------
// DrawObjectはDrawerから生成されるObjectの既定となるクラス。
//=============================================================
class DrawObject {
protected:
	Drawer *pDrawer;
public:
	DrawObject(Drawer *pDrawer);
	DrawObject(DrawObject &);
	virtual ~DrawObject();
};

//=============================================================
// Class : Translation
//-------------------------------------------------------------
// 図形変換を扱うオブジェクト。
// 図形変換の情報は保存されます。
// 関数が呼ばれた順番に変換が適用されるので、
// 順序に注意してください。
//=============================================================
class Translation : public DrawObject {
	D2D1_MATRIX_3X2_F m_mat;
public:
	Translation(Drawer *pDrawer);
	Translation(Translation &);
	~Translation();

	// 初期状態に戻す
	void Reset();

	// 位置(px, py原点からの距離)
	void AddPos(float px, float py);
	// 回転(ox, oyは回転の中心, aは角度)
	void AddRot(float ox, float oy, float a);
	// 傾斜(ox, oyは傾斜の中心, sx, syは傾斜角度)
	void AddSkew(float ox, float oy, float sx, float sy);
	// 拡大率(ox, oyは拡大の中心, sx, syは拡大率)
	void AddScale(float ox, float oy, float sx, float sy);

	// レンダーターゲットに反映させる
	void Bind();

	// 各DirectObjectの取得
	D2D1_MATRIX_3X2_F &GetMatrix() { return m_mat; }
};

//=============================================================
// Class : Bitmap
//-------------------------------------------------------------
// ビットマップデータを扱うオブジェクト。
//=============================================================
class Bitmap : public DrawObject {
	ID2D1Bitmap *m_pBitmap;
public:
	Bitmap(Drawer *pDrawer);
	Bitmap(Drawer *pDrawer, LPCWSTR filename);
	Bitmap(Bitmap &);
	~Bitmap();

	// 初期化
	bool Initialize(LPCWSTR filename);

	// 図形の描画
	void Draw();
	// 不透明度(0.0~1.0f)
	void Draw(float alpha);
	// 矩形切抜き(pixel)
	void Draw(float left, float right, float top, float bottom);
	// 不透明度、矩形切抜き
	void Draw(float alpha, float left, float right, float top, float bottom);

	// 各DirectObjectの取得
	ID2D1Bitmap *GetBitmap() { return m_pBitmap; }
};

//=============================================================
// Struct : グラデーション定義データ
//-------------------------------------------------------------
// グラデーションブラシを用意するために必要。
// 生成後は解放してよい。
//=============================================================
struct Gradient {
	struct GradPass {
		float r, g, b, a;	// カラー
		float pos;			// グラデーション位置(0.0f~1.0f)
	} *pPassArray;
	int nPass;
	int gamma;				// 色調保管モード(0か1)
	int extend;				// 拡張モード(0:エッジの引き伸ばし, 1:繰り返し, 2:鏡面)
};

//=============================================================
// Class : Brush
//-------------------------------------------------------------
// ブラシデータを扱うオブジェクト。
//=============================================================
class Brush : public DrawObject {
	ID2D1Brush *m_pBrush;
public:
	// 空のブラシ
	Brush(Drawer *pDrawer);
	// 単色ブラシ
	Brush(Drawer *pDrawer, float r, float g, float b, float a=1.0f);
	// 線形グラデーションブラシ
	Brush(Drawer *pDrawer, Gradient *pGradient, float sx, float sy, float ex, float ey);
	// 放射状グラデーションブラシ
	Brush(Drawer *pDrawer, Gradient *pGradient, float cx, float cy, float bx, float by, float rx, float ry);
	// ビットマップブラシ
	Brush(Drawer *pDrawer, Bitmap *pBitmap, int ex_x=0, int ex_y=0);
	Brush(Brush &);
	~Brush();

	// 初期化
	// 単色ブラシ
	bool Initialize(float r, float g, float b, float a=1.0f);
	// 線形グラデーションブラシ(sx, sy:始点, ex, ey:終点)
	bool Initialize(Gradient *pGradient, float sx, float sy, float ex, float ey);
	// 放射状グラデーションブラシ(cx, cy:原点, bx, by:グラデーション開始位置の原点からのずれ, rx, ry:半径)
	bool Initialize(Gradient *pGradient, float cx, float cy, float bx, float by, float rx, float ry);
	// ビットマップブラシ(ex_x, ex_y:拡張モード, 0:枠の引き伸ばし, 1:繰り返し, 2:鏡面)
	bool Initialize(Bitmap *pBitmap, int ex_x=0, int ex_y=0);

	// ブラシに対する変換の設定
	void SetTranslation(Translation *pTrans);
	// ブラシに対する不透明度の設定
	void SetOpacity(float alpha);

	// ラインの描画(sx, syは始点, ex, eyは終点)
	void DrawLine(float size, float sx, float sy, float ex, float ey);
	// 矩形の描画
	void DrawRect(float size, float left, float right, float top, float bottom);
	// 丸角矩形の描画(rx, ryは角円の半径)
	void DrawRoundedRect(float size, float left, float right, float top, float bottom, float rx, float ry);
	// 楕円の描画(ox, oyは楕円の中心, rx, ryは楕円の半径)
	void DrawEllipse(float size, float ox, float oy, float rx, float ry);
	// 矩形の塗りつぶし
	void FillRect(float left, float right, float top, float bottom);
	// 丸角矩形の塗りつぶし
	void FillRoundedRect(float left, float right, float top, float bottom, float rx, float ry);
	// 楕円の塗りつぶし
	void FillEllipse(float ox, float oy, float rx, float ry);
	// マスク塗りつぶし(textはマスク画像の種類 false=Graphics, true=Text)
	void FillMask(Bitmap *pMask, bool text);
	// マスク塗りつぶし(マスク矩形切抜き)
	void FillMask(Bitmap *pMask, bool text, float left, float right, float top, float bottom);

	// 各DirectObjectの取得
	ID2D1Brush *GetBrush() { return m_pBrush; }
};

//=============================================================
// enum : PassType
//-------------------------------------------------------------
// ジオメトリを記述するパスデータの種類。
//=============================================================
enum PassType {
	PT_START,		// 始点
	PT_END,			// 終点
	PT_LINE,			// 直線
	PT_BEZIER,			// 3次ベジェ
	PT_QUAD_BEZIER,	// 2次ベジェ
	PT_ARC				// 弧
};

//=============================================================
// Struct : Pass
//-------------------------------------------------------------
// ジオメトリを構成するパスのデータ。
//=============================================================
struct Pass {
	PassType type;
	union {
		// 始点
		struct {
			float x, y;		// 始点の位置
			bool filled;	// 新しい図形を塗りつぶすか
		} PassStart;
		// 終点
		struct {
			bool close;		// 図形を閉じるか
		} PassEnd;
		// 直線
		struct {
			float x, y;		// 次の点
		} PassLine;
		// 3次ベジェ曲線
		struct {
			float x, y;		// 次の点
			float c1x, c1y;	// 制御点1
			float c2x, c2y;	// 制御点2
		} PassBezier;
		// 2次ベジェ曲線
		struct {
			float x, y;		// 次の点
			float cx, cy;	// 制御点
		} PassQuadBezier;
		// 弧
		struct {
			float x, y;		// 次の点
			float rx, ry;	// 弧
			float a;		// 弧の角度
			bool clockwise;	// 弧の方向(false=反時計, true=時計)
			bool large;		// 弧の角度(false=180度以下, true=180度以上)
		} PassArc;
	} data;
};

//=============================================================
// Class : Geometry
//-------------------------------------------------------------
// ジオメトリデータを扱うオブジェクト。
//=============================================================
class Geometry : public DrawObject {
	ID2D1Geometry *m_pGeometry;
	ID2D1Brush *pBrush;
public:
	// 空のジオメトリ
	Geometry(Drawer *pDrawer);
	// パスジオメトリ
	Geometry(Drawer *pDrawer, Pass *pPasses, int nPass);
	// 矩形ジオメトリ
	Geometry(Drawer *pDrawer, float left, float right, float top, float bottom);
	// 楕円ジオメトリ
	Geometry(Drawer *pDrawer, float ox, float oy, float rx, float ry, void *pNoUse);
	// 丸角矩形ジオメトリ
	Geometry(Drawer *pDrawer, float left, float right, float top, float bottom, float rx, float ry);
	// ジオメトリグループ
	Geometry(Drawer *pDrawer, int fill, Geometry **ppGeometries, int nGeometries);
	// 変換されたジオメトリ
	Geometry(Drawer *pDrawer, Geometry *pGeometry, Translation *pTrans);
	Geometry(Geometry &);
	~Geometry();

	// 初期化
	// パスジオメトリ
	bool Initialize(Pass *pPasses, int nPass);
	// 矩形
	bool Initialize(float left, float right, float top, float bottom);
	// 楕円(最後の引数は利用されない)
	bool Initialize(float ox, float oy, float rx, float ry, void *pNoUse);
	// 丸角矩形
	bool Initialize(float left, float right, float top, float bottom, float rx, float ry);
	// ジオメトリグループ(fillは0だと交互, 1だと全域)
	bool Initialize(int fill, Geometry **ppGeometries, int nGeometries);
	// 変換されたジオメトリ
	bool Initialize(Geometry *pGeometry, Translation *pTrans);

	// 描画ブラシの設定
	void SetBrush(Brush *pBrush);

	// ジオメトリの描画
	void Draw();
	void Draw(float size);
	// ジオメトリの塗りつぶし
	void Fill();
	void Fill(Brush *pOpacityBrush);	// 透明度ブラシあり

	// 各DirectObjectの取得
	ID2D1Geometry *GetGeometry() { return m_pGeometry; }
};

//=============================================================
// Struct : Triangle
//-------------------------------------------------------------
// メッシュを記述する三角形データ。
//=============================================================
struct Triangle {
	struct _Point {
		float x, y;
	} Point[3];
};

//=============================================================
// Class : Mesh
//-------------------------------------------------------------
// メッシュデータを扱うオブジェクト。
//=============================================================
class Mesh : public DrawObject {
	ID2D1Mesh *m_pMesh;
	ID2D1Brush *pBrush;
public:
	Mesh(Drawer *pDrawer);
	Mesh(Drawer *pDrawer, Triangle *pTris, int nTri);
	Mesh(Drawer *pDrawer, Geometry *pGeometry, Translation *pTrans);
	Mesh(Mesh &);
	~Mesh();

	// 初期化
	bool Initialize();
	bool Initialize(Triangle *pTriangles, int nTriangles);
	bool Initialize(Geometry *pGeometry, Translation *pTrans);

	// 三角形の書き込み(Triangleの配列と、配列の個数)
	bool AddTriangles(Triangle *pTriangles, int nTriangles);

	// 描画ブラシの設定
	void SetBrush(Brush *pBrush);

	// メッシュの塗りつぶし
	void Fill();
};

//=============================================================
// Class : TextFormat
//-------------------------------------------------------------
// テキストフォーマットを扱うオブジェクト。
//=============================================================
class TextFormat : public DrawObject {
	IDWriteTextFormat *m_pFormat;
	ID2D1Brush *pBrush;
public:
	TextFormat(Drawer *pDrawer);
	TextFormat(Drawer *pDrawer, LPCWSTR font, int weight = 400, int style = 0, int stretch = 5, float size=10.0f, LPCWSTR locale = L"jp");
	TextFormat(TextFormat &);
	~TextFormat();

	// 初期化
	// weight (1 ~ 999, Normal は 400)
	// style (0:通常, 1:斜体, 2:イタリック体)
	// stretch (1~9, Normal は 5)
	bool Initialize(LPCWSTR font, int weight = 400, int style = 0, int stretch = 5, float size=10.0f, LPCWSTR locale = L"jp");

	// ブラシの設定
	void SetBrush(Brush *pBrush);

	// テキストの描画
	void Draw(wstring text);

	// 各DirectObjectの取得
	IDWriteTextFormat *GetTextFormat() { return m_pFormat; }
};

//=============================================================
// Class : TextLayout
//-------------------------------------------------------------
// レイアウトされたテキストを扱うオブジェクト。
//=============================================================
class TextLayout : public DrawObject {
	IDWriteTextLayout *m_pLayout;
	ID2D1Brush *pBrush;
public:
	TextLayout(Drawer *pDrawer);
	TextLayout(Drawer *pDrawer, wstring text, TextFormat *pFormat);
	TextLayout(TextLayout &);
	~TextLayout();

	// 初期化
	bool Initialize(wstring text, TextFormat *pFormat);

	// デフォルトブラシの設定
	void SetDefaultBrush(Brush *pBrush);

	// レイアウトの設定
	// ブラシ
	void SetBrush(Brush *pBrush, unsigned int start, unsigned int length);
	// フォント
	void SetFont(LPCWSTR fontname, unsigned int start, unsigned int length);
	// サイズ
	void SetSize(float size, unsigned int start, unsigned int length);
	// 引き伸ばし
	void SetStretch(int stretch, unsigned int start, unsigned int length);
	// スタイル
	void SetStyle(int style, unsigned int start, unsigned int length);
	// 太さ
	void SetWeight(int weight, unsigned int start, unsigned int length);
	// 取り消し線
	void SetStrikethrough(bool strinkethrough, unsigned int start, unsigned int length);
	// アンダーライン
	void SetUnderline(bool underline, unsigned int start, unsigned int length);

	// テキストレイアウトの描画
	void Draw();

	// 各DirectObjectの取得
	IDWriteTextLayout *GetTextLayout() { return m_pLayout; }
};

//=============================================================
// Class : Layer
//-------------------------------------------------------------
// レイヤーを扱うオブジェクト。
//=============================================================
class Layer : public DrawObject {
	ID2D1Layer *m_pLayer;
	D2D1_RECT_F bounds;
	ID2D1Geometry *pGeometry;
	D2D1_MATRIX_3X2_F matrix;
	float a;
	ID2D1Brush *pBrush;
public:
	Layer(Drawer *pDrawer);
	Layer(Layer &);
	~Layer();

	// 初期化
	bool Initialize();

	// レイヤーの設定
	void SetBounds(float left, float right, float top, float bottom);
	void SetGeometricMask(Geometry *pGeometry);
	void SetTranslation(Translation *pTrans);
	void SetOpacity(float alpha);
	void SetOpacityBrush(Brush *pBrush);

	// レイヤーの開始
	void Begin();

	// レイヤーの終了
	void End();
};
