#pragma once

class CountDown :
	public IGameObject
{
public:
	CountDown();
	~CountDown() {
		GetEngine().SetStandardFrameRate(FRAME_RATE);//�t���[�����[�g�߂�
	}

	/*bool Start()override {
		m_font.LoadFont(L"Resource/font/eunomia_0200/Eunomia.spritefont");
		return true;
	}*/
	void PostLoopUpdate()override {
		//�ŏ��̉��t���[�����͏������Ȃ�
		if (m_unuseFrameCount > 0) {
			m_unuseFrameCount--;
			if (m_unuseFrameCount == 0) {
				GetEngine().SetStandardFrameRate(1);//�t���[�����[�g�ύX
			}
			return;
		}
		//�J�E���g�_�E������
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

