#include "stdafx.h"
#include "GameManager.h"
#include "CResult.h"

namespace {
	constexpr int BGM_NUM = 1;
	constexpr wchar_t BGM_PATH[BGM_NUM][64] = { 
		L"Resource/sound/bgm/tenkuu-toshi.wav",
		//L"Resource/sound/bgm/kami-no-kourin.wav",
		//L"Resource/sound/bgm/kami-no-kourin.wav", 
		//L"Resource/sound/bgm/kami-no-kourin.wav" 
	};
}

void GameManager::GameStart() {
	m_game.reset();
	m_game = std::make_unique<Game>(this);
	
	if (!GetIsPracticeRound()) {
		//BGM
		if (m_bgmNumber < 0) {
			m_bgmNumber = CMath::RandomInt() % BGM_NUM;
		}
		FindGO<BGMPlayer>(L"BGMPlayer")->Play(BGM_PATH[m_bgmNumber]);
	}
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
			//SE
			SuicideObj::CSE* se = NewGO<SuicideObj::CSE>(L"Resource/sound/Randomize.wav");
			se->Play();

			//BGM
			FindGO<BGMPlayer>(L"BGMPlayer")->Play(L"Resource/sound/bgm/the-night-of-blood-syokei-no-theme.wav");

			new CResult(m_score);
			delete this;

			return;
		}
	}

	//���̃��E���h���J�n
	GameStart();
}
void GameManager::NoContest() {
	//SE
	SuicideObj::CSE* se = NewGO<SuicideObj::CSE>(L"Resource/sound/Randomize.wav");
	se->Play();

	//BGM
	FindGO<BGMPlayer>(L"BGMPlayer")->Play(L"Resource/sound/bgm/the-night-of-blood-syokei-no-theme.wav");

	new CResult(m_score, true);
	delete this;

	return;
}
