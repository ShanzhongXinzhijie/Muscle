#include "stdafx.h"
#include "CResult.h"
#include "CTitle.h"
#include "GameManager.h"
#include "LoadingScreen.h"
#include "AssembleScene.h"

void CResult::Update() {
	for (int i = 0; i < PLAYER_NUM; i++) {
		if (Pad(i).GetDown(enButtonA)) {
			m_pushCnt2++;
		}
		if (Pad(i).GetDown(enButtonB)) {
			m_pushCnt++;
		}
	}
	if (m_pushCnt >= MAX_PUSH) {
		new LoadingScreen([]() {new CTitle; });
		delete this;
		return;
	}
	if (m_pushCnt2 >= MAX_PUSH) {
		new LoadingScreen([]() {new AssembleScene; });
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
	if (m_isNoContest) {
		swprintf_s(string, L"�m�[�R���e�X�g");
	}
	else if (m_score[0] == m_score[1]) {
		if (m_playerCnt == 2) {
			swprintf_s(string, L"�Ђ��킯"); 
		}
		else {
			swprintf_s(string, L"�Ђ��킯\n���C�̂���");
		}
	}
	else if (m_score[0] > m_score[1]) {
		if (m_playerCnt == 2) {
			swprintf_s(string, L"�Ђ���̂���\n�݂��̓J�X");
		}
		else {
			swprintf_s(string, L"���܂��̂���\n���C�̓J�X");
		}
	}
	else {
		if (m_playerCnt == 2) {
			swprintf_s(string, L"�݂��̂���\n�Ђ���̓J�X");
		}
		else {
			swprintf_s(string, L"���C�̂���\n���܂��̓J�X");
		}
	}
	m_font.Draw(string, { 0.5f,0.6f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });

	//���̑���
	swprintf_s(string, L"B��񂾂ł��イ��傤 (%d/%d)", m_pushCnt, MAX_PUSH);
	m_font.Draw(string, { 0.5f,0.84f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });

	swprintf_s(string, L"A��񂾂ł��������� (%d/%d)", m_pushCnt2, MAX_PUSH);
	m_font.Draw(string, { 0.5f,0.9f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
}