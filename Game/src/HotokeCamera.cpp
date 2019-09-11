#include "stdafx.h"
#include "HotokeCamera.h"

void HotokeCameraController::Update() {
	if (!m_lock) {
		//�}�E�X�J�[�\���̓����ɘA�����ăJ������]
		m_hotokeCam.RotationCamera(MouseCursor().GetMouseMove()*m_mouseSensi);
	}
	else {
		//�p�b�h�̓��͂ɂ���Ď��_����]
		//m_hotokeCam.RotationCamera(m_ptrPad->GetStick(R)*m_padSensi);
		m_hotokeCam.SetRotationCamera(CVector2(m_ptrPad->GetStick(R).x, -m_ptrPad->GetStick(R).y)*CMath::PI_HALF);
	}

	//�o�b�N�~���[
	bool oldIsBackMirror = m_isBackMirror;
	if (m_ptrPad->GetBackMirror()) {
		m_hotokeCam.SetRotationCamera(CVector2(m_ptrPad->GetStick(R).x, -m_ptrPad->GetStick(R).y)*CMath::PI_HALF + CVector2(CMath::PI,0.0f));
		m_isBackMirror = true;
	}
	else {
		m_isBackMirror = false;
	}
	if (oldIsBackMirror != m_isBackMirror) {
		//���[�V�����u���[�����Z�b�g
		m_hotokeCam.ResetMotionBlur();
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

	//�J�����ʒu�ݒ�
	CVector3 offsetVec = { 120.0f, 200.0f, -370.0f };	
	float z = m_hotokeCam.GetRot().y / CMath::PI_HALF;
	if (z > FLT_EPSILON) {
		offsetVec.z += z*470.0f;
		offsetVec.x += z * -60.0f;
		offsetVec.y += z * 50.0f;
	}
	else {
		offsetVec.z += z*-100.0f;
		offsetVec.y += z*270.0f;
	}
	m_ptrHotoke->GetRot().Multiply(offsetVec);
	m_hotokeCam.SetPos(m_ptrHotoke->GetPos() + offsetVec);

	//�J������]�ݒ�
	m_hotokeCam.SetRot(m_ptrHotoke->GetRot());

	//�J�����X�V
	m_hotokeCam.Update();
};
