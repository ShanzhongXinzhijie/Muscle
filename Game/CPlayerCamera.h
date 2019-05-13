#pragma once

#include"TpsCamera.h"

class CPlayerCamera : public IGameObject {
public:
	CPlayerCamera(){//bool Start()override {
		//TPSカメラの初期化
		m_camera.Init(CVector3::Zero(), CVector3::AxisZ()*100.0f + CVector3::AxisX()*50.0f, CVector3::Up());
		m_camera.GetCamera().SetFar(5000.0f);
		//m_camera.GetCamera().SetNear(1000);
		//メインカメラに設定
		m_camera.SetToMainCamera();

		//マウスカーソルを中央に固定
		MouseCursor().SetLockMouseCursor(true);
		//マウスカーソルを非表示
		MouseCursor().SetShowMouseCursor(false);

		m_camera.RotationCamera({ 0.0f,CMath::PI*-0.5f });

		//return true;
	}
	void Update()override {
		if (!m_lock) {
			//マウスカーソルの動きに連動してカメラ回転
			CVector2 mousemove = MouseCursor().GetMouseMove();
			mousemove.x *= m_sensi.x; mousemove.y *= m_sensi.y;
			m_camera.RotationCamera(mousemove);
		}

		if (GetAsyncKeyState('R')) {
			m_camera.RotationCamera({ 0.15f,0.0f });
		}
		if (GetAsyncKeyState('Q')) {
			m_camera.RotationCamera({ -0.31f,0.0f });
		}

		if (GetAsyncKeyState(VK_LEFT)) {
			m_camera.GetCamera().SetViewAngleDeg(angle);
			angle -= 0.4f;
		}
		if (GetAsyncKeyState(VK_RIGHT)) {
			m_camera.GetCamera().SetViewAngleDeg(angle);
			angle += 0.4f;
		}

		if (GetAsyncKeyState(VK_UP)) {
			m_camera.GetCamera().SetViewAngleDeg(angle);
			angle = 180.0f;
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			m_camera.GetCamera().SetViewAngleDeg(angle);
			angle = 1.0f;
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

		//カメラ更新
		m_camera.Update();
	};

	void SetPos(const CVector3& v) {
		m_camera.SetTarget(v);
	}

	CVector3 GetFront()const {
		return m_camera.GetFront();
	}
	CVector3 GetUp()const {
		return m_camera.GetUp();
	}
	CVector3 GetLeft()const {
		return CVector3::GetCross(m_camera.GetFront(), m_camera.GetUp());
	}

private:
	TpsCamera m_camera;
	CVector2 m_sensi = { 4.0f*(1.0f/1280.0f),-4.0f*(1.0f / 1280.0f) };
	bool m_lock = false;
	float angle = 90.0f;
};

