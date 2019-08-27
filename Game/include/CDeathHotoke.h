#pragma once
#include"IGamePad.h"

class IBodyPart;

class CDeathHotoke :
	public IGameObject
{
public:
	//コンストラクタ
	CDeathHotoke(IGamePad* ptrPad, bool isDrawHUD):m_ptrPad(ptrPad),m_isDrawHUD(isDrawHUD){}

	//IGameObject関係
	bool Start()override;
	void Update()override;
	void PostRender()override;

	//移動量を加える
	void AddMove(const CVector3& vec) { m_move += vec; }
	void AddRot(const CQuaternion& rot) { m_rotMove = rot * m_rotMove; }

	//セッター
	void SetPos(const CVector3& pos) { m_pos = pos; }
	void SetRot(const CQuaternion& rot) { m_rot = rot; }
	//ターゲット位置を設定
	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }
	
	//ゲッター
	const CVector3& GetPos()const { return m_pos; }	
	const CQuaternion& GetRot()const { return m_rot; }
	const CVector3& GetScale()const { return m_scale; }
	//移動量を取得
	const CVector3& GetMove() const { return m_move; }
	//ターゲット位置を取得
	const CVector3& GetTargetPos() const { return m_targetPos; }
	//前ベクトルを取得
	[[nodiscard]] const CVector3& GetFront()const { return m_front; }
	//左ベクトルを取得
	[[nodiscard]] const CVector3& GetLeft()const { return m_left; }
	//左ベクトルを取得
	[[nodiscard]] const CVector3& GetUp()const { return m_up; }

	//パッドの取得
	IGamePad* GetPad() { return m_ptrPad; }

	//HUDを表示するか取得
	[[nodiscard]] bool GetIsDrawHUD()const { return m_isDrawHUD; }

private:
	//コアのモデル
	GameObj::CSkinModelRender m_coreModel;

	//コリジョン
	DHCollision m_col;

	//TRS
	CVector3 m_pos, m_scale;
	CQuaternion m_rot;

	//方向ベクトル
	CVector3 m_front = CVector3::Front(), m_left = CVector3::Left(), m_up = CVector3::Up();

	//移動量
	CVector3 m_move;
	CQuaternion m_rotMove;

	//パーツ
	IBodyPart* m_parts[4] = {};

	//ゲームパッド
	IGamePad* m_ptrPad = nullptr;

	//HUDを表示するか
	bool m_isDrawHUD = false;

	//ターゲット位置
	CVector3 m_targetPos;

public:
	//ボディパーツの種類
	enum enBodyParts {
		enWing, enLeg, enArm, enHead, enPartsNum, 
	};
};

