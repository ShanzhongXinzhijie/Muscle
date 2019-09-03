#pragma once
#include"IGamePad.h"
#include"AI.h"

class IBodyPart;

class CDeathHotoke :
	public IGameObject
{
public:
	//コンストラクタ
	CDeathHotoke(IGamePad* ptrPad, bool isDrawHUD, std::unique_ptr<IAI> AI):m_ptrPad(ptrPad),m_isDrawHUD(isDrawHUD),m_ai(std::move(AI)){
		SetName(L"CDeathHotoke");
	}

	//IGameObject関係
	bool Start()override;
	void Update()override;
	void PostRender()override;

	//移動量を加える
	void AddMove(const CVector3& vec) { m_move += vec; }
	void AddRot(const CQuaternion& rot) { m_rotMove = rot * m_rotMove; }

	//ダメージをあたえる
	void Damage(const ReferenceCollision& ref);

	//セッター
	void SetPos(const CVector3& pos) { m_pos = pos; }
	void SetRot(const CQuaternion& rot) { m_rot = rot; }
	//ターゲット位置を設定
	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }
	
	//ゲッター
	[[nodiscard]] const CVector3& GetPos()const { return m_pos; }
	[[nodiscard]] const CQuaternion& GetRot()const { return m_rot; }
	[[nodiscard]] const CVector3& GetScale()const { return m_scale; }
	//移動量を取得
	[[nodiscard]] const CVector3& GetMove() const { return m_move; }
	//ターゲット位置を取得
	[[nodiscard]] const CVector3& GetTargetPos() const { return m_targetPos; }
	
	//前ベクトルを取得
	[[nodiscard]] const CVector3& GetFront()const { return m_front; }
	//後ベクトルを取得
	[[nodiscard]] const CVector3& GetBack()const { return m_back; }
	//左ベクトルを取得
	[[nodiscard]] const CVector3& GetLeft()const { return m_left; }
	//右ベクトルを取得
	[[nodiscard]] const CVector3& GetRight()const { return m_right; }
	//上ベクトルを取得
	[[nodiscard]] const CVector3& GetUp()const { return m_up; }
	//下ベクトルを取得
	[[nodiscard]] const CVector3& GetDown()const { return m_down; }

	//パッドの取得
	[[nodiscard]] IGamePad* GetPad() { return m_ptrPad; }

	//HUDを表示するか取得
	[[nodiscard]] bool GetIsDrawHUD()const { return m_isDrawHUD; }

	//AIの生み出すステータスを取得
	[[nodiscard]] const AIStatus* GetAIStatus()const { if (m_ai) { return &m_ai->GetOutputStatus(); } return nullptr; }

private:
	//方向ベクトルを計算・更新
	void CalcDirection();

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
	CVector3 m_back = CVector3::Back(), m_right = CVector3::Right(), m_down = CVector3::Down();

	//移動量
	CVector3 m_move;
	CQuaternion m_rotMove;

	//ステータス
	float m_hp = 100.0f;

	//パーツ
	IBodyPart* m_parts[4] = {};

	//ゲームパッド
	IGamePad* m_ptrPad = nullptr;

	//HUDを表示するか
	bool m_isDrawHUD = false;

	//ターゲット位置
	CVector3 m_targetPos;

	//AI
	std::unique_ptr<IAI> m_ai;

public:
	//ボディパーツの種類
	enum enBodyParts {
		enWing, enLeg, enArm, enHead, enPartsNum, 
	};
};

