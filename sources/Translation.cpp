#include "Direct2D.h"
#include "Game.h"

Translation::Translation(Drawer *pDrawer) : DrawObject(pDrawer), m_mat(D2D1::IdentityMatrix()) {
}

Translation::Translation(Translation &t) : DrawObject(t.pDrawer), m_mat(t.m_mat) {
}

Translation::~Translation() {
}

void Translation::Reset() {
	m_mat = D2D1::IdentityMatrix();
}

void Translation::AddPos(float px, float py) {
	m_mat = m_mat * D2D1::Matrix3x2F::Translation(px, py);
}

void Translation::AddRot(float ox, float oy, float a) {
	m_mat = m_mat * D2D1::Matrix3x2F::Rotation(a, D2D1::Point2F(ox, oy));
}

void Translation::AddSkew(float ox, float oy, float sx, float sy) {
	m_mat = m_mat * D2D1::Matrix3x2F::Skew(sx, sy, D2D1::Point2F(ox, oy));
}

void Translation::AddScale(float ox, float oy, float sx, float sy) {
	m_mat = m_mat * D2D1::Matrix3x2F::Scale(sx, sy, D2D1::Point2F(ox, oy));
}

void Translation::Bind() {
	pDrawer->GetRenderTarget()->SetTransform(m_mat);
}
