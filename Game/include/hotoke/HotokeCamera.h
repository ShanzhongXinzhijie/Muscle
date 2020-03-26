#pragma once

#include"CDeathHotoke.h"
#include"TimeManager.h"

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
	void ResetMotionBlur() { 
		m_camera.ResetIsFirstMatrixUpdate(); 
	}

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
		//���W�A�b�v�f�[�g
		//UpdateVector();
	}

	//���C���J�����ɐݒ�
	void SetToMainCamera() {
		SetMainCamera(&m_camera);
	}
	void SetToMainCamera(int num) {
		SetCameraToList(num, &m_camera);
	}

	//�J�������擾
	GameObj::PerspectiveCamera& GetCamera() {
		return m_camera;
	}
	//�ݒ�ނ��擾
	const CVector3& GetTargetSetting()const {
		return m_target;
	}
	const CVector3& GetUpSetting()const {
		return m_up;
	}

	//�ʒu���擾
	const CVector3& GetPos()const {
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
	CVector3 GetTargetPoint() const {
		return m_pos + m_targetPosOffset + m_updatedFireTarget * m_camera.GetFar();
	}
	//�����_���擾
	CVector3 GetVanishingPoint()const {
		return m_pos + m_updatedTarget * m_camera.GetFar();
	}
	//Far���擾
	float GetFar()const {
		return m_camera.GetFar();
	}
	
	//���W��2D��̍��W�ɕϊ�
	CVector3 CalcScreenPosFromWorldPos(const CVector3& pos) {
		return m_camera.CalcScreenPosFromWorldPos(pos);
	}

	//�J�����X�V
	void Update() {
		m_camera.SetPos(m_pos);
		m_camera.SetTarget(m_pos + m_updatedTarget * m_camera.GetFar());
		m_camera.SetUp(m_updatedUp);
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

	CVector3 m_pos, m_targetPosOffset;
	CVector3 m_target, m_fireTarget = CVector3::Front(), m_up = CVector3::Up();//�ݒ�
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
		MouseCursor().SetShowMouseCursor(false);
	}

	//���t���[���̏���
	void Update()override;

	//���C���J�����ɐݒ�
	void SetToMainCamera() {
		if (m_isZoomOut) {
			SetMainCamera(&m_zoomOutCam);
		}
		else {
			m_hotokeCam.SetToMainCamera();
		}
	}
	void SetToMainCamera(int num) {
		if (m_isZoomOut) {
			SetCameraToList(num, &m_zoomOutCam);
		}
		else {
			m_hotokeCam.SetToMainCamera(num);
		}
	}

	//�Y�[���A�E�g���[�h���ǂ����ݒ�
	void SetIsZoomout(bool isZoomOut, const CVector3& zoomoutDir = { 0.f, 400.f, 800.f }, const IFu* target = nullptr) {
		m_zoomOutDir = zoomoutDir;
		m_zoomOutTarget = target;

		//�J�����ύX
		bool isChangeCam = false;
		if (m_isZoomOut != isZoomOut && isZoomOut) {
			isChangeCam = true;
		}

		if (!m_isZoomOut && isZoomOut) {
			//�J�����X�V(������)
			UpdateZoomOutCamera(true);
		}

		//�ݒ�
		m_isZoomOut = isZoomOut;
		//�J�����ύX
		if (isChangeCam) {
			ChangeCamera();
		}
	}

	//�J�������擾
	/*const GameObj::PerspectiveCamera& GetCamera()const {
		return m_hotokeCam.GetCamera();
	}*/

	//�ʒu���擾
	const CVector3& GetPos()const {
		return m_hotokeCam.GetPos();
	}
	//�O�������擾
	const CVector3& GetFront() {
		return m_hotokeCam.GetFront();
	}
	//�Ə��_���擾
	CVector3 GetTargetPoint()const {
		return m_hotokeCam.GetTargetPoint();
	}
	CVector3 GetVanishingPoint()const {
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
	void ChangeCamera();
	void UpdateZoomOutCamera(bool isInit);

private:
	//�J����
	HotokeCamera m_hotokeCam;//�z�g�P���_
	GameObj::PerspectiveCamera m_zoomOutCam;//�Y�[���A�E�g�p�J����

	//�Y�[���A�E�g
	bool m_isZoomOut = false;
	CVector3 m_zoomOutDir;
	const IFu* m_zoomOutTarget = nullptr;
	float m_zoomPercent = 0.0f;
	CVector3 m_originalPos, m_originalTarget;

	//����
	bool m_isBackMirror = false;//�o�b�N�~���[��Ԃ��H
	bool m_lock = false;//�}�E�X�J�[�\���̌Œ�ݒ�
	CVector2 m_mouseSensi = { 4.0f*(1.0f / 1280.0f),4.0f*(1.0f / 1280.0f) };//���_���x(�}�E�X
	CVector2 m_padSensi = { 0.05f,-0.05f };//���_���x(�p�b�h

	//�p�����[�^
	static constexpr float m_cameraHeight = 16.45f;//�J�����̒n�ʂ���̍���(�l�Ԃ̐g��)
	
	//�Q��
	CDeathHotoke* m_ptrHotoke = nullptr;
	IGamePad* m_ptrPad = nullptr;
};

/*
class ForcusPoint {
	//�V���O���g��
private:
	ForcusPoint() = default;
	~ForcusPoint() = default;
public:
	ForcusPoint(const ForcusPoint&) = delete;
	ForcusPoint& operator=(const ForcusPoint&) = delete;
	ForcusPoint(ForcusPoint&&) = delete;
	ForcusPoint& operator=(ForcusPoint&&) = delete;

public:
	static inline ForcusPoint& GetInstance() {
		static ForcusPoint instance;
		return instance;
	}

	void Enable(const CVector3& zoomoutDir = { 0.f, 400.f, 800.f }, const IFu* target = nullptr) {

	}
	void Disable() {
		m_isEnable = false;
	}

private:
	bool m_isEnable = false;
	CVector3 m_zoomOutDir;
	const IFu* m_zoomOutTarget = nullptr;
};
*/