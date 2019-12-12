#include "stdafx.h"
#include "HotokeCamera.h"
#include "DemolisherWeapon/physics/CollisionAttr.h"

void HotokeCameraController::Update() {

	//ズームアウト処理
	m_zoomPercent += m_isZoomOut ? 0.1f : -0.1f;
	m_zoomPercent = CMath::Clamp(m_zoomPercent, 0.0f, 1.0f);

	if (!m_lock) {
		//マウスカーソルの動きに連動してカメラ回転
		m_hotokeCam.RotationCamera(MouseCursor().GetMouseMove()*m_mouseSensi);
	}
	else {
		//パッドの入力によって視点を回転
		//m_hotokeCam.RotationCamera(m_ptrPad->GetStick(R)*m_padSensi);
		m_hotokeCam.SetRotationCamera(CVector2(m_ptrPad->GetStick(R).x, -m_ptrPad->GetStick(R).y)*CMath::PI_HALF);
	}

	//バックミラー
	bool oldIsBackMirror = m_isBackMirror;
	if (m_ptrPad->GetBackMirrorDown()) {
		m_isBackMirror = !m_isBackMirror;
	}
	if (m_isBackMirror) {
		m_hotokeCam.SetRotationCamera(CVector2(m_ptrPad->GetStick(R).x, -m_ptrPad->GetStick(R).y)*CMath::PI_HALF + CVector2(CMath::PI, 0.0f));
	}
	if (oldIsBackMirror != m_isBackMirror) {
		//モーションブラーをリセット
		m_hotokeCam.ResetMotionBlur();
	}

	if (GetAsyncKeyState('O')) {
		//マウスカーソルを中央に固定
		GetMouseCursorManager().SetLockMouseCursor(true);
		//マウスカーソルを非表示
		GetMouseCursorManager().SetShowMouseCursor(false);
		m_lock = false;
	}
	if (GetAsyncKeyState('P')) {
		//マウスカーソルを中央に固定
		GetMouseCursorManager().SetLockMouseCursor(false);
		//マウスカーソルを非表示
		GetMouseCursorManager().SetShowMouseCursor(true);
		m_lock = true;
	}

	//カメラ位置設定
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

	//ズームアウト
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

	//位置設定
	m_hotokeCam.SetPos(start); //m_ptrHotoke->GetPos() + CVector3(400,0,0));
	m_hotokeCam.SetTargetPosOffset(offsetVec*-1.0f);

	//カメラ回転設定
	if (m_zoomPercent > 0.0f) {
		//ズームアウト時
		CQuaternion zoomoutRot(CVector3::AxisX(), CMath::PI_HALF/2.0f);
		zoomoutRot.Concatenate({ CVector3::AxisY(), CMath::PI });
		m_hotokeCam.SetRot(m_ptrHotoke->GetRot().GetSlerp(m_zoomPercent, m_ptrHotoke->GetRot(), zoomoutRot));
	}
	else {
		//通常
		m_hotokeCam.SetRot(m_ptrHotoke->GetRot());
	}

	//カメラ更新
	m_hotokeCam.Update();
};
