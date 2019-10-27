#pragma once
#include"IGamePad.h"
#include"AI.h"
#include"BodyPartsHeader.h"
#include"IFu.h"

class CDeathHotoke :
	public ILifeObject
{
public:
	//コンストラクタ
	CDeathHotoke(IGamePad* ptrPad, bool isDrawHUD, std::unique_ptr<IAI> AI):m_ptrPad(ptrPad),m_isDrawHUD(isDrawHUD),m_ai(std::move(AI)){
		SetName(L"CDeathHotoke");
	}

	//ボディパーツの種類
	enum enBodyParts {
		enWing, enLeg, enArm, enHead, enPartsNum,
	};
	//パーツ設定
	void SetBodyPart(enBodyParts partsType, std::unique_ptr<IBodyPart> part);

	//IGameObject関係
	bool Start()override;
	void PreUpdate()override;
	void Update()override;
	void PostLoopUpdate()override;
	void PostRender()override;

	//移動量を加える
	void AddVelocity(const CVector3& vec) { m_veloxity += vec; }
	//回転量を加える
	void AddAngularVelocity(const CVector3& axis, float radAngle) { 
		m_angularVelocity = CQuaternion(axis,radAngle*GetRotatability()) * m_angularVelocity;
	}

	//抵抗を乗算
	void MulDrag(float mulDrag) { m_drag *= mulDrag; }
	void MulAngularDrag(float mulDrag) { m_angularDrag *= mulDrag; }

	//回転しやすさを乗算
	void MulRotatability(float rotatability) { m_rotatability = rotatability; }

	//ダメージをあたえる
	void Damage(const ReferenceCollision& ref, const CVector3& pos);

	//ターゲット位置を設定
	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }

	//HUDを表示するか設定
	void SetIsDrawHUD(bool enable) { m_isDrawHUD = enable; }
	
	//ゲッター//

	//スケールを取得
	[[nodiscard]] const CVector3& GetScale()const { return m_scale; }
	//移動したベクトルを取得
	[[nodiscard]] CVector3 GetMove() const { return GetPos() - m_posOld; }
	//ベロシティを取得
	[[nodiscard]] const CVector3& GetVelocity() const { return m_veloxity; }
	//回転しやすさを取得
	[[nodiscard]] float GetRotatability()const { return m_rotatability; }
	//ターゲット位置を取得
	[[nodiscard]] const CVector3& GetTargetPos() const { return m_targetPos; }
	
	//パッドの取得
	[[nodiscard]] IGamePad* GetPad() { return m_ptrPad; }

	//HUDを表示するか取得
	[[nodiscard]] bool GetIsDrawHUD()const { return m_isDrawHUD; }

	//AIの生み出すステータスを取得
	[[nodiscard]] const AIStatus* GetAIStatus()const { if (m_ai) { return &m_ai->GetOutputStatus(); } return nullptr; }


	//落下速度を取得
	[[nodiscard]] float GetGravity()const { return GRAVITY; }

private:
	//重力定数
	static constexpr float GRAVITY = 10.0f;

	//コアのモデル
	GameObj::CSkinModelRender m_coreModel;
	
	//スケール
	CVector3 m_scale;

	//前フレームの位置
	CVector3 m_posOld;
	
	//移動量
	CVector3 m_veloxity;
	CQuaternion m_angularVelocity;

	//ステータス
	float m_hp = 100.0f;//ヘルス
	float m_drag = 1.0f;//抵抗
	float m_angularDrag = 1.0f;//回転抵抗
	float m_rotatability = 1.0f;//回転しやすさ

	//パーツ
	std::unique_ptr<IBodyPart> m_parts[4];

	//ゲームパッド
	IGamePad* m_ptrPad = nullptr;

	//HUDを表示するか
	bool m_isDrawHUD = false;

	//ターゲット位置
	CVector3 m_targetPos;

	//AI
	std::unique_ptr<IAI> m_ai;
};

