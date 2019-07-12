#pragma once
class HotokeCamera
{
public:
	HotokeCamera() {
		//初期化
		m_camera.SetFar(15000.0f);
		//m_camera.SetViewAngleDeg();		
		//メインカメラに設定
		SetMainCamera(&m_camera);
	}

	//カメラ回転
	void RotationCamera(const CVector2& rot) {
		m_rot += rot;
		if (m_rot.x < -CMath::PI2) { m_rot.x -= -CMath::PI2; }
		if (m_rot.x > CMath::PI2) { m_rot.x -= CMath::PI2; }
		if (m_rot.y < -CMath::PI_HALF) { m_rot.y = -CMath::PI_HALF; }
		if (m_rot.y > CMath::PI_HALF) { m_rot.y = CMath::PI_HALF; }
		//座標アップデート
		UpdateVector();
	}
	//カメラ位置設定
	void SetPos(const CVector3& vec) {
		m_pos = vec;
	}
	//メインカメラに設定
	void SetToMainCamera() {
		SetMainCamera(&m_camera);
	}

	//回転を取得
	const CVector2& GetRot()const {
		return m_rot;
	}
	//前方向を取得
	const CVector3& GetFront() {
		return m_camera.GetFront();
	}
	//消失点を取得
	const CVector3& GetVanishingPoint() {
		return m_pos + m_updatedTarget * m_camera.GetFar();
	}

	void Update() {
		//カメラ更新
		m_camera.SetPos(m_pos);
		m_camera.SetTarget(m_pos + m_updatedTarget);
		m_camera.SetUp(m_updatedUp);
	}

private:
	//回転後の座標算出
	void UpdateVector() {
		m_updatedTarget = m_target, m_updatedUp = m_up;

		CQuaternion cq;
		cq.SetRotation(CVector3::AxisX(), m_rot.y);
		cq.Multiply(m_updatedTarget);
		cq.Multiply(m_updatedUp);

		cq.SetRotation(CVector3::AxisY(), m_rot.x);
		cq.Multiply(m_updatedTarget);
		cq.Multiply(m_updatedUp);
	}

private:
	GameObj::PerspectiveCamera m_camera;

	CVector3 m_pos, m_target = CVector3::AxisZ(), m_up = CVector3::Up();
	CVector3 m_updatedTarget = CVector3::AxisZ(), m_updatedUp = CVector3::Up();
	CVector2 m_rot;
};

class HotokeCameraController : public IGameObject {
public:
	HotokeCameraController() {
		//マウスカーソルを中央に固定
		MouseCursor().SetLockMouseCursor(true);
		//マウスカーソルを非表示
		MouseCursor().SetShowMouseCursor(false);
	}

	void Update()override;

	//カメラ位置設定
	void SetPos(const CVector3& vec) {
		CVector3 offset;
		float z = m_hotokeCam.GetRot().y;
		if (z > FLT_EPSILON) {
			offset = CVector3::AxisZ()*((z / CMath::PI_HALF)*270.0f);
		}
		else {
			offset = CVector3::AxisZ()*((z / CMath::PI_HALF)*100.0f);
			offset += CVector3::AxisY()*((z / CMath::PI_HALF)*270.0f);
		}
		m_hotokeCam.SetPos(vec + offset);
	}
	//メインカメラに設定
	void SetToMainCamera() {
		m_hotokeCam.SetToMainCamera();
	}

	//前方向を取得
	const CVector3& GetFront() {
		return m_hotokeCam.GetFront();
	}
	//消失点を取得
	const CVector3& GetVanishingPoint() {
		return m_hotokeCam.GetVanishingPoint();
	}

private:
	HotokeCamera m_hotokeCam;
	CVector2 m_sensi = { 4.0f*(1.0f / 1280.0f),4.0f*(1.0f / 1280.0f) };
	bool m_lock = false;
};