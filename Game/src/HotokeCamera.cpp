#include "stdafx.h"
#include "HotokeCamera.h"

void HotokeCameraController::Update() {
	//パッドの入力によって視点を回転
	m_hotokeCam.RotationCamera(m_ptrPad->GetStick(R)*m_padSensi);

	if (!m_lock) {
		//マウスカーソルの動きに連動してカメラ回転
		m_hotokeCam.RotationCamera(MouseCursor().GetMouseMove()*m_mouseSensi);
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
	CVector3 offsetVec = { 120.0f, 250.0f, -370.0f };	
	float z = m_hotokeCam.GetRot().y / CMath::PI_HALF;
	if (z > FLT_EPSILON) {
		offsetVec.z += z*470.0f;
		offsetVec.x += z * -60.0f;
	}
	else {
		offsetVec.z += z*-100.0f;
		offsetVec.y += z*270.0f;
	}
	m_ptrHotoke->GetRot().Multiply(offsetVec);
	m_hotokeCam.SetPos(m_ptrHotoke->GetPos() + offsetVec);

	//カメラ回転設定
	m_hotokeCam.SetRot(m_ptrHotoke->GetRot());

	//カメラ更新
	m_hotokeCam.Update();
};
