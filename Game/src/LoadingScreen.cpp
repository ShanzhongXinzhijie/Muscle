#include "stdafx.h"
#include "LoadingScreen.h"
#include "GameManager.h"

bool LoadingScreen::Start() {
	m_gomiCamera = std::make_unique<GameObj::NoRegisterOrthoCamera>();
	SetMainCamera(m_gomiCamera.get());
	GetCameraList().push_back(m_gomiCamera.get());
	GetCameraList().push_back(m_gomiCamera.get());
	m_sprite.Init(L"Resource/spriteData/logo.png");
	return true;
}

void LoadingScreen::PostLoopUpdate() {
	//ロード画面背景
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void LoadingScreen::PostRender() {
	if (m_isOneLooped) {
		//ゴミカメラ削除
		m_gomiCamera.reset();
		//このクラス無効化
		SetEnable(false);
		//ロードするぞ
		new GameManager;
	}
	m_isOneLooped = true;

	//ロード画面
	m_sprite.Draw({ 0.5f,0.5f }, CVector2::One(), { 0.5f,0.5f });
	m_font.Draw(L"プリプリプリプリ...",{ 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
}