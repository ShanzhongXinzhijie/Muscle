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
	//背景
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void CResult::PostRender() {
	//タイトル
	m_font.Draw(L"けっか", { 0.5f,0.0f }, CVector4::Black(), CVector2::One(), {0.5f,0.0f});

	//スコア
	wchar_t string[64];
	swprintf_s(string, L"%d - %d", m_score[0], m_score[1]);
	m_fontScore.Draw(string, { 0.5f,0.25f }, CVector4::Black(), CVector2::One(), { 0.57f,0.6f });

	//リザルト
	if (m_score[0] == m_score[1]) {
		m_font.Draw(L"ひきわけ\nわたしのかち", { 0.5f,0.6f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
	}
	else if (m_score[0] > m_score[1]) {
		m_font.Draw(L"ひだりのかち\nみぎはカス", { 0.5f,0.6f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
	}
	else {
		m_font.Draw(L"みぎのかち\nひだりはしね", { 0.5f,0.6f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
	}

	//次の操作
	swprintf_s(string, L"Aれんだでしゅうりょう (%d/%d)", m_pushCnt, MAX_PUSH);
	m_font.Draw(string, { 0.5f,0.9f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
}