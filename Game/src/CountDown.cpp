#include "stdafx.h"
#include "CountDown.h"
#include "TimeManager.h"

//namespace {
//	constexpr int MAX_COUNTDOWN_TYPE = 2;
//	constexpr int MAX_COUNTDOWN = 4;
//	constexpr wchar_t countDownString[MAX_COUNTDOWN_TYPE][MAX_COUNTDOWN][32] = {
//		{L"ベギン",L"勇敢な",L"走る…",L"行く"},
//		{L"ベギン",L"勇敢な",L"走る…",L"行く"}
//	};
//}

CountDown::CountDown(int round, int roundmax, int score[PLAYER_NUM], int timeLimitSec)
	: m_roundCount(round), m_maxRound(roundmax), m_timeLimitSec(timeLimitSec)
{
	int i = 0;
	for (auto& playerscore : m_score) {
		playerscore = score[i];
		i++;
	}

	TimeManager::GetInstance().ResetFrameRate();//フレームレートをデフォルトに戻す
	//m_countDownType = CMath::RandomInt() % MAX_COUNTDOWN_TYPE;
}

bool CountDown::Start() {
	m_font.LoadFont(L"Resource/font/eunomia_0200/EunomiaBold.spritefont");//フォント初期化
	return true;
}

void CountDown::PostLoopUpdate() {
	//最初の何フレームかは処理しない
	if (m_unuseFrameCount > 0) {
		m_unuseFrameCount--;
		if (m_unuseFrameCount == 0) {
			GetEngine().SetStandardFrameRate(1);//フレームレート変更
		}
		return;
	}
	//カウントダウン減少
	m_countDownSec -= GetRealDeltaTimeSec();
	if (m_countDownSec < 0.0f) {
		//自殺
		delete this;
		return;
	}
}

void CountDown::PostRender() {
	wchar_t string[64];
	CVector4 color = CVector4::Black();
	CVector2 scale = 0.8f;

	if (m_countDownSec > 4.0f) {
		if (m_roundCount + 1 == m_maxRound) {
			swprintf_s(string, L"FINALROUND\n%d - %d",  m_score[0], m_score[1]);
			color.x = 1.0f;
		}
		else {
			swprintf_s(string, L"ROUND %d/%d\n%d - %d", m_roundCount + 1, m_maxRound, m_score[0], m_score[1]);
		}
		scale.x *= 1.0f - CMath::Saturate(m_countDownSec - 5.0f);
	}else
	if (m_countDownSec > 2.0f) {
		swprintf_s(string, L"TIME LIMIT\n%2d:%2d", m_timeLimitSec / 60, m_timeLimitSec % 60);
		scale.x *= 1.0f - CMath::Saturate(m_countDownSec - 3.0f);
	}else
	if (m_countDownSec > 0.0f) {		
		wcscpy_s(string, L"START");
		scale *= (1.0f - CMath::Saturate(m_countDownSec - 1.0f)) * 2.0f;
		color.x = 1.0f;
	}

	//float scale, x; 
	//scale = (1.0f-std::modf(m_countDownSec, &x));

	m_font.Draw(string, 0.5f, color, scale, { 0.575f, 0.6f});
	
	/*int count = MAX_COUNTDOWN - 1 - (int)(m_countDownSec + 0.5f);
	float scale,x;
	if (count == MAX_COUNTDOWN-1) {
		scale = 4.0f*(1.0f-std::modf(m_countDownSec, &x));
	}
	else {
		scale = 2.0f*std::modf(m_countDownSec, &x);
	}
	m_font.Draw(
		countDownString[m_countDownType][count], 0.5f,
		CVector4::White(), 5.0f*scale, 0.5f,
		0.0f,DirectX::SpriteEffects_None,
		0.0f
	);*/
}