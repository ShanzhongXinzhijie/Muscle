#pragma once

#include"CDeathHotoke.h"
#include"TimeManager.h"

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
	void ResetMotionBlur() { 
		m_camera.ResetIsFirstMatrixUpdate(); 
	}

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
		//座標アップデート
		//UpdateVector();
	}

	//メインカメラに設定
	void SetToMainCamera() {
		SetMainCamera(&m_camera);
	}
	void SetToMainCamera(int num) {
		SetCameraToList(num, &m_camera);
	}

	//カメラを取得
	GameObj::PerspectiveCamera& GetCamera() {
		return m_camera;
	}
	//設定類を取得
	const CVector3& GetTargetSetting()const {
		return m_target;
	}
	const CVector3& GetUpSetting()const {
		return m_up;
	}

	//位置を取得
	const CVector3& GetPos()const {
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
	CVector3 GetTargetPoint() const {
		return m_pos + m_targetPosOffset + m_updatedFireTarget * m_camera.GetFar();
	}
	//消失点を取得
	CVector3 GetVanishingPoint()const {
		return m_pos + m_updatedTarget * m_camera.GetFar();
	}
	//Farを取得
	float GetFar()const {
		return m_camera.GetFar();
	}
	
	//座標を2D上の座標に変換
	CVector3 CalcScreenPosFromWorldPos(const CVector3& pos) {
		return m_camera.CalcScreenPosFromWorldPos(pos);
	}

	//カメラ更新
	void Update() {
		m_camera.SetPos(m_pos);
		m_camera.SetTarget(m_pos + m_updatedTarget * m_camera.GetFar());
		m_camera.SetUp(m_updatedUp);
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

	CVector3 m_pos, m_targetPosOffset;
	CVector3 m_target, m_fireTarget = CVector3::Front(), m_up = CVector3::Up();//設定
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
		MouseCursor().SetShowMouseCursor(false);
	}

	//毎フレームの処理
	void Update()override;

	//メインカメラに設定
	void SetToMainCamera() {
		if (m_isZoomOut) {
			SetMainCamera(&m_zoomOutCam);
		}
		else {
			m_hotokeCam.SetToMainCamera();
		}
	}
	void SetToMainCamera(int num) {
		if (m_isZoomOut) {
			SetCameraToList(num, &m_zoomOutCam);
		}
		else {
			m_hotokeCam.SetToMainCamera(num);
		}
	}

	//ズームアウトモードかどうか設定
	void SetIsZoomout(bool isZoomOut, const CVector3& zoomoutDir = { 0.f, 400.f, 800.f }, const IFu* target = nullptr) {
		m_zoomOutDir = zoomoutDir;
		m_zoomOutTarget = target;

		//カメラ変更
		bool isChangeCam = false;
		if (m_isZoomOut != isZoomOut && isZoomOut) {
			isChangeCam = true;
		}

		if (!m_isZoomOut && isZoomOut) {
			//カメラ更新(初期化)
			UpdateZoomOutCamera(true);
		}

		//設定
		m_isZoomOut = isZoomOut;
		//カメラ変更
		if (isChangeCam) {
			ChangeCamera();
		}
	}

	//カメラを取得
	/*const GameObj::PerspectiveCamera& GetCamera()const {
		return m_hotokeCam.GetCamera();
	}*/

	//位置を取得
	const CVector3& GetPos()const {
		return m_hotokeCam.GetPos();
	}
	//前方向を取得
	const CVector3& GetFront() {
		return m_hotokeCam.GetFront();
	}
	//照準点を取得
	CVector3 GetTargetPoint()const {
		return m_hotokeCam.GetTargetPoint();
	}
	CVector3 GetVanishingPoint()const {
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
	void ChangeCamera();
	void UpdateZoomOutCamera(bool isInit);

private:
	//カメラ
	HotokeCamera m_hotokeCam;//ホトケ視点
	GameObj::PerspectiveCamera m_zoomOutCam;//ズームアウト用カメラ

	//ズームアウト
	bool m_isZoomOut = false;
	CVector3 m_zoomOutDir;
	const IFu* m_zoomOutTarget = nullptr;
	float m_zoomPercent = 0.0f;
	CVector3 m_originalPos, m_originalTarget;

	//操作
	bool m_isBackMirror = false;//バックミラー状態か？
	bool m_lock = false;//マウスカーソルの固定設定
	CVector2 m_mouseSensi = { 4.0f*(1.0f / 1280.0f),4.0f*(1.0f / 1280.0f) };//視点感度(マウス
	CVector2 m_padSensi = { 0.05f,-0.05f };//視点感度(パッド

	//パラメータ
	static constexpr float m_cameraHeight = 16.45f;//カメラの地面からの高さ(人間の身長)
	
	//参照
	CDeathHotoke* m_ptrHotoke = nullptr;
	IGamePad* m_ptrPad = nullptr;
};

/*
class ForcusPoint {
	//シングルトン
private:
	ForcusPoint() = default;
	~ForcusPoint() = default;
public:
	ForcusPoint(const ForcusPoint&) = delete;
	ForcusPoint& operator=(const ForcusPoint&) = delete;
	ForcusPoint(ForcusPoint&&) = delete;
	ForcusPoint& operator=(ForcusPoint&&) = delete;

public:
	static inline ForcusPoint& GetInstance() {
		static ForcusPoint instance;
		return instance;
	}

	void Enable(const CVector3& zoomoutDir = { 0.f, 400.f, 800.f }, const IFu* target = nullptr) {

	}
	void Disable() {
		m_isEnable = false;
	}

private:
	bool m_isEnable = false;
	CVector3 m_zoomOutDir;
	const IFu* m_zoomOutTarget = nullptr;
};
*/