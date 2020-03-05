#include "stdafx.h"
#include "CTitle.h"
#include "WindowSizeManager.h"
#include "LoadingScreen.h"
#include "AssembleScene.h"
#include "CGameMode.h"

bool CTitle::Start() {
	//��ʃT�C�Y�ύX
	FindGO<WindowSizeManager>(L"WindowSizeManager")->ChangeWindowSize(true);

	m_sprite.Init(L"Resource/spriteData/logo.png");
	return true;
}

void CTitle::Update() {
	for (int i = 0; i < PLAYER_NUM; i++) {
		//�I��
		if (Pad(i).GetDown(enButtonB)) {
			m_pushCnt++;
		}

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
				//�v���C���[���ύX
				if (m_selectMode == 0) {
					FindGO<CGameMode>(L"CGameMode")->SetPlayerNum(2);
				}
				else {
					FindGO<CGameMode>(L"CGameMode")->SetPlayerNum(1);
				}
				//�J��
				new LoadingScreen([]() {new AssembleScene(); });
				//���E
				delete this;
				return;
			}
		}
	}

	//�I��
	if (m_pushCnt >= MAX_PUSH) {
		BreakGameLoop();
		delete this;
		return;
	}
}

void CTitle::PostLoopUpdate() {
	//�w�i
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void CTitle::PostRender() {
	//���S
	m_sprite.Draw({ 0.5f,0.5f }, CVector2::One(), { 0.5f,0.5f });

	//�o�[�W����
	//m_font.Draw(L"Ver.", { 0.0f,0.0f }, CVector4::Black(), CVector2::One(), { 0.0f,0.0f });

	wchar_t string[64];
	swprintf_s(string, L"B��񂾂ŃI���� (%d/%d)", m_pushCnt, MAX_PUSH);
	m_font.Draw(string, { 0.5f,0.0f }, CVector4::Black(), CVector2::One()*0.5f, { 0.5f,0.0f });

	if(!m_enter) {
		m_font.Draw(L"������f�X�z�g�P", { 0.5f,0.09f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
		m_font.Draw(L"PRESS ANY BUTTON ->ECLIPSE", { 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });		
		return;
	}

	//�^�C�g��
	m_font.Draw(L"���[�h�����񂽂�", { 0.5f,0.09f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });

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