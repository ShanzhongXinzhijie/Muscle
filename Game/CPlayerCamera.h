#pragma once

#include"TpsCamera.h"

class CPlayerCamera : public IGameObject {
public:
	CPlayerCamera(){//bool Start()override {
		//TPS�J�����̏�����
		m_camera.Init(CVector3::Zero(), CVector3::AxisZ()*100.0f + CVector3::AxisX()*50.0f, CVector3::Up());
		m_camera.GetCamera().SetFar(5000.0f);
		//m_camera.GetCamera().SetNear(1000);
		//���C���J�����ɐݒ�
		m_camera.SetToMainCamera();

		//�}�E�X�J�[�\���𒆉��ɌŒ�
		MouseCursor().SetLockMouseCursor(true);
		//�}�E�X�J�[�\�����\��
		MouseCursor().SetShowMouseCursor(false);

		m_camera.RotationCamera({ 0.0f,CMath::PI*-0.5f });

		//return true;
	}
	void Update()override {
		if (!m_lock) {
			//�}�E�X�J�[�\���̓����ɘA�����ăJ������]
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
			//�}�E�X�J�[�\���𒆉��ɌŒ�
			GetMouseCursorManager().SetLockMouseCursor(true);
			//�}�E�X�J�[�\�����\��
			GetMouseCursorManager().SetShowMouseCursor(false);
			m_lock = false;
		}
		if (GetAsyncKeyState('P')) {
			//�}�E�X�J�[�\���𒆉��ɌŒ�
			GetMouseCursorManager().SetLockMouseCursor(false);
			//�}�E�X�J�[�\�����\��
			GetMouseCursorManager().SetShowMouseCursor(true);
			m_lock = true;
		}

		//�J�����X�V
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

