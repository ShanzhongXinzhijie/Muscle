#include "stdafx.h"
#include "LoadingScreen.h"

bool LoadingScreen::Start()
{
	//���S�ǂݍ���
	m_sprite.Init(L"Resource/spriteData/logo.png");

	//SE
	SuicideObj::CSE* se = NewGO<SuicideObj::CSE>(L"Resource/sound/Randomize.wav");
	se->Play();

	return true;
}
void LoadingScreen::PostLoopUpdate()
{
	//���[�h��ʔw�i
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}
void LoadingScreen::PostRender()
{
	if (m_isOneLooped) {
		//���̃N���X������
		SetEnable(false);
		//���[�h
		Load();
	}
	m_isOneLooped = true;

	//���S
	m_sprite.Draw({ 0.5f,0.5f }, CVector2::One(), { 0.5f,0.5f });

	//���[�h���
	m_font.Draw(L"�v���v���v���v��...", { 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
}