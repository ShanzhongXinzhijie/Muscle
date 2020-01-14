#include "stdafx.h"
#include "CResult.h"
#include "GameManager.h"

void CResult::Update() {
	for (int i = 0; i < PLAYER_NUM; i++) {
		if (Pad(i).GetDown(enButtonA)) {
			m_pushCnt++;
		}
	}
	if (m_pushCnt >= MAX_PUSH) {
		new GameManager;
		delete this;
		return;
	}
}

void CResult::PostLoopUpdate() {
	//�w�i
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void CResult::PostRender() {
	//�^�C�g��
	m_font.Draw(L"������", { 0.5f,0.0f }, CVector4::Black(), CVector2::One(), {0.5f,0.0f});

	//�X�R�A
	wchar_t string[64];
	swprintf_s(string, L"%d - %d", m_score[0], m_score[1]);
	m_fontScore.Draw(string, { 0.5f,0.25f }, CVector4::Black(), CVector2::One(), { 0.57f,0.6f });

	//���U���g
	if (m_score[0] == m_score[1]) {
		m_font.Draw(L"�Ђ��킯\n�킽���̂���", { 0.5f,0.6f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
	}
	else if (m_score[0] > m_score[1]) {
		m_font.Draw(L"�Ђ���̂���\n�݂��̓J�X", { 0.5f,0.6f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
	}
	else {
		m_font.Draw(L"�݂��̂���\n�Ђ���͂���", { 0.5f,0.6f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
	}

	//���̑���
	swprintf_s(string, L"A��񂾂ł��イ��傤 (%d/%d)", m_pushCnt, MAX_PUSH);
	m_font.Draw(string, { 0.5f,0.9f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
}