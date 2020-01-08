#pragma once

class CountDown :
	public IGameObject
{
public:
	CountDown(int round, int roundmax, int score[PLAYER_NUM], int timeLimitSec);
	~CountDown() {
		GetEngine().SetStandardFrameRate(FRAME_RATE);//フレームレート戻す
	}

	bool Start()override;
	void PostLoopUpdate()override;
	void PostRender()override;

private:
	CFont m_font;

	int m_unuseFrameCount = 2;
	//int m_countDownType = 0;
	float m_countDownSec = 6.0f;

	int m_roundCount = 0, m_maxRound = 0;
	int m_score[PLAYER_NUM] = {};
	int m_timeLimitSec = 0;
};

