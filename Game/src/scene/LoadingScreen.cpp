#include "stdafx.h"
#include "LoadingScreen.h"

bool LoadingScreen::Start()
{
	//ロゴ読み込み
	m_sprite.Init(L"Resource/spriteData/logo.png");

	//SE
	SuicideObj::CSE* se = NewGO<SuicideObj::CSE>(L"Resource/sound/Randomize.wav");
	se->Play();

	return true;
}
void LoadingScreen::PostLoopUpdate()
{
	//ロード画面背景
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}
void LoadingScreen::PostRender()
{
	if (m_isOneLooped) {
		//このクラス無効化
		SetEnable(false);
		//ロード
		Load();
	}
	m_isOneLooped = true;

	//ロゴ
	m_sprite.Draw({ 0.5f,0.5f }, CVector2::One(), { 0.5f,0.5f });

	//ロード画面
	m_font.Draw(L"プリプリプリプリ...", { 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
}