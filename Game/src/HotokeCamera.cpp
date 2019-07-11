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

	//カメラ更新
	m_hotokeCam.Update();
};
