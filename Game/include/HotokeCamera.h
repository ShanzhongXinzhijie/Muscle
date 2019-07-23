#pragma once

#include"CDeathHotoke.h"

class HotokeCamera
{
public:
	HotokeCamera() {
		//������
		m_camera.SetFar(15000.0f);
		//m_camera.SetViewAngleDeg();		
		//���C���J�����ɐݒ�
		SetMainCamera(&m_camera);
	}

	//�J������]
	void RotationCamera(const CVector2& rot) {
		m_rot += rot;
		if (m_rot.x < -CMath::PI2) { m_rot.x -= -CMath::PI2; }
		if (m_rot.x > CMath::PI2) { m_rot.x -= CMath::PI2; }
		if (m_rot.y < -CMath::PI_HALF) { m_rot.y = -CMath::PI_HALF; }
		if (m_rot.y > CMath::PI_HALF) { m_rot.y = CMath::PI_HALF; }
		//���W�A�b�v�f�[�g
		UpdateVector();
	}
	//�J�����ʒu�ݒ�
	void SetPos(const CVector3& vec) {
		m_pos = vec;
	}
	//�J������]�ݒ�
	//���}�E�X��]�Ƃ͕�
	void SetRot(const CQuaternion& rot) {
		m_rotOffset = rot;
	}

	//���C���J�����ɐݒ�
	void SetToMainCamera() {
		SetMainCamera(&m_camera);
	}

	//��]���擾
	const CVector2& GetRot()const {
		return m_rot;
	}
	//�O�������擾
	const CVector3& GetFront() {
		return m_camera.GetFront();
	}
	//�����_���擾
	CVector3 GetVanishingPoint() {
		return m_pos + m_updatedTarget * m_camera.GetFar();
	}

	void Update() {
		//�J�����X�V
		m_camera.SetPos(m_pos);
		m_camera.SetTarget(m_pos + m_updatedTarget);
		m_camera.SetUp(m_updatedUp);
	}

private:
	//��]��̍��W�Z�o
	void UpdateVector() {
		m_updatedTarget = m_target, m_updatedUp = m_up;

		CQuaternion cq;
		cq.SetRotation(CVector3::AxisX(), m_rot.y);
		cq.Concatenate(CQuaternion(CVector3::AxisY(), m_rot.x));
		cq.Concatenate(m_rotOffset);
		cq.Multiply(m_updatedTarget);
		cq.Multiply(m_updatedUp);
	}

private:
	GameObj::PerspectiveCamera m_camera;

	CVector3 m_pos, m_target = CVector3::AxisZ(), m_up = CVector3::Up();
	CQuaternion m_rotOffset;
	CVector3 m_updatedTarget = CVector3::AxisZ(), m_updatedUp = CVector3::Up();
	CVector2 m_rot;
};

class HotokeCameraController : public IGameObject {
public:
	HotokeCameraController(CDeathHotoke* ptrHotoke) : m_ptrHotoke(ptrHotoke) {
		//�}�E�X�J�[�\���𒆉��ɌŒ�
		MouseCursor().SetLockMouseCursor(true);
		//�}�E�X�J�[�\�����\��
		MouseCursor().SetShowMouseCursor(false);
	}

	void Update()override;

	//���C���J�����ɐݒ�
	void SetToMainCamera() {
		m_hotokeCam.SetToMainCamera();
	}

	//�O�������擾
	const CVector3& GetFront() {
		return m_hotokeCam.GetFront();
	}
	//�����_���擾
	CVector3 GetVanishingPoint() {
		return m_hotokeCam.GetVanishingPoint();
	}

private:
	HotokeCamera m_hotokeCam;
	CVector2 m_sensi = { 4.0f*(1.0f / 1280.0f),4.0f*(1.0f / 1280.0f) };
	bool m_lock = false;

	CDeathHotoke* m_ptrHotoke = nullptr;
};