#pragma once

class CountDown :
	public IGameObject
{
public:
	CountDown();
	~CountDown() {
		GetEngine().SetStandardFrameRate(FRAME_RATE);//フレームレート戻す
	}

	/*bool Start()override {
		m_font.LoadFont(L"Resource/font/eunomia_0200/Eunomia.spritefont");
		return true;
	}*/
	void PostLoopUpdate()override {
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
			delete this;
			return;
		}
	}
	void PostRender()override;

private:
	CFont m_font;
	int m_unuseFrameCount = 2;
	int m_countDownType = 0;
	float m_countDownSec = 3.0f;
};

