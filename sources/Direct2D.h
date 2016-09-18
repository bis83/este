#pragma once

/*

Direct2D, DirectWrite�Ȃǂ�DirectX�܂��

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
// �`��V�X�e���N���X�B
//=============================================================
class Drawer {
	// �R�s�[�R���X�g���N�^�̋֎~
	Drawer(Drawer &d) {}

	// �t�@�N�g��
	ID2D1Factory *m_pFactory2D;
	IDWriteFactory *m_pFactoryWrite;
	IWICImagingFactory *m_pFactoryWIC;

	// �����_�[�^�[�Q�b�g
	ID2D1HwndRenderTarget *m_pRenderTarget;

public:
	Drawer();
	Drawer(HWND hWnd);
	~Drawer();

	// �������֐� HWND�����t�̃R���X�g���N�^�͕W���ł�����Ă�
	bool Initialize(HWND hWnd);

	// �����_�����O�̊J�n�ƏI��
	void BeginRender();
	void EndRender();

	// �����_�[�^�[�Q�b�g�̃N���A
	void Clear(float r, float g, float b);
	// �}�`�ϊ��̃��Z�b�g
	void ResetTranslation();

	// �eDirectObject�̎擾
	ID2D1Factory *GetD2D1Factory() { return m_pFactory2D; }
	IDWriteFactory *GetDWriteFactory() { return m_pFactoryWrite; }
	IWICImagingFactory *GetWICFactory() { return m_pFactoryWIC; } 
	ID2D1RenderTarget *GetRenderTarget() { return m_pRenderTarget; }
};

//=============================================================
// Class : DrawObject
//-------------------------------------------------------------
// DrawObject��Drawer���琶�������Object�̊���ƂȂ�N���X�B
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
// �}�`�ϊ��������I�u�W�F�N�g�B
// �}�`�ϊ��̏��͕ۑ�����܂��B
// �֐����Ă΂ꂽ���Ԃɕϊ����K�p�����̂ŁA
// �����ɒ��ӂ��Ă��������B
//=============================================================
class Translation : public DrawObject {
	D2D1_MATRIX_3X2_F m_mat;
public:
	Translation(Drawer *pDrawer);
	Translation(Translation &);
	~Translation();

	// ������Ԃɖ߂�
	void Reset();

	// �ʒu(px, py���_����̋���)
	void AddPos(float px, float py);
	// ��](ox, oy�͉�]�̒��S, a�͊p�x)
	void AddRot(float ox, float oy, float a);
	// �X��(ox, oy�͌X�΂̒��S, sx, sy�͌X�Ίp�x)
	void AddSkew(float ox, float oy, float sx, float sy);
	// �g�嗦(ox, oy�͊g��̒��S, sx, sy�͊g�嗦)
	void AddScale(float ox, float oy, float sx, float sy);

	// �����_�[�^�[�Q�b�g�ɔ��f������
	void Bind();

	// �eDirectObject�̎擾
	D2D1_MATRIX_3X2_F &GetMatrix() { return m_mat; }
};

//=============================================================
// Class : Bitmap
//-------------------------------------------------------------
// �r�b�g�}�b�v�f�[�^�������I�u�W�F�N�g�B
//=============================================================
class Bitmap : public DrawObject {
	ID2D1Bitmap *m_pBitmap;
public:
	Bitmap(Drawer *pDrawer);
	Bitmap(Drawer *pDrawer, LPCWSTR filename);
	Bitmap(Bitmap &);
	~Bitmap();

	// ������
	bool Initialize(LPCWSTR filename);

	// �}�`�̕`��
	void Draw();
	// �s�����x(0.0~1.0f)
	void Draw(float alpha);
	// ��`�ؔ���(pixel)
	void Draw(float left, float right, float top, float bottom);
	// �s�����x�A��`�ؔ���
	void Draw(float alpha, float left, float right, float top, float bottom);

	// �eDirectObject�̎擾
	ID2D1Bitmap *GetBitmap() { return m_pBitmap; }
};

//=============================================================
// Struct : �O���f�[�V������`�f�[�^
//-------------------------------------------------------------
// �O���f�[�V�����u���V��p�ӂ��邽�߂ɕK�v�B
// ������͉�����Ă悢�B
//=============================================================
struct Gradient {
	struct GradPass {
		float r, g, b, a;	// �J���[
		float pos;			// �O���f�[�V�����ʒu(0.0f~1.0f)
	} *pPassArray;
	int nPass;
	int gamma;				// �F���ۊǃ��[�h(0��1)
	int extend;				// �g�����[�h(0:�G�b�W�̈����L�΂�, 1:�J��Ԃ�, 2:����)
};

//=============================================================
// Class : Brush
//-------------------------------------------------------------
// �u���V�f�[�^�������I�u�W�F�N�g�B
//=============================================================
class Brush : public DrawObject {
	ID2D1Brush *m_pBrush;
public:
	// ��̃u���V
	Brush(Drawer *pDrawer);
	// �P�F�u���V
	Brush(Drawer *pDrawer, float r, float g, float b, float a=1.0f);
	// ���`�O���f�[�V�����u���V
	Brush(Drawer *pDrawer, Gradient *pGradient, float sx, float sy, float ex, float ey);
	// ���ˏ�O���f�[�V�����u���V
	Brush(Drawer *pDrawer, Gradient *pGradient, float cx, float cy, float bx, float by, float rx, float ry);
	// �r�b�g�}�b�v�u���V
	Brush(Drawer *pDrawer, Bitmap *pBitmap, int ex_x=0, int ex_y=0);
	Brush(Brush &);
	~Brush();

	// ������
	// �P�F�u���V
	bool Initialize(float r, float g, float b, float a=1.0f);
	// ���`�O���f�[�V�����u���V(sx, sy:�n�_, ex, ey:�I�_)
	bool Initialize(Gradient *pGradient, float sx, float sy, float ex, float ey);
	// ���ˏ�O���f�[�V�����u���V(cx, cy:���_, bx, by:�O���f�[�V�����J�n�ʒu�̌��_����̂���, rx, ry:���a)
	bool Initialize(Gradient *pGradient, float cx, float cy, float bx, float by, float rx, float ry);
	// �r�b�g�}�b�v�u���V(ex_x, ex_y:�g�����[�h, 0:�g�̈����L�΂�, 1:�J��Ԃ�, 2:����)
	bool Initialize(Bitmap *pBitmap, int ex_x=0, int ex_y=0);

	// �u���V�ɑ΂���ϊ��̐ݒ�
	void SetTranslation(Translation *pTrans);
	// �u���V�ɑ΂���s�����x�̐ݒ�
	void SetOpacity(float alpha);

	// ���C���̕`��(sx, sy�͎n�_, ex, ey�͏I�_)
	void DrawLine(float size, float sx, float sy, float ex, float ey);
	// ��`�̕`��
	void DrawRect(float size, float left, float right, float top, float bottom);
	// �ۊp��`�̕`��(rx, ry�͊p�~�̔��a)
	void DrawRoundedRect(float size, float left, float right, float top, float bottom, float rx, float ry);
	// �ȉ~�̕`��(ox, oy�͑ȉ~�̒��S, rx, ry�͑ȉ~�̔��a)
	void DrawEllipse(float size, float ox, float oy, float rx, float ry);
	// ��`�̓h��Ԃ�
	void FillRect(float left, float right, float top, float bottom);
	// �ۊp��`�̓h��Ԃ�
	void FillRoundedRect(float left, float right, float top, float bottom, float rx, float ry);
	// �ȉ~�̓h��Ԃ�
	void FillEllipse(float ox, float oy, float rx, float ry);
	// �}�X�N�h��Ԃ�(text�̓}�X�N�摜�̎�� false=Graphics, true=Text)
	void FillMask(Bitmap *pMask, bool text);
	// �}�X�N�h��Ԃ�(�}�X�N��`�ؔ���)
	void FillMask(Bitmap *pMask, bool text, float left, float right, float top, float bottom);

	// �eDirectObject�̎擾
	ID2D1Brush *GetBrush() { return m_pBrush; }
};

//=============================================================
// enum : PassType
//-------------------------------------------------------------
// �W�I���g�����L�q����p�X�f�[�^�̎�ށB
//=============================================================
enum PassType {
	PT_START,		// �n�_
	PT_END,			// �I�_
	PT_LINE,			// ����
	PT_BEZIER,			// 3���x�W�F
	PT_QUAD_BEZIER,	// 2���x�W�F
	PT_ARC				// ��
};

//=============================================================
// Struct : Pass
//-------------------------------------------------------------
// �W�I���g�����\������p�X�̃f�[�^�B
//=============================================================
struct Pass {
	PassType type;
	union {
		// �n�_
		struct {
			float x, y;		// �n�_�̈ʒu
			bool filled;	// �V�����}�`��h��Ԃ���
		} PassStart;
		// �I�_
		struct {
			bool close;		// �}�`����邩
		} PassEnd;
		// ����
		struct {
			float x, y;		// ���̓_
		} PassLine;
		// 3���x�W�F�Ȑ�
		struct {
			float x, y;		// ���̓_
			float c1x, c1y;	// ����_1
			float c2x, c2y;	// ����_2
		} PassBezier;
		// 2���x�W�F�Ȑ�
		struct {
			float x, y;		// ���̓_
			float cx, cy;	// ����_
		} PassQuadBezier;
		// ��
		struct {
			float x, y;		// ���̓_
			float rx, ry;	// ��
			float a;		// �ʂ̊p�x
			bool clockwise;	// �ʂ̕���(false=�����v, true=���v)
			bool large;		// �ʂ̊p�x(false=180�x�ȉ�, true=180�x�ȏ�)
		} PassArc;
	} data;
};

//=============================================================
// Class : Geometry
//-------------------------------------------------------------
// �W�I���g���f�[�^�������I�u�W�F�N�g�B
//=============================================================
class Geometry : public DrawObject {
	ID2D1Geometry *m_pGeometry;
	ID2D1Brush *pBrush;
public:
	// ��̃W�I���g��
	Geometry(Drawer *pDrawer);
	// �p�X�W�I���g��
	Geometry(Drawer *pDrawer, Pass *pPasses, int nPass);
	// ��`�W�I���g��
	Geometry(Drawer *pDrawer, float left, float right, float top, float bottom);
	// �ȉ~�W�I���g��
	Geometry(Drawer *pDrawer, float ox, float oy, float rx, float ry, void *pNoUse);
	// �ۊp��`�W�I���g��
	Geometry(Drawer *pDrawer, float left, float right, float top, float bottom, float rx, float ry);
	// �W�I���g���O���[�v
	Geometry(Drawer *pDrawer, int fill, Geometry **ppGeometries, int nGeometries);
	// �ϊ����ꂽ�W�I���g��
	Geometry(Drawer *pDrawer, Geometry *pGeometry, Translation *pTrans);
	Geometry(Geometry &);
	~Geometry();

	// ������
	// �p�X�W�I���g��
	bool Initialize(Pass *pPasses, int nPass);
	// ��`
	bool Initialize(float left, float right, float top, float bottom);
	// �ȉ~(�Ō�̈����͗��p����Ȃ�)
	bool Initialize(float ox, float oy, float rx, float ry, void *pNoUse);
	// �ۊp��`
	bool Initialize(float left, float right, float top, float bottom, float rx, float ry);
	// �W�I���g���O���[�v(fill��0���ƌ���, 1���ƑS��)
	bool Initialize(int fill, Geometry **ppGeometries, int nGeometries);
	// �ϊ����ꂽ�W�I���g��
	bool Initialize(Geometry *pGeometry, Translation *pTrans);

	// �`��u���V�̐ݒ�
	void SetBrush(Brush *pBrush);

	// �W�I���g���̕`��
	void Draw();
	void Draw(float size);
	// �W�I���g���̓h��Ԃ�
	void Fill();
	void Fill(Brush *pOpacityBrush);	// �����x�u���V����

	// �eDirectObject�̎擾
	ID2D1Geometry *GetGeometry() { return m_pGeometry; }
};

//=============================================================
// Struct : Triangle
//-------------------------------------------------------------
// ���b�V�����L�q����O�p�`�f�[�^�B
//=============================================================
struct Triangle {
	struct _Point {
		float x, y;
	} Point[3];
};

//=============================================================
// Class : Mesh
//-------------------------------------------------------------
// ���b�V���f�[�^�������I�u�W�F�N�g�B
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

	// ������
	bool Initialize();
	bool Initialize(Triangle *pTriangles, int nTriangles);
	bool Initialize(Geometry *pGeometry, Translation *pTrans);

	// �O�p�`�̏�������(Triangle�̔z��ƁA�z��̌�)
	bool AddTriangles(Triangle *pTriangles, int nTriangles);

	// �`��u���V�̐ݒ�
	void SetBrush(Brush *pBrush);

	// ���b�V���̓h��Ԃ�
	void Fill();
};

//=============================================================
// Class : TextFormat
//-------------------------------------------------------------
// �e�L�X�g�t�H�[�}�b�g�������I�u�W�F�N�g�B
//=============================================================
class TextFormat : public DrawObject {
	IDWriteTextFormat *m_pFormat;
	ID2D1Brush *pBrush;
public:
	TextFormat(Drawer *pDrawer);
	TextFormat(Drawer *pDrawer, LPCWSTR font, int weight = 400, int style = 0, int stretch = 5, float size=10.0f, LPCWSTR locale = L"jp");
	TextFormat(TextFormat &);
	~TextFormat();

	// ������
	// weight (1 ~ 999, Normal �� 400)
	// style (0:�ʏ�, 1:�Α�, 2:�C�^���b�N��)
	// stretch (1~9, Normal �� 5)
	bool Initialize(LPCWSTR font, int weight = 400, int style = 0, int stretch = 5, float size=10.0f, LPCWSTR locale = L"jp");

	// �u���V�̐ݒ�
	void SetBrush(Brush *pBrush);

	// �e�L�X�g�̕`��
	void Draw(wstring text);

	// �eDirectObject�̎擾
	IDWriteTextFormat *GetTextFormat() { return m_pFormat; }
};

//=============================================================
// Class : TextLayout
//-------------------------------------------------------------
// ���C�A�E�g���ꂽ�e�L�X�g�������I�u�W�F�N�g�B
//=============================================================
class TextLayout : public DrawObject {
	IDWriteTextLayout *m_pLayout;
	ID2D1Brush *pBrush;
public:
	TextLayout(Drawer *pDrawer);
	TextLayout(Drawer *pDrawer, wstring text, TextFormat *pFormat);
	TextLayout(TextLayout &);
	~TextLayout();

	// ������
	bool Initialize(wstring text, TextFormat *pFormat);

	// �f�t�H���g�u���V�̐ݒ�
	void SetDefaultBrush(Brush *pBrush);

	// ���C�A�E�g�̐ݒ�
	// �u���V
	void SetBrush(Brush *pBrush, unsigned int start, unsigned int length);
	// �t�H���g
	void SetFont(LPCWSTR fontname, unsigned int start, unsigned int length);
	// �T�C�Y
	void SetSize(float size, unsigned int start, unsigned int length);
	// �����L�΂�
	void SetStretch(int stretch, unsigned int start, unsigned int length);
	// �X�^�C��
	void SetStyle(int style, unsigned int start, unsigned int length);
	// ����
	void SetWeight(int weight, unsigned int start, unsigned int length);
	// ��������
	void SetStrikethrough(bool strinkethrough, unsigned int start, unsigned int length);
	// �A���_�[���C��
	void SetUnderline(bool underline, unsigned int start, unsigned int length);

	// �e�L�X�g���C�A�E�g�̕`��
	void Draw();

	// �eDirectObject�̎擾
	IDWriteTextLayout *GetTextLayout() { return m_pLayout; }
};

//=============================================================
// Class : Layer
//-------------------------------------------------------------
// ���C���[�������I�u�W�F�N�g�B
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

	// ������
	bool Initialize();

	// ���C���[�̐ݒ�
	void SetBounds(float left, float right, float top, float bottom);
	void SetGeometricMask(Geometry *pGeometry);
	void SetTranslation(Translation *pTrans);
	void SetOpacity(float alpha);
	void SetOpacityBrush(Brush *pBrush);

	// ���C���[�̊J�n
	void Begin();

	// ���C���[�̏I��
	void End();
};
