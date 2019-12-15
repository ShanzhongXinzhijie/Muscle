#include "stdafx.h"
#include "CountDown.h"

namespace {
	constexpr int MAX_COUNTDOWN_TYPE = 2;
	constexpr int MAX_COUNTDOWN = 4;
	constexpr wchar_t countDownString[MAX_COUNTDOWN_TYPE][MAX_COUNTDOWN][32] = {
		{L"ÉxÉMÉì",L"óEä∏Ç»",L"ëñÇÈÅc",L"çsÇ≠"},
		{L"ÉxÉMÉì",L"óEä∏Ç»",L"ëñÇÈÅc",L"çsÇ≠"}
	};
}

CountDown::CountDown() {
	m_countDownType = CMath::RandomInt() % MAX_COUNTDOWN_TYPE;
}

void CountDown::PostRender() {
	/*wchar_t string[8];
	swprintf_s(string, L"%.2f", m_countDownSec);
	m_font.Draw(string, 0.5f, CVector4::White(), 5.0f, 0.5f);*/
	int count = MAX_COUNTDOWN - 1 - (int)(m_countDownSec + 0.5f);
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
	);
}