#pragma once

#include"TpsCamera.h"

class CPlayerCamera : public IGameObject {
public:
	CPlayerCamera(){
		//カメラの初期化
		m_camera.SetViewAngleDeg(m_angle);
		m_camera.SetNear(10.0f);
		m_camera.SetFar(10000.0f);
		m_camera.SetPos(CVector3::AxisY()*500.0f + CVector3::AxisZ()*1500.0f);
		m_camera.SetTarget(CVector3::AxisY()*1500.0f);

		//メインカメラに設定
		SetMainCamera(&m_camera);

		//マウスカーソルを中央に固定
		MouseCursor().SetLockMouseCursor(true);
		//マウスカーソルを非表示
		MouseCursor().SetShowMouseCursor(false);
	}
	void Update()override {
		if (!m_lock) {
			//マウスカーソルの動きに連動してカメラ回転
			CVector2 mousemove = MouseCursor().GetMouseMove();
			m_rot.y += mousemove.x*m_sensi.x / CMath::PI; m_rot.x -= mousemove.y *m_sensi.y / CMath::PI;
		}

		CVector3 up = CVector3::AxisY(), toTarget = m_target - m_pos;

		CQuaternion cq;
		cq.SetRotation(CVector3::AxisX(), m_rot.x);
		cq.Multiply(toTarget);
		cq.Multiply(up);

		cq.SetRotation(CVector3::AxisY(), m_rot.y);
		cq.Multiply(toTarget);
		cq.Multiply(up);

		m_camera.SetPos(m_pos);
		m_camera.SetTarget(m_camera.GetPos() + toTarget);
		m_camera.SetUp(up);

		/*if (GetAsyncKeyState('R')) {
			m_camera.RotationCamera({ 0.15f,0.0f });
		}
		if (GetAsyncKeyState('Q')) {
			m_camera.RotationCamera({ -0.31f,0.0f });
		}*/

		if (GetAsyncKeyState(VK_LEFT)) {
			m_angle -= 0.4f;
			m_camera.SetViewAngleDeg(m_angle);
		}
		if (GetAsyncKeyState(VK_RIGHT)) {
			m_angle += 0.4f;
			m_camera.SetViewAngleDeg(m_angle);
		}
		if (GetAsyncKeyState(VK_UP)) {
			m_angle = 180.0f;
			m_camera.SetViewAngleDeg(m_angle);
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			m_angle = 1.0f;
			m_camera.SetViewAngleDeg(m_angle);
		}

		if (GetAsyncKeyState('O')) {
			//マウスカーソルを中央に固定
			GetMouseCursorManager().SetLockMouseCursor(true);
			//マウスカーソルを非表示
			GetMouseCursorManager().SetShowMouseCursor(false);
			m_lock = false;
		}
		if (GetAsyncKeyState('P')) {
			//マウスカーソルを中央に固定
			GetMouseCursorManager().SetLockMouseCursor(false);
			//マウスカーソルを非表示
			GetMouseCursorManager().SetShowMouseCursor(true);
			m_lock = true;
		}
	};

	/*CVector3 GetFront()const {
		return m_camera.GetFront();
	}
	CVector3 GetUp()const {
		return m_camera.GetUp();
	}
	CVector3 GetLeft()const {
		return CVector3::GetCross(m_camera.GetFront(), m_camera.GetUp());
	}*/

	void SetPos(const CVector3& pos) { m_pos = pos; }
	void SetTarget(const CVector3& target) { m_target = target; }
	

private:
	GameObj::PerspectiveCamera m_camera;
	CVector3 m_rot;
	CVector2 m_sensi = { 4.0f*(1.0f/1280.0f),-4.0f*(1.0f / 1280.0f) };
	bool m_lock = false;
	float m_angle = 24.0f;

	CVector3 m_pos, m_target;
};

