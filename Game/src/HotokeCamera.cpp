#include "stdafx.h"
#include "HotokeCamera.h"

void HotokeCameraController::Update() {
	if (!m_lock) {
		//マウスカーソルの動きに連動してカメラ回転
		CVector2 mousemove = MouseCursor().GetMouseMove();
		mousemove.x *= m_sensi.x; mousemove.y *= m_sensi.y;
		m_hotokeCam.RotationCamera(mousemove);
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

	//カメラ回転設定
	m_hotokeCam.SetRot(m_ptrHotoke->GetRot());

	//カメラ更新
	m_hotokeCam.Update();
};
