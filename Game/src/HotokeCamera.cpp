#include "stdafx.h"
#include "HotokeCamera.h"
#include "DemolisherWeapon/physics/CollisionAttr.h"

void HotokeCameraController::Update() {

	//�Y�[���A�E�g����
	m_zoomPercent += m_isZoomOut ? 0.1f : -0.1f;
	m_zoomPercent = CMath::Clamp(m_zoomPercent, 0.0f, 1.0f);

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
	if (m_ptrPad->GetBackMirrorDown()) {
		m_isBackMirror = !m_isBackMirror;
	}
	if (m_isBackMirror) {
		m_hotokeCam.SetRotationCamera(CVector2(m_ptrPad->GetStick(R).x, -m_ptrPad->GetStick(R).y)*CMath::PI_HALF + CVector2(CMath::PI, 0.0f));
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
	CVector3 offsetVec = { 0.0f, -205.0f, -100.0f };
	float z = m_hotokeCam.GetRot().y / CMath::PI_HALF;
	/*if (z > FLT_EPSILON) {
		offsetVec.z += z * 470.0f;
		offsetVec.x += z * -60.0f;
		offsetVec.y += z * 50.0f;
	}
	else {
		offsetVec.z += z * -100.0f;
		offsetVec.y += z * 270.0f;
	}*/
	m_ptrHotoke->GetRot().Multiply(offsetVec);

	//�Y�[���A�E�g
	if (m_zoomPercent > 0.0f) {
		offsetVec = CVector3(0.f, 400.f, 800.f)*m_zoomPercent;
	}

	CVector3 start = m_ptrHotoke->GetPos() + offsetVec, end = m_ptrHotoke->GetPos() + offsetVec; 
	start.y = m_ptrHotoke->GetPos().y; end.y -= m_cameraHeight + 1.0f;
	btCollisionWorld::AllHitsRayResultCallback callback(start, end);
	GetPhysicsWorld().RayTest(start, end, callback);
	start = m_ptrHotoke->GetPos() + offsetVec; start.y -= m_cameraHeight;
	bool isHit = false;
	if (callback.hasHit()) {
		for (int i = 0; i < callback.m_collisionObjects.size(); i++) {
			if (callback.m_collisionObjects[i]->getUserIndex() == enCollisionAttr_CCollisionObj) {
				SuicideObj::CCollisionObj* Obj = (SuicideObj::CCollisionObj*)(callback.m_collisionObjects[i]->getUserPointer());
				if (Obj->GetGroupBitset().test(enField)) {
					if (start.y < callback.m_hitPointWorld[i].y()) { isHit = true; start = callback.m_hitPointWorld[i]; }
				}
			}
			else {
				if (start.y < callback.m_hitPointWorld[i].y()) { isHit = true; start = callback.m_hitPointWorld[i]; }
			}
		}
	}
	start.y += m_cameraHeight; 

	//�ʒu�ݒ�
	m_hotokeCam.SetPos(start); //m_ptrHotoke->GetPos() + CVector3(400,0,0));
	m_hotokeCam.SetTargetPosOffset(offsetVec*-1.0f);

	//�J������]�ݒ�
	if (m_zoomPercent > 0.0f) {
		//�Y�[���A�E�g��
		CQuaternion zoomoutRot(CVector3::AxisX(), CMath::PI_HALF/2.0f);
		zoomoutRot.Concatenate({ CVector3::AxisY(), CMath::PI });
		m_hotokeCam.SetRot(m_ptrHotoke->GetRot().GetSlerp(m_zoomPercent, m_ptrHotoke->GetRot(), zoomoutRot));
	}
	else {
		//�ʏ�
		m_hotokeCam.SetRot(m_ptrHotoke->GetRot());
	}

	//�J�����X�V
	m_hotokeCam.Update();
};
