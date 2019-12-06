#pragma once

/// <summary>
/// ズームアウトするカメラ
/// </summary>
class ZoomOutCamera :
	public IGameObject
{
public:
	ZoomOutCamera() {
		m_cam.SetEnable(false);
	}

	//ズーム開始
	void Init(const GameObj::PerspectiveCamera& camera, bool isZoomOut);
	//ズーム処理が終了しているか?
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

