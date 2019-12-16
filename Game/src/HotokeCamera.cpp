#include "stdafx.h"
#include "HotokeCamera.h"
#include "DemolisherWeapon/physics/CollisionAttr.h"

void HotokeCameraController::ChangeCamera() {
	const GameObj::ICamera* nowCam = !m_isZoomOut ? &m_zoomOutCam : &m_hotokeCam.GetCamera();
	if (nowCam->GetIsMainCamera()) {
		if (m_isZoomOut) {
			SetMainCamera(&m_zoomOutCam);
		}
		else {
			m_hotokeCam.SetToMainCamera();
		}
	}
	for (auto& listcam : GetCameraList()) {
		if (!m_isZoomOut && listcam == &m_zoomOutCam) {
			listcam = &m_hotokeCam.GetCamera();
		}
		if (m_isZoomOut && listcam == &m_hotokeCam.GetCamera()) {
			listcam = &m_zoomOutCam;
		}
	}
}
void HotokeCameraController::UpdateZoomOutCamera(bool isInit) {
	if (isInit) {
		//������
		m_zoomOutCam.ResetIsFirstMatrixUpdate();
		m_zoomOutCam.SetNear(m_hotokeCam.GetCamera().GetNear());
		m_zoomOutCam.SetFar(m_hotokeCam.GetCamera().GetFar());
		m_zoomOutCam.SetViewAngle(m_hotokeCam.GetCamera().GetFOV());
		m_originalPos = m_hotokeCam.GetPos();
		m_originalTarget = (m_hotokeCam.GetVanishingPoint() - m_hotokeCam.GetPos()).GetNorm() * m_zoomOutDir.Length();
	}
	else {
		float beforePer = m_zoomPercent;
		m_zoomPercent += (m_isZoomOut ? 0.04f : -0.04f) * (GetDeltaTimeSec() / FRAME_PER_SECOND);
		m_zoomPercent = CMath::Clamp(m_zoomPercent, 0.0f, 1.0f);
		if (beforePer > 0.0f && !(m_zoomPercent > 0.0f)) {
			ChangeCamera();
		}
	}

	if (m_zoomPercent > 0.0f) {
		if (!m_isZoomOut) {
			m_originalPos = m_hotokeCam.GetPos();
			m_originalTarget = (m_hotokeCam.GetVanishingPoint() - m_hotokeCam.GetPos()).GetNorm() * m_zoomOutDir.Length();
		}

		//�ʒu�ݒ�
		CVector3 targetFuPos = m_zoomOutTarget ? m_zoomOutTarget->GetPos() : m_ptrHotoke->GetPos();
		targetFuPos += m_zoomOutDir;
		CVector3 pos;
		pos.Lerp(m_zoomPercent, m_originalPos, targetFuPos);
		m_zoomOutCam.SetPos(pos);

		//��]
		CVector3 target;
		target.Lerp(m_zoomPercent, m_originalTarget, m_zoomOutDir*-1.0f);
		m_zoomOutCam.SetTarget(pos + target * m_zoomOutCam.GetFar());
	}
}

void HotokeCameraController::Update() {
	//�Y�[���A�E�g����	
	UpdateZoomOutCamera(false);		

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
	m_ptrHotoke->GetRot().Multiply(offsetVec);

	//�n�ʂƔ��肵�č����Z�o
	CVector3 start = m_ptrHotoke->GetPos() + offsetVec, end = m_ptrHotoke->GetPos() + offsetVec;
	start.y = m_ptrHotoke->GetPos().y; end.y -= m_cameraHeight + 1.0f;

	btCollisionWorld::AllHitsRayResultCallback callback(start, end);
	GetPhysicsWorld().RayTest(start, end, callback);
		
	start = m_ptrHotoke->GetPos() + offsetVec; start.y -= m_cameraHeight;
	if (callback.hasHit()) {
		for (int i = 0; i < callback.m_collisionObjects.size(); i++) {
			if (callback.m_collisionObjects[i]->getUserIndex() == enCollisionAttr_CCollisionObj) {
				SuicideObj::CCollisionObj* Obj = (SuicideObj::CCollisionObj*)(callback.m_collisionObjects[i]->getUserPointer());
				if (Obj->GetGroupBitset().test(enField)) {
					if (start.y < callback.m_hitPointWorld[i].y()) { start = callback.m_hitPointWorld[i]; }
				}
			}
			else {
				if (start.y < callback.m_hitPointWorld[i].y()) { start = callback.m_hitPointWorld[i]; }
			}
		}
	}
	start.y += m_cameraHeight;

	//�ʒu�ݒ�
	m_hotokeCam.SetPos(start);
	m_hotokeCam.SetTargetPosOffset(offsetVec*-1.0f);	

	//�J������]�ݒ�
	m_hotokeCam.SetRot(m_ptrHotoke->GetRot());	

	//�J�����X�V
	m_hotokeCam.Update();
};
