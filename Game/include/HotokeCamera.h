#pragma once

#include"CDeathHotoke.h"

class HotokeCamera
{
public:
	HotokeCamera() {
		//������
		m_camera.SetFar(150000.0f);
		//m_camera.SetViewAngleDeg(30.0f);			
		m_target = { 0.0f,0.7f,1.0f }; m_target.Normalize();//�����_������
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

	//�ʒu���擾
	const CVector3& GetPos() {
		return m_camera.GetPos();
	}
	//��]���擾
	const CVector2& GetRot()const {
		return m_rot;
	}
	//�O�������擾
	const CVector3& GetFront() {
		return m_camera.GetFront();
	}
	//�Ə��_���擾
	CVector3 GetTargetPoint() {
		return m_pos + m_targetPosOffset + m_updatedFireTarget * m_camera.GetFar();
	}
	//�����_���擾
	CVector3 GetVanishingPoint() {
		return m_pos + m_updatedTarget * m_camera.GetFar();
	}
	float GetFar()const {
		return m_camera.GetFar();
	}

	//�J�����X�V
	void Update() {
		m_camera.SetPos(m_pos);
		m_camera.SetTarget(m_pos + m_updatedTarget * m_camera.GetFar());
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

	//�ʒu���擾
	const CVector3& GetPos() {
		return m_hotokeCam.GetPos();
	}
	//�O�������擾
	const CVector3& GetFront() {
		return m_hotokeCam.GetFront();
	}
	//�Ə��_���擾
	CVector3 GetTargetPoint() {
		return m_hotokeCam.GetTargetPoint();
	}
	CVector3 GetVanishingPoint() {
		return m_hotokeCam.GetVanishingPoint();
	}	
	float GetFar()const {
		return m_hotokeCam.GetFar();
	}

	//�o�b�N�~���[��Ԃ��擾
	bool GetIsBackMirror()const { return m_isBackMirror; }

	//���W��2D��̍��W�ɕϊ�
	CVector3 CalcScreenPosFromWorldPos(const CVector3& pos) {
		return m_hotokeCam.CalcScreenPosFromWorldPos(pos);
	}

private:
	HotokeCamera m_hotokeCam;

	bool m_isBackMirror = false;//�o�b�N�~���[��Ԃ��H

	bool m_lock = false;//�}�E�X�J�[�\���̌Œ�ݒ�
	CVector2 m_mouseSensi = { 4.0f*(1.0f / 1280.0f),4.0f*(1.0f / 1280.0f) };//���_���x(�}�E�X
	CVector2 m_padSensi = { 0.05f,-0.05f };//���_���x(�p�b�h

	float m_cameraHeight = 16.45f;//�J�����̒n�ʂ���̍���
	
	CDeathHotoke* m_ptrHotoke = nullptr;
	IGamePad* m_ptrPad = nullptr;
};