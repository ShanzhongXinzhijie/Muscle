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
	//���[�h��ʔw�i
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void LoadingScreen::PostRender() {
	if (m_isOneLooped) {
		//�S�~�J�����폜
		m_gomiCamera.reset();
		//���̃N���X������
		SetEnable(false);
		//���[�h���邼
		new GameManager;
	}
	m_isOneLooped = true;

	//���[�h���
	m_sprite.Draw({ 0.5f,0.5f }, CVector2::One(), { 0.5f,0.5f });
	m_font.Draw(L"�v���v���v���v��...",{ 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
}