#pragma once

/// <summary>
/// �Y�[���A�E�g����J����
/// </summary>
class ZoomOutCamera :
	public IGameObject
{
public:
	ZoomOutCamera() {
		m_cam.SetEnable(false);
	}

	//�Y�[���J�n
	void Init(const GameObj::PerspectiveCamera& camera, bool isZoomOut);
	//�Y�[���������I�����Ă��邩?
	bool GetIsFinish()const {
		if (m_isZoomout) {
			return m_zoomOutTimer > 1.0f - FLT_EPSILON;
		}
		else {
			return m_isDisable;
		}
	}

	void Update()override;

private:
	void Disable();

private:
	bool m_isZoomout = false;
	bool m_isDisable = true;
	float m_zoomOutTimer = 0.0f;
	GameObj::PerspectiveCamera m_cam;
	const GameObj::PerspectiveCamera* m_beforeCam = nullptr;
};

