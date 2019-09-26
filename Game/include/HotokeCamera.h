#pragma once

#include"CDeathHotoke.h"

class HotokeCamera
{
public:
	HotokeCamera() {
		//初期化
		m_camera.SetFar(15000.0f);
		//m_camera.SetViewAngleDeg();		
		//メインカメラに設定
		SetMainCamera(&m_camera);
	}

	//モーションブラーをリセット
	void ResetMotionBlur() { m_camera.ResetIsFirstMatrixUpdate(); }

	//カメラを回転させる
	void RotationCamera(const CVector2& rot) {
		m_rot += rot;
		if (m_rot.x < -CMath::PI2) { m_rot.x -= -CMath::PI2; }
		if (m_rot.x > CMath::PI2) { m_rot.x -= CMath::PI2; }
		if (m_rot.y < -CMath::PI_HALF) { m_rot.y = -CMath::PI_HALF; }
		if (m_rot.y > CMath::PI_HALF) { m_rot.y = CMath::PI_HALF; }
		//座標アップデート
		UpdateVector();
	}
	//カメラ回転値を設定
	void SetRotationCamera(const CVector2& rad) {
		m_rot = rad;
		if (m_rot.x < -CMath::PI2) { m_rot.x -= -CMath::PI2; }
		if (m_rot.x > CMath::PI2) { m_rot.x -= CMath::PI2; }
		if (m_rot.y < -CMath::PI_HALF) { m_rot.y = -CMath::PI_HALF; }
		if (m_rot.y > CMath::PI_HALF) { m_rot.y = CMath::PI_HALF; }
		//座標アップデート
		UpdateVector();
	}

	//カメラ位置設定
	void SetPos(const CVector3& vec) {
		m_pos = vec;
	}
	//カメラ回転設定
	//※マウス回転とは別
	void SetRot(const CQuaternion& rot) {
		m_rotOffset = rot;
	}

	//メインカメラに設定
	void SetToMainCamera() {
		SetMainCamera(&m_camera);
	}

	//回転を取得
	const CVector2& GetRot()const {
		return m_rot;
	}
	//前方向を取得
	const CVector3& GetFront() {
		return m_camera.GetFront();
	}
	//消失点を取得
	CVector3 GetVanishingPoint() {
		return m_pos + m_updatedTarget * m_camera.GetFar();
	}

	//カメラ更新
	void Update() {
		m_camera.SetPos(m_pos);
		m_camera.SetTarget(m_pos + m_updatedTarget);
		m_camera.SetUp(m_updatedUp);
	}

	//座標を2D上の座標に変換
	CVector3 CalcScreenPosFromWorldPos(const CVector3& pos) {
		return m_camera.CalcScreenPosFromWorldPos(pos);
	}

private:
	//回転後の座標算出
	void UpdateVector() {
		m_updatedTarget = m_target, m_updatedUp = m_up;

		CQuaternion cq;
		cq.SetRotation(CVector3::AxisX(), m_rot.y);
		cq.Concatenate(CQuaternion(CVector3::AxisY(), m_rot.x));
		cq.Concatenate(m_rotOffset);
		cq.Multiply(m_updatedTarget);
		cq.Multiply(m_updatedUp);
	}

private:
	GameObj::PerspectiveCamera m_camera;

	CVector3 m_pos, m_target = CVector3::AxisZ(), m_up = CVector3::Up();
	CQuaternion m_rotOffset;
	CVector3 m_updatedTarget = CVector3::AxisZ(), m_updatedUp = CVector3::Up();
	CVector2 m_rot;
};

class HotokeCameraController : public IGameObject {
public:
	HotokeCameraController(CDeathHotoke* ptrHotoke, IGamePad* ptrPad) : m_ptrHotoke(ptrHotoke), m_ptrPad(ptrPad) {
		//マウスカーソルを中央に固定
		//MouseCursor().SetLockMouseCursor(true);
		m_lock = true;
		//マウスカーソルを非表示
		//MouseCursor().SetShowMouseCursor(false);
	}

	void Update()override;

	//メインカメラに設定
	void SetToMainCamera() {
		m_hotokeCam.SetToMainCamera();
	}

	//前方向を取得
	const CVector3& GetFront() {
		return m_hotokeCam.GetFront();
	}
	//消失点を取得
	CVector3 GetVanishingPoint() {
		return m_hotokeCam.GetVanishingPoint();
	}

	//座標を2D上の座標に変換
	CVector3 CalcScreenPosFromWorldPos(const CVector3& pos) {
		return m_hotokeCam.CalcScreenPosFromWorldPos(pos);
	}

private:
	HotokeCamera m_hotokeCam;

	bool m_isBackMirror = false;//バックミラー状態か？

	bool m_lock = false;//マウスカーソルの固定設定
	CVector2 m_mouseSensi = { 4.0f*(1.0f / 1280.0f),4.0f*(1.0f / 1280.0f) };//視点感度(マウス
	CVector2 m_padSensi = { 0.025f,-0.025f };//視点感度(パッド
	
	CDeathHotoke* m_ptrHotoke = nullptr;
	IGamePad* m_ptrPad = nullptr;
};