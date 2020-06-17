#pragma once

/// <summary>
/// ウィンドウ・スクリーンサイズ管理
/// </summary>
class WindowSizeManager :
	public IGameObject
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="winscale">画面サイズ</param>
	WindowSizeManager(float winscale) : m_winScale(winscale) {
		ChangeWindowScaleInner();
	};
	bool Start()override;

	/// <summary>
	/// 画面設定切り替え
	/// </summary>
	/// <param name="isTwoScreen">二分割画面か</param>
	void ChangeWindowSize(bool isTwoScreen);

	/// <summary>
	/// 画面サイズ変更
	/// </summary>
	void ChangeWindowScale() {
		m_winScale += 0.25f; if (m_winScale > 2.0f) { m_winScale = 1.0f; }
		ChangeWindowScaleInner();
	}
	/// <summary>
	/// 画面サイズ取得
	/// </summary>
	float GetWindowScale()const {
		return m_winScale;
	}

private:
	void ChangeWindowScaleInner() {
		m_windowSize = static_cast<int>(WINDOW_SIZE * m_winScale);
		m_3dScreenSize = static_cast<int>(SCREEN_SIZE_3D * m_winScale);
	}

private:
	float m_winScale = 1.0f;
	int m_windowSize = WINDOW_SIZE;
	int m_3dScreenSize = SCREEN_SIZE_3D;
};

