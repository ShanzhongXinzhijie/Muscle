#include "stdafx.h"
#include "LoadingScreen.h"
#include "GameManager.h"
#include "AssembleScene.h"

bool LoadingScreen::Start() {
	//���Ӗ��J�����ݒ�
	m_gomiCamera = std::make_unique<GameObj::NoRegisterOrthoCamera>();
	SetMainCamera(m_gomiCamera.get());
	SetCameraToList(0, m_gomiCamera.get());
	SetCameraToList(1, m_gomiCamera.get());

	//���S�ǂݍ���
	m_sprite.Init(L"Resource/spriteData/logo.png");

	return true;
}

void LoadingScreen::Update() {
	//PRESS ANY BUTTON
	for (int i = 0; i < PLAYER_NUM; i++) {
		if (Pad(i).GetAnyDown()) {
			m_enter = true;
			break;
		}
	}
}

void LoadingScreen::PostLoopUpdate() {
	//���[�h��ʔw�i
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void LoadingScreen::PostRender() 
{
	if (m_enter) {
		if (m_isOneLooped) {
			//�S�~�J�����폜
			m_gomiCamera.reset();
			//���̃N���X������
			SetEnable(false);
			//���[�h���邼
			new AssembleScene;// GameManager;
		}
		m_isOneLooped = true;
	}

	//���S
	m_sprite.Draw({ 0.5f,0.5f }, CVector2::One(), { 0.5f,0.5f });
	
	if (m_enter) {
		//���[�h���
		m_font.Draw(L"�v���v���v���v��...", { 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
	}
	else {
		m_font.Draw(L"������f�X�z�g�P", { 0.5f,0.0f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
		m_font.Draw(L"PRESS ANY BUTTON ->THE ORDER", { 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
	}
}