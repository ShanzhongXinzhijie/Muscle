#include "stdafx.h"
#include "CTitle.h"
#include "WindowSizeManager.h"
#include "LoadingScreen.h"
#include "AssembleScene.h"

bool CTitle::Start() {
	//��ʃT�C�Y�ύX
	FindGO<WindowSizeManager>(L"WindowSizeManager")->ChangeWindowSize(true);

	m_sprite.Init(L"Resource/spriteData/logo.png");
	return true;
}

void CTitle::Update() {
	for (int i = 0; i < PLAYER_NUM; i++) {
		if (!m_enter) {
			//�^�C�g��
			//PRESS ANY BUTTON
			if (Pad(i).GetAnyDown()) {
				m_enter = true;
				break;
			}
		}
		else {
			//���[�h�I��
			if (Pad(i).GetDown(enButtonLeft) || Pad(i).GetDown(enButtonLSLeft)) {
				m_selectMode--; if (m_selectMode < 0) { m_selectMode = 0; }
			}
			if (Pad(i).GetDown(enButtonRight) || Pad(i).GetDown(enButtonLSRight)) {
				m_selectMode++; if (m_selectMode > 1) { m_selectMode = 1; }
			}
			//����
			if (Pad(i).GetDown(enButtonA) || Pad(i).GetDown(enButtonStart)) {
				//��ʃT�C�Y�ύX
				FindGO<WindowSizeManager>(L"WindowSizeManager")->ChangeWindowSize(m_selectMode == 0);
				//�J��
				if (m_selectMode == 0) {
					new LoadingScreen([]() {new AssembleScene(2); });
				}
				else {
					new LoadingScreen([]() {new AssembleScene(1); });
				}
				delete this;
				return;
			}
		}
	}
}

void CTitle::PostLoopUpdate() {
	//�w�i
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void CTitle::PostRender() {
	//���S
	m_sprite.Draw({ 0.5f,0.5f }, CVector2::One(), { 0.5f,0.5f });

	if(!m_enter) {
		m_font.Draw(L"������f�X�z�g�P", { 0.5f,0.0f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
		m_font.Draw(L"PRESS ANY BUTTON ->THE ORDER", { 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
		return;
	}

	//�^�C�g��
	m_font.Draw(L"���[�h�����񂽂�", { 0.5f,0.0f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });

	//���[�h�Z���N�g
	for (int i = 0; i < 2; i++) {
		CVector4 color = CVector4::Black();
		if (i == m_selectMode) {
			color = CVector4::Red();
		}
		if (i == 0) {
			m_font.Draw(L"�Ђ��� VS �݂�", { 0.25f,0.85f }, color, CVector2::One(), { 0.0f,0.5f });
		}
		else {
			m_font.Draw(L"���܂� VS ���J", { 0.5f,0.85f }, color, CVector2::One(), { 0.0f,0.5f });
		}
	}

	//���[�h����
	if (0 == m_selectMode) {
		m_font.Draw(L"�Ƃ������Ƃ��������܂�", { 0.5f,0.95f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
	}
	else {
		m_font.Draw(L"�������̂Ƃ������Ƃ��������܂�", { 0.5f,0.95f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
	}
}