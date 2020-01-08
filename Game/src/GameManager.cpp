#include "stdafx.h"
#include "GameManager.h"

void GameManager::GameStart() {
	m_game = std::make_unique<Game>(this);
	//TODO �G�t�F�N�g�Ƃ������Ȃ���
}
void GameManager::GameEnd(bool isDeathPlayer[PLAYER_NUM]) {	
	//���肪����ł���X�R�A���₷
	if (isDeathPlayer[1]) {
		m_score[0]++;
	}
	if (isDeathPlayer[0]) {
		m_score[1]++;
	}
	//TODO ���Ԑ؂�ƈ�������
	
	//���E���h�������߂�
	m_roundCount++;
	
	//�����I��
	if (m_maxRound <= m_roundCount || static_cast<int>(ceil(m_maxRound / 2.0f)) <= max(m_score[0], m_score[1])) {
		return;
	}
	
	//���̃��E���h���J�n
	GameStart();
}
