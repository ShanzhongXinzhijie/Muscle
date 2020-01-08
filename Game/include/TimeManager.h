#pragma once

/// <summary>
/// フレームレートを制御するクラス
/// </summary>
class TimeManager :
	public IQSGameObject
{
	//シングルトン
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
	/// フレームレートを設定
	/// </summary>
	/// <param name="timelimitSec">フレームレートをデフォルトに戻すまでの時間(秒)</param>
	/// <param name="frameRate">設定するフレームレート</param>
	/// <param name="motionBlurScale">設定するモーションブラースケール(0以下で設定しない)</param>
	void SetFrameRate(float timelimitSec, int frameRate, float motionBlurScale = -1.0f) {
		if (m_lastSetTimeLimit > timelimitSec) {
			//現在設定より制限時間が短いなら設定しない
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
	/// 設定をもとに戻す
	/// </summary>
	void ResetFrameRate() {
		SetEnable(false);
		m_lastSetTimeLimit = 0.0f;
		GetEngine().SetStandardFrameRate(FRAME_RATE);
		GetEngine().SetUseFpsLimiter(true, FRAME_RATE);
		GetGraphicsEngine().GetMotionBlurRender().SetMotionBlurScale();
	}

	/// <summary>
	/// 毎フレームの処理
	/// </summary>
	void PreUpdate()override {
		m_timeLimitSec -= GetDeltaTimeSec();
		m_timeLimitSec = max(0.0f, m_timeLimitSec);
		if (m_timeLimitSec < FLT_EPSILON) {
			ResetFrameRate();//もとに戻す
		}
	}

private:
	float m_timeLimitSec = 0.0f;//フレームレートをデフォルトに戻すまでの時間
	float m_lastSetTimeLimit = 0.0f;//最後に設定した制限時間
};