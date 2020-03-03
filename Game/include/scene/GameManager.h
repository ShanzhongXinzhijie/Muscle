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
	/// ���݂̃��E���h�����K���E���h���擾
	/// </summary>
	bool GetIsPracticeRound()const {
		return m_isPracticeRound;
	}

	/// <summary>
	/// ���݂̃��E���h�����擾
	/// </summary>
	int GetRoundCount()const {
		return m_isPracticeRound ? -1 : m_roundCount;
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

	bool m_isPracticeRound = true;//���ɊJ�n����̂����K���E���h��
	int m_roundCount = 0;//���E���h��
	int m_score[PLAYER_NUM] = {};//�X�R�A

	static constexpr int PRACTICE_TIME_LIMIT = 120;//���K���E���h��������
	static constexpr int TIME_LIMIT = 30;//��������

	int m_maxRound = 5;//�ő僉�E���h
	int m_timeLimitSec = PRACTICE_TIME_LIMIT;//��������(�b)
};

