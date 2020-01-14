#include "stdafx.h"
#include "LoadingScreen.h"
#include "GameManager.h"

bool LoadingScreen::Start() {
	//無意味カメラ設定
	m_gomiCamera = std::make_unique<GameObj::NoRegisterOrthoCamera>();
	SetMainCamera(m_gomiCamera.get());
	SetCameraToList(0, m_gomiCamera.get());
	SetCameraToList(1, m_gomiCamera.get());

	//ロゴ読み込み
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