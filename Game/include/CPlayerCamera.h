#pragma once

#include"TpsCamera.h"

class CPlayerCamera : public IGameObject {
public:
	CPlayerCamera(){
		//�J�����̏�����
		m_camera.SetViewAngleDeg(m_angle);
		m_camera.SetNear(10.0f);
		m_camera.SetFar(10000.0f);
		m_camera.SetPos(CVector3::AxisY()*1500.0f + CVector3::AxisZ()*4000.0f);
		m_camera.SetTarget(CVector3::AxisY()*500.0f);

		//���C���J�����ɐݒ�
		SetMainCamera(&m_camera);

		//�}�E�X�J�[�\���𒆉��ɌŒ�
		MouseCursor().SetLockMouseCursor(true);
		//�}�E�X�J�[�\�����\��
		MouseCursor().SetShowMouseCursor(false);
	}
	void Update()override {
		//if (!m_lock) {
		//	//�}�E�X�J�[�\���̓����ɘA�����ăJ������]
		//	CVector2 mousemove = MouseCursor().GetMouseMove();
		//	mousemove.x *= m_sensi.x; mousemove.y *= m_sensi.y;
		//	//m_camera.RotationCamera(mousemove);

		//	CVector3 up = m_camera.GetUp(), toTarget = m_camera.GetTarget() - m_camera.GetPos();

		//	CQuaternion cq;
		//	cq.SetRotation(CVector3::AxisX(), mousemove.y);
		//	cq.Multiply(toTarget);
		//	cq.Multiply(up);

		//	cq.SetRotation(CVector3::AxisY(), mousemove.x);
		//	cq.Multiply(toTarget);
		//	cq.Multiply(up);

		//	m_camera.SetTarget(m_camera.GetPos() + toTarget);
		//	m_camera.SetUp(up);
		//}

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

private:
	GameObj::PerspectiveCamera m_camera;
	CVector2 m_sensi = { 4.0f*(1.0f/1280.0f),-4.0f*(1.0f / 1280.0f) };
	bool m_lock = false;
	float m_angle = 24.0f;
};

