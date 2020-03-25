#pragma once

#include"CGameMode.h"

class CResult :
	public IGameObject
{
public:
	CResult(int score[PLAYER_NUM], bool isNoContest = false) : m_isNoContest(isNoContest){
		m_fontScore.LoadFont(L"Resource/font/eunomia_0200/EunomiaBold.spritefont");//�t�H���g������
		
		int i = 0;
		for (auto& playerscore : m_score) {
			playerscore = score[i];
			i++;
		}

		//�v���C���[�l���̎擾
		m_playerCnt = FindGO<CGameMode>(L"CGameMode")->GetPlayerNum();

		if (m_playerCnt == 1) {
			MAX_PUSH = 6;
		}
	}

	void Update()override;

	void PostLoopUpdate()override;
	void PostRender()override;

private:
	bool m_isNoContest = false;
	int m_playerCnt = 0;
	int m_score[PLAYER_NUM] = {};

	CFont m_font, m_fontScore;

	int m_pushCnt = 0, m_pushCnt2 = 0;
	int MAX_PUSH = 35;
};

