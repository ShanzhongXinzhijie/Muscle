#include "stdafx.h"
#include "Game.h"
#include "GameManager.h"

Game::Game(GameManager* manager) : m_manager(manager), m_timeLimitSec(static_cast<float>(manager->GetTimeLimitSec()) + 0.5f) {
	int scores[PLAYER_NUM] = { m_manager->GetPlayerScore(0),m_manager->GetPlayerScore(1) };
	new CountDown(m_manager->GetRoundCount(), m_manager->GetMaxRound(), scores, m_manager->GetTimeLimitSec());
	m_player[0] = std::make_unique<CPlayer>(1);
	m_player[1] = std::make_unique<CPlayer>(0);
	m_shibuya = std::make_unique<Shibuya>();
}

void Game::PreUpdate() {
	//�v���C���[�̎��S�����m
	bool isDeath = false;
	bool isDeathPlayer[PLAYER_NUM] = {};
	for (int i = 0; i < PLAYER_NUM;i++) {
		if (m_player[i]->GetIsDeath()) {
			isDeath = true;
			isDeathPlayer[m_player[i]->GetPlayerNum()] = true;
		}
	}
	//�v���C���[���S�ŃQ�[���I��
	if (isDeath) {
		m_manager->GameEnd(isDeathPlayer);
		return;
	}

	//��������
	m_timeLimitSec -= FRAME_PER_SECOND;
	if (m_timeLimitSec < 0.0f) {
		m_manager->GameEnd(isDeathPlayer);
		return;
	}
}

void Game::PostRender() {
	//��������
	m_timeLimitSec = max(0.0f, m_timeLimitSec);
	m_font.DrawFormat(L"%d:%d", { 0.5f,0.0f }, {0.5f,0.0f}, static_cast<int>(m_timeLimitSec / 60.0f), static_cast<int>(m_timeLimitSec)%60);
}
