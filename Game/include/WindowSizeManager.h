#pragma once

/// <summary>
/// ウィンドウ・スクリーンサイズ管理
/// </summary>
class WindowSizeManager :
	public IGameObject
{
public:
	WindowSizeManager(float winscale) : m_winScale(winscale) {
		m_windowSize = static_cast<int>(WINDOW_SIZE * m_winScale);
	};

	void ChangeWindowSize(bool isTwoScreen);

	bool Start()override;

	void ChangeWindowScale() {
		m_winScale += 0.25f; if (m_winScale > 2.0f) { m_winScale = 1.0f; }
		m_windowSize = static_cast<int>(WINDOW_SIZE * m_winScale);
	}
	float GetWindowScale()const {
		return m_winScale;
	}

private:
	float m_winScale = 1.0f;
	int m_windowSize = WINDOW_SIZE;
	int m_3dScreenSize = SCREEN_SIZE_3D;
};

