#include "stdafx.h"
#include "GameManager.h"
#include "CResult.h"

void GameManager::GameStart() {
	m_game.reset();
	m_game = std::make_unique<Game>(this);
}
void GameManager::GameEnd(bool isDeathPlayer[PLAYER_NUM]) {	

	if (m_isPracticeRound) {
		//���K���E���h�I��
		m_isPracticeRound = false;
		m_timeLimitSec = TIME_LIMIT;
	}
	else {
		//���肪����ł���X�R�A���₷
		if (isDeathPlayer[1]) {
			m_score[0]++;
		}
		if (isDeathPlayer[0]) {
			m_score[1]++;
		}

		//���E���h�������߂�
		m_roundCount++;

		//�����I��
		if (m_maxRound <= m_roundCount || (m_maxRound - m_roundCount) < max(m_score[0], m_score[1]) - min(m_score[0], m_score[1])) {
			new CResult(m_score);
			delete this;
			return;
		}
	}

	//���̃��E���h���J�n
	GameStart();
}
void GameManager::NoContest() {
	new CResult(m_score, true);
	delete this;
	return;
}
