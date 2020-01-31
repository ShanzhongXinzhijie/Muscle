#pragma once

#include"CGameMode.h"

class CResult :
	public IGameObject
{
public:
	CResult(int score[PLAYER_NUM]) {
		m_fontScore.LoadFont(L"Resource/font/eunomia_0200/EunomiaBold.spritefont");//フォント初期化
		
		int i = 0;
		for (auto& playerscore : m_score) {
			playerscore = score[i];
			i++;
		}

		//プレイヤー人数の取得
		m_playerCnt = FindGO<CGameMode>(L"CGameMode")->GetPlayerNum();
	}

	void Update()override;

	void PostLoopUpdate()override;
	void PostRender()override;

private:
	int m_playerCnt = 0;
	int m_score[PLAYER_NUM] = {};

	CFont m_font, m_fontScore;

	int m_pushCnt = 0, m_pushCnt2 = 0;
	static constexpr int MAX_PUSH = 35;
};

