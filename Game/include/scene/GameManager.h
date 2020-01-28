#pragma once
#include "Game.h"

class GameManager
{
public:
	GameManager() {
		GameStart();
	}

	/// <summary>
	/// �ő僉�E���h����ݒ�
	/// </summary>
	void SetMaxRound(int maxRound) {
		m_maxRound = maxRound;
	}

	/// <summary>
	/// ���E���h�̏I��
	/// </summary>
	/// <param name="isDeathPlayer">�v���C���[������ł��邩</param>
	void GameEnd(bool isDeathPlayer[PLAYER_NUM]);

	/// <summary>
	/// �������Ԑݒ���擾
	/// </summary>
	int GetTimeLimitSec()const {
		return m_timeLimitSec;
	}

	/// <summary>
	/// ���݂̃��E���h�����擾
	/// </summary>
	int GetRoundCount()const {
		return m_roundCount;
	}
	/// <summary>
	/// �ő僉�E���h�����擾
	/// </summary>
	int GetMaxRound()const {
		return m_maxRound;
	}

	/// <summary>
	/// �v���C���[�̃X�R�A���擾
	/// </summary>
	int GetPlayerScore(int playernum)const {
		return m_score[playernum];
	}

private:
	void GameStart();

private:
	std::unique_ptr<Game> m_game;

	int m_roundCount = 0;//���E���h��
	int m_score[PLAYER_NUM] = {};//�X�R�A

	int m_maxRound = 5;//�ő僉�E���h
	int m_timeLimitSec = 30;//��������(�b)
};

