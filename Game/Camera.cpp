#include "stdafx.h"
#include "Camera.h"


Camera::Camera()
{
	//カメラ作成
	SetMainCamera(&m_cam);
	m_pos = CVector3::Up() * 200.0f + CVector3::AxisZ() * -400.0f;
	m_cam.SetPos(m_pos);
}

Camera::~Camera()
{
}

void Camera::Update() {	
	//設定
	m_cam.SetPos(m_pos);
	m_cam.SetTarget(m_target);
	//上方向の算出
	CVector3 front = m_target - m_pos; front.Normalize();
	CVector3 up;
	if (front.y > 0.5f) {
		up = CVector3::GetCross(front, {front.x,0.0f,front.z});
		up.Cross(front*-1.0f);
	}
	else {
		up = CVector3::GetCross(front, CVector3::Up());
		up.Cross(front);
	}
	m_cam.SetUp(up);

	//マウス入力
	GetMouseCursorManager().SetLockMouseCursor(true);//カーソルを中央に固定
	GetMouseCursorManager().SetShowMouseCursor(false);//カーソルを非表示
	CVector2 mouseMove = GetMouseCursorManager().GetMouseMove()*0.1f;//移動量を取得
	//注視点を回転
	CVector3 eyeLine = m_target - m_pos;
	CQuaternion rot;
	rot.SetRotationDeg(CVector3::AxisY(), mouseMove.x);
	rot.Multiply(eyeLine);
	rot.SetRotationDeg(CVector3::GetCross(CVector3::AxisY(), { front.x,0.0f,front.z }), mouseMove.y);
	rot.Multiply(eyeLine);

	//キー入力で位置移動
	if (GetKeyInput('W')) {
		m_pos += front * 10.0f;
	}
	if (GetKeyInput('S')) {
		m_pos -= front * 10.0f;
	}
	if (GetKeyInput('A')) {
		m_pos += CVector3::GetCross(front, up) * 10.0f;
	}
	if (GetKeyInput('D')) {
		m_pos -= CVector3::GetCross(front, up) * 10.0f;
	}

	//注視点を算出
	m_target = m_pos + eyeLine;
}
