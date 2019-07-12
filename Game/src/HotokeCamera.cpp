#include "stdafx.h"
#include "HotokeCamera.h"

void HotokeCameraController::Update() {
	if (!m_lock) {
		//�}�E�X�J�[�\���̓����ɘA�����ăJ������]
		CVector2 mousemove = MouseCursor().GetMouseMove();
		mousemove.x *= m_sensi.x; mousemove.y *= m_sensi.y;
		m_hotokeCam.RotationCamera(mousemove);
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
	CVector3 offsetVec = { 60.0f, 250.0f, -170.0f };
	
	float z = m_hotokeCam.GetRot().y;
	if (z > FLT_EPSILON) {
		offsetVec += CVector3::AxisZ()*((z / CMath::PI_HALF)*270.0f);
	}
	else {
		offsetVec += CVector3::AxisZ()*((z / CMath::PI_HALF)*100.0f);
		offsetVec += CVector3::AxisY()*((z / CMath::PI_HALF)*270.0f);
	}

	m_ptrHotoke->GetRot().Multiply(offsetVec);
	m_hotokeCam.SetPos(m_ptrHotoke->GetPos() + offsetVec);

	//�J������]�ݒ�
	m_hotokeCam.SetRot(m_ptrHotoke->GetRot());

	//�J�����X�V
	m_hotokeCam.Update();
};
