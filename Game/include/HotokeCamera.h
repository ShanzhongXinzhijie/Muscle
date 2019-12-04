#pragma once

#include"CDeathHotoke.h"

class HotokeCamera
{
public:
	HotokeCamera() {
		//初期化
		m_camera.SetFar(150000.0f);
		//m_camera.SetViewAngleDeg(30.0f);			
		m_target = { 0.0f,0.7f,1.0f }; m_target.Normalize();//注視点初期化
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
	//ターゲット座標オフセットを設定
	void SetTargetPosOffset(const CVector3& vec) {
		m_targetPosOffset = vec;
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
	void SetToMainCamera(int num) {
		GetCameraList().at(num) = &m_camera;
	}

	//位置を取得
	const CVector3& GetPos() {
		return m_camera.GetPos();
	}
	//回転を取得
	const CVector2& GetRot()const {
		return m_rot;
	}
	//前方向を取得
	const CVector3& GetFront() {
		return m_camera.GetFront();
	}
	//照準点を取得
	CVector3 GetTargetPoint() {
		return m_pos + m_targetPosOffset + m_updatedFireTarget * m_camera.GetFar();
	}
	//消失点を取得
	CVector3 GetVanishingPoint() {
		return m_pos + m_updatedTarget * m_camera.GetFar();
	}
	float GetFar()const {
		return m_camera.GetFar();
	}

	//カメラ更新
	void Update() {
		m_camera.SetPos(m_pos);
		m_camera.SetTarget(m_pos + m_updatedTarget * m_camera.GetFar());
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
		m_updatedFireTarget = m_fireTarget;

		CQuaternion cq;
		cq.SetRotation(CVector3::AxisX(), m_rot.y);
		cq.Concatenate(CQuaternion(CVector3::AxisY(), m_rot.x));
		cq.Concatenate(m_rotOffset);
		cq.Multiply(m_updatedTarget);
		cq.Multiply(m_updatedFireTarget);
		cq.Multiply(m_updatedUp);
	}

private:
	GameObj::PerspectiveCamera m_camera;

	CVector3 m_pos, m_targetPosOffset, m_target, m_fireTarget = CVector3::Front(), m_up = CVector3::Up();
	CQuaternion m_rotOffset;
	CVector3 m_updatedTarget = CVector3::AxisZ(), m_updatedFireTarget = CVector3::AxisZ(), m_updatedUp = CVector3::Up();
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
	void SetToMainCamera(int num) {
		GetCameraList().resize(2);
		m_hotokeCam.SetToMainCamera(num);
	}

	//位置を取得
	const CVector3& GetPos() {
		return m_hotokeCam.GetPos();
	}
	//前方向を取得
	const CVector3& GetFront() {
		return m_hotokeCam.GetFront();
	}
	//照準点を取得
	CVector3 GetTargetPoint() {
		return m_hotokeCam.GetTargetPoint();
	}
	CVector3 GetVanishingPoint() {
		return m_hotokeCam.GetVanishingPoint();
	}	
	float GetFar()const {
		return m_hotokeCam.GetFar();
	}

	//バックミラー常態か取得
	bool GetIsBackMirror()const { return m_isBackMirror; }

	//座標を2D上の座標に変換
	CVector3 CalcScreenPosFromWorldPos(const CVector3& pos) {
		return m_hotokeCam.CalcScreenPosFromWorldPos(pos);
	}

private:
	HotokeCamera m_hotokeCam;

	bool m_isBackMirror = false;//バックミラー状態か？

	bool m_lock = false;//マウスカーソルの固定設定
	CVector2 m_mouseSensi = { 4.0f*(1.0f / 1280.0f),4.0f*(1.0f / 1280.0f) };//視点感度(マウス
	CVector2 m_padSensi = { 0.05f,-0.05f };//視点感度(パッド

	float m_cameraHeight = 16.45f;//カメラの地面からの高さ
	
	CDeathHotoke* m_ptrHotoke = nullptr;
	IGamePad* m_ptrPad = nullptr;
};