#pragma once

class CResult :
	public IGameObject
{
public:
	CResult(int score[PLAYER_NUM]) {
		m_fontScore.LoadFont(L"Resource/font/eunomia_0200/EunomiaBold.spritefont");//ÉtÉHÉìÉgèâä˙âª
		
		int i = 0;
		for (auto& playerscore : m_score) {
			playerscore = score[i];
			i++;
		}
	}

	void Update()override;

	void PostLoopUpdate()override;
	void PostRender()override;

private:
	int m_score[PLAYER_NUM] = {};

	CFont m_font, m_fontScore;

	int m_pushCnt = 0;
	static constexpr int MAX_PUSH = 35;
};

