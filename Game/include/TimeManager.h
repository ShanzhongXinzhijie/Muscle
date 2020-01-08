#pragma once

/// <summary>
/// �t���[�����[�g�𐧌䂷��N���X
/// </summary>
class TimeManager :
	public IQSGameObject
{
	//�V���O���g��
private:
	TimeManager() {
		SetEnable(false);
	}
	~TimeManager() = default;
public:
	TimeManager(const TimeManager&) = delete;
	TimeManager& operator=(const TimeManager&) = delete;
	TimeManager(TimeManager&&) = delete;
	TimeManager& operator=(TimeManager&&) = delete;

public:
	static inline TimeManager& GetInstance() {
		static TimeManager instance;
		return instance;
	}

	/// <summary>
	/// �t���[�����[�g��ݒ�
	/// </summary>
	/// <param name="timelimitSec">�t���[�����[�g���f�t�H���g�ɖ߂��܂ł̎���(�b)</param>
	/// <param name="frameRate">�ݒ肷��t���[�����[�g</param>
	/// <param name="motionBlurScale">�ݒ肷�郂�[�V�����u���[�X�P�[��(0�ȉ��Őݒ肵�Ȃ�)</param>
	void SetFrameRate(float timelimitSec, int frameRate, float motionBlurScale = -1.0f) {
		if (m_lastSetTimeLimit > timelimitSec) {
			//���ݐݒ��萧�����Ԃ��Z���Ȃ�ݒ肵�Ȃ�
			return;
		}
		SetEnable(true);
		m_timeLimitSec = timelimitSec;
		m_lastSetTimeLimit = timelimitSec;
		GetEngine().SetStandardFrameRate(frameRate);
		GetEngine().SetUseFpsLimiter(true, frameRate);
		if (motionBlurScale >= 0.0f) {
			GetGraphicsEngine().GetMotionBlurRender().SetMotionBlurScale(motionBlurScale);
		}
	}
	/// <summary>
	/// �ݒ�����Ƃɖ߂�
	/// </summary>
	void ResetFrameRate() {
		SetEnable(false);
		m_lastSetTimeLimit = 0.0f;
		GetEngine().SetStandardFrameRate(FRAME_RATE);
		GetEngine().SetUseFpsLimiter(true, FRAME_RATE);
		GetGraphicsEngine().GetMotionBlurRender().SetMotionBlurScale();
	}

	/// <summary>
	/// ���t���[���̏���
	/// </summary>
	void PreUpdate()override {
		m_timeLimitSec -= GetDeltaTimeSec();
		m_timeLimitSec = max(0.0f, m_timeLimitSec);
		if (m_timeLimitSec < FLT_EPSILON) {
			ResetFrameRate();//���Ƃɖ߂�
		}
	}

private:
	float m_timeLimitSec = 0.0f;//�t���[�����[�g���f�t�H���g�ɖ߂��܂ł̎���
	float m_lastSetTimeLimit = 0.0f;//�Ō�ɐݒ肵����������
};