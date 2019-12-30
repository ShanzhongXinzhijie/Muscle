#include "stdafx.h"
#include "CTitle.h"

bool CTitle::Start() {
	m_sprite.Init(L"Resource/spriteData/logo.png");
	return true;
}

void CTitle::PostLoopUpdate() {
	//îwåi
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void CTitle::PostRender() {
	m_sprite.Draw({ 0.0f,0.0f }, CVector2::One(), { 0.0f,0.0f });
	m_font.Draw(L"ëŒêÌäJén", { 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
}