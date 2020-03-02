#include "stdafx.h"
#include "CountDown.h"
#include "TimeManager.h"
#include "CDeathHotoke.h"

CountDown::CountDown(int round, int roundmax, int score[PLAYER_NUM], int timeLimitSec)
	: m_roundCount(round), m_maxRound(roundmax), m_timeLimitSec(timeLimitSec)
{
	SetName(L"CountDown");

	int i = 0;
	for (auto& playerscore : m_score) {
		playerscore = score[i];
		i++;
	}

	TimeManager::GetInstance().ResetFrameRate();//�t���[�����[�g���f�t�H���g�ɖ߂�
	//m_countDownType = CMath::RandomInt() % MAX_COUNTDOWN_TYPE;
}

bool CountDown::Start() {
	//�t�H���g������
	m_font.LoadFont(L"Resource/font/eunomia_0200/EunomiaBold.spritefont");
	m_fontJPN.LoadFont(L"Resource/font/x14y24pxHeadUpDaisy.spritefont"); 
	return true;
}

void CountDown::PostLoopUpdate() {
	//�ŏ��̉��t���[�����͏������Ȃ�
	if (m_unuseFrameCount > 0) {
		m_unuseFrameCount--;
		if (m_unuseFrameCount == 0) {
			GetEngine().SetStandardFrameRate(1);//�t���[�����[�g�ύX
		}
		return;
	}
	//�J�E���g�_�E������
	m_countDownSec -= GetRealDeltaTimeSec()*2.0f;// 1.5f;
	//�J�E���g�_�E���I��
	if (m_countDownSec < 0.0f) {
		//���ׂẴz�g�P�𑀍�\��
		QueryGOs<CDeathHotoke>(L"CDeathHotoke",
			[](CDeathHotoke* go) {
				go->SetIsControl(true);
				return true;
			}
		);
		//���E
		delete this;
		return;
	}
}

void CountDown::PostRender() {
	bool isJpnFont = false;
	wchar_t string[64];
	CVector4 color = CVector4::Black();
	CVector2 scale = 0.8f;

	if (m_countDownSec > 4.0f) {
		scale.x *= 1.0f - CMath::Saturate(m_countDownSec - 5.0f);

		if (m_roundCount < 0) {
			wcscpy_s(string, L"��񂵂イ���E���h"); 
			isJpnFont = true;
		}else if (m_roundCount + 1 == m_maxRound) {
			color.x = 1.0f;
			m_font.Draw(L"FINALROUND", { 0.5f,0.4f }, color, scale, { 0.565f, 1.0f });
			color = CVector4::Black();
			swprintf_s(string, L"\n%d - %d",  m_score[0], m_score[1]);
		}
		else {
			swprintf_s(string, L"ROUND %d/%d", m_roundCount + 1, m_maxRound);
			m_font.Draw(string, { 0.5f,0.4f }, color, scale, { 0.575f, 1.0f });
			swprintf_s(string, L"\n%d - %d",  m_score[0], m_score[1]);
		}
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

	if (isJpnFont) {
		m_fontJPN.Draw(string, 0.5f, color, scale*2.5f, { 0.575f, 0.6f });
	}
	else {
		m_font.Draw(string, 0.5f, color, scale, { 0.575f, 0.6f });
	}
}