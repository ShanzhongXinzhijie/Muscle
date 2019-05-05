#include "stdafx.h"
#include "Camera.h"


Camera::Camera()
{
	//�J�����쐬
	SetMainCamera(&m_cam);
	m_pos = CVector3::Up() * 200.0f + CVector3::AxisZ() * -400.0f;
	m_cam.SetPos(m_pos);
}

Camera::~Camera()
{
}

void Camera::Update() {	
	//�ݒ�
	m_cam.SetPos(m_pos);
	m_cam.SetTarget(m_target);
	//������̎Z�o
	CVector3 front = m_target - m_pos; front.Normalize();
	CVector3 up;
	if (front.y > 0.5f) {
		up = CVector3::GetCross(front, {front.x,0.0f,front.z});
		up.Cross(front*-1.0f);
	}
	else {
		up = CVector3::GetCross(front, CVector3::Up());
		up.Cross(front);
	}
	m_cam.SetUp(up);

	//�}�E�X����
	GetMouseCursorManager().SetLockMouseCursor(true);//�J�[�\���𒆉��ɌŒ�
	GetMouseCursorManager().SetShowMouseCursor(false);//�J�[�\�����\��
	CVector2 mouseMove = GetMouseCursorManager().GetMouseMove()*0.1f;//�ړ��ʂ��擾
	//�����_����]
	CVector3 eyeLine = m_target - m_pos;
	CQuaternion rot;
	rot.SetRotationDeg(CVector3::AxisY(), mouseMove.x);
	rot.Multiply(eyeLine);
	rot.SetRotationDeg(CVector3::GetCross(CVector3::AxisY(), { front.x,0.0f,front.z }), mouseMove.y);
	rot.Multiply(eyeLine);

	//�L�[���͂ňʒu�ړ�
	if (GetKeyInput('W')) {
		m_pos += front * 10.0f;
	}
	if (GetKeyInput('S')) {
		m_pos -= front * 10.0f;
	}
	if (GetKeyInput('A')) {
		m_pos += CVector3::GetCross(front, up) * 10.0f;
	}
	if (GetKeyInput('D')) {
		m_pos -= CVector3::GetCross(front, up) * 10.0f;
	}

	//�����_���Z�o
	m_target = m_pos + eyeLine;
}
