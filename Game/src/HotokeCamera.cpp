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

	//�J�����X�V
	m_hotokeCam.Update();
};
