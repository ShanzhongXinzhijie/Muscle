#include "stdafx.h"
#include "GameManager.h"
#include "CResult.h"

void GameManager::GameStart() {
	m_game = std::make_unique<Game>(this);
	//TODO エフェクトとか消さないと
}
void GameManager::GameEnd(bool isDeathPlayer[PLAYER_NUM]) {	
	//相手が死んでたらスコア増やす
	if (isDeathPlayer[1]) {
		m_score[0]++;
	}
	if (isDeathPlayer[0]) {
		m_score[1]++;
	}
	//TODO 時間切れと引き分け
	
	//ラウンドをすすめる
	m_roundCount++;
	
	//試合終了
	if (m_maxRound <= m_roundCount || (m_maxRound - m_roundCount) < max(m_score[0], m_score[1]) - min(m_score[0], m_score[1])) {
		new CResult(m_score);
		delete this;
		return;
	}
	
	//次のラウンドを開始
	GameStart();
}
