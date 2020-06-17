#pragma once

/// <summary>
/// �E�B���h�E�E�X�N���[���T�C�Y�Ǘ�
/// </summary>
class WindowSizeManager :
	public IGameObject
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="winscale">��ʃT�C�Y</param>
	WindowSizeManager(float winscale) : m_winScale(winscale) {
		ChangeWindowScaleInner();
	};
	bool Start()override;

	/// <summary>
	/// ��ʐݒ�؂�ւ�
	/// </summary>
	/// <param name="isTwoScreen">�񕪊���ʂ�</param>
	void ChangeWindowSize(bool isTwoScreen);

	/// <summary>
	/// ��ʃT�C�Y�ύX
	/// </summary>
	void ChangeWindowScale() {
		m_winScale += 0.25f; if (m_winScale > 2.0f) { m_winScale = 1.0f; }
		ChangeWindowScaleInner();
	}
	/// <summary>
	/// ��ʃT�C�Y�擾
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

