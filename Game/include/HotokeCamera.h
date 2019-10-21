#pragma once

#include"CDeathHotoke.h"

class HotokeCamera
{
public:
	HotokeCamera() {
		//������
		m_camera.SetFar(15000.0f);
		//m_camera.SetViewAngleDeg(30.0f);			
		m_target = { 0.0f,0.8f,1.0f }; m_target.Normalize();//�����_������
		//���C���J�����ɐݒ�
		SetMainCamera(&m_camera);		
	}

	//���[�V�����u���[�����Z�b�g
	void ResetMotionBlur() { m_camera.ResetIsFirstMatrixUpdate(); }

	//�J��������]������
	void RotationCamera(const CVector2& rot) {
		m_rot += rot;
		if (m_rot.x < -CMath::PI2) { m_rot.x -= -CMath::PI2; }
		if (m_rot.x > CMath::PI2) { m_rot.x -= CMath::PI2; }
		if (m_rot.y < -CMath::PI_HALF) { m_rot.y = -CMath::PI_HALF; }
		if (m_rot.y > CMath::PI_HALF) { m_rot.y = CMath::PI_HALF; }
		//���W�A�b�v�f�[�g
		UpdateVector();
	}
	//�J������]�l��ݒ�
	void SetRotationCamera(const CVector2& rad) {
		m_rot = rad;
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
	//�^�[�Q�b�g���W�I�t�Z�b�g��ݒ�
	void SetTargetPosOffset(const CVector3& vec) {
		m_targetPosOffset = vec;
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
	void SetToMainCamera(int num) {
		GetCameraList().at(num) = &m_camera;
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
		return m_pos + m_targetPosOffset + m_updatedFireTarget * m_camera.GetFar();
	}

	//�J�����X�V
	void Update() {
		m_camera.SetPos(m_pos);
		m_camera.SetTarget(m_pos + m_targetPosOffset + m_updatedTarget * m_camera.GetFar());
		m_camera.SetUp(m_updatedUp);
	}

	//���W��2D��̍��W�ɕϊ�
	CVector3 CalcScreenPosFromWorldPos(const CVector3& pos) {
		return m_camera.CalcScreenPosFromWorldPos(pos);
	}

private:
	//��]��̍��W�Z�o
	void UpdateVector() {
		m_updatedTarget = m_target, m_updatedUp = m_up;
		m_updatedFireTarget = m_fireTarget;

		CQuaternion cq;
		cq.SetRotation(CVector3::AxisX(), m_rot.y);
		cq.Concatenate(CQuaternion(CVector3::AxisY(), m_rot.x));
		cq.Concatenate(m_rotOffset);
		cq.Multiply(m_updatedTarget);
		cq.Multiply(m_updatedFireTarget);
		cq.Multiply(m_updatedUp);
	}

private:
	GameObj::PerspectiveCamera m_camera;

	CVector3 m_pos, m_targetPosOffset, m_target, m_fireTarget = CVector3::Front(), m_up = CVector3::Up();
	CQuaternion m_rotOffset;
	CVector3 m_updatedTarget = CVector3::AxisZ(), m_updatedFireTarget = CVector3::AxisZ(), m_updatedUp = CVector3::Up();
	CVector2 m_rot;
};

class HotokeCameraController : public IGameObject {
public:
	HotokeCameraController(CDeathHotoke* ptrHotoke, IGamePad* ptrPad) : m_ptrHotoke(ptrHotoke), m_ptrPad(ptrPad) {
		//�}�E�X�J�[�\���𒆉��ɌŒ�
		//MouseCursor().SetLockMouseCursor(true);
		m_lock = true;
		//�}�E�X�J�[�\�����\��
		//MouseCursor().SetShowMouseCursor(false);
	}

	void Update()override;

	//���C���J�����ɐݒ�
	void SetToMainCamera() {
		m_hotokeCam.SetToMainCamera();
	}
	void SetToMainCamera(int num) {
		GetCameraList().resize(2);
		m_hotokeCam.SetToMainCamera(num);
	}

	//�O�������擾
	const CVector3& GetFront() {
		return m_hotokeCam.GetFront();
	}
	//�����_���擾
	CVector3 GetVanishingPoint() {
		return m_hotokeCam.GetVanishingPoint();
	}

	//���W��2D��̍��W�ɕϊ�
	CVector3 CalcScreenPosFromWorldPos(const CVector3& pos) {
		return m_hotokeCam.CalcScreenPosFromWorldPos(pos);
	}

private:
	HotokeCamera m_hotokeCam;

	bool m_isBackMirror = false;//�o�b�N�~���[��Ԃ��H

	bool m_lock = false;//�}�E�X�J�[�\���̌Œ�ݒ�
	CVector2 m_mouseSensi = { 4.0f*(1.0f / 1280.0f),4.0f*(1.0f / 1280.0f) };//���_���x(�}�E�X
	CVector2 m_padSensi = { 0.025f,-0.025f };//���_���x(�p�b�h
	
	CDeathHotoke* m_ptrHotoke = nullptr;
	IGamePad* m_ptrPad = nullptr;
};