#pragma once
#include"IGamePad.h"
#include"AI.h"
#include"BodyPartsHeader.h"
#include"IFu.h"
#include"HUDFont.h"

class CDeathHotoke :
	public ILifeObject
{
public:
	//コンストラクタ
	CDeathHotoke(int playernum, IGamePad* ptrPad, bool isDrawHUD, HUDFont* ptrFont, std::unique_ptr<IAI> AI)
		: m_playerNum(playernum),m_ptrPad(ptrPad),m_isDrawHUD(isDrawHUD), m_ptrHUDFont(ptrFont),m_ai(std::move(AI))
	{
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
	void HUDRender(int)override;

	//移動量を加える
	void AddVelocity(const CVector3& vec) { m_veloxity += vec; }//TODO 高さで上昇量下げる?
	//回転量を加える
	void AddAngularVelocity(const CVector3& axis, float radAngle) { 
		m_angularVelocity = CQuaternion(axis,radAngle*GetRotatability()) * m_angularVelocity;
	}

	//抵抗を乗算
	void MulDrag(float mulDrag) { m_drag[enNext] *= mulDrag; }
	void MulAngularDrag(float mulDrag) { m_angularDrag[enNext] *= mulDrag; }

	//回転しやすさを乗算
	void MulRotatability(float rotatability) { m_rotatability[enNext] *= rotatability; }

	//ダメージをあたえる
	void Damage(const ReferenceCollision& ref, const CVector3& pos);

	//ターゲット位置を設定
	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }
	//ターゲット対象設定
	void SetTargetFu(IFu* target) { m_target = target; }

	//HUDを表示するか設定
	void SetIsDrawHUD(bool enable) { m_isDrawHUD = enable; }
	//バックミラー状態か設定
	void SetIsBackMirror(bool isMirror) { m_isBackMirror = isMirror; }
	
	//ゲッター//

	//高度を取得(メートル)
	float GetHeightMeter()const {
		return GetPos().y / METER;
	}

	//HPを取得
	float GetHP()const {
		return m_hp;
	}
	float GetHPMax()const {
		return HP_MAX;
	}
	//HPを取得(％で)
	float GetHPPer()const {
		return m_hp / HP_MAX;
	}

	//スタンしているか?
	bool GetIsStun()const {
		return m_stunTimeSec > 0.0f;
	}

	//スケールを取得
	[[nodiscard]] const CVector3& GetScale()const { return m_scale; }
	//移動したベクトルを取得
	[[nodiscard]] CVector3 GetMove() const { return GetPos() - m_posOld; }
	//ベロシティを取得
	[[nodiscard]] const CVector3& GetVelocity() const { return m_veloxity; }
	//抵抗を取得
	[[nodiscard]] float GetDrag()const { return m_drag[enNow]; }
	//回転しやすさを取得
	[[nodiscard]] float GetRotatability()const { return m_rotatability[enNow]; }

	//ターゲット位置を取得
	[[nodiscard]] const CVector3& GetTargetPos() const { return m_targetPos; }
	//ターゲットを取得
	[[nodiscard]] const IFu* GetTarget() const { return m_target; }
	[[nodiscard]] IFu* GetTarget() { return m_target; }

	//パッドの取得
	[[nodiscard]] const IGamePad* GetPad()const { return m_ptrPad; }

	//フォントの取得
	[[nodiscard]] const HUDFont* GetFont()const { return m_ptrHUDFont; }

	//HUDを表示するか取得
	[[nodiscard]] bool GetIsDrawHUD()const { return m_isDrawHUD; }
	//バックミラー状態か設定
	[[nodiscard]] bool GetIsBackMirror()const { return m_isBackMirror; }

	//AIの生み出すステータスを取得
	[[nodiscard]] const AIStatus* GetAIStatus()const { if (m_ai) { return &m_ai->GetOutputStatus(); } return nullptr; }


	//落下速度を取得
	[[nodiscard]] constexpr float GetGravity()const { return GRAVITY; }

private:
	//スタン処理
	void Stun();

private:
	//重力定数
	static constexpr float GRAVITY = OneG;

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
	static constexpr float HP_MAX = 100.0f;
	float m_hp = HP_MAX;//ヘルス
	float m_stunTimeSec = 0.0f;//スタン時間
	enum{enNow,enNext};
	float m_drag[2] = { 1.0f,1.0f };//抵抗
	float m_angularDrag[2] = { 1.0f,1.0f };//回転抵抗
	float m_rotatability[2] = { 1.0f,1.0f };//回転しやすさ

	//パーツ
	std::unique_ptr<IBodyPart> m_parts[4];

	//プレイヤー番号
	int m_playerNum = -1;
	//ゲームパッド
	IGamePad* m_ptrPad = nullptr;
	//AI
	std::unique_ptr<IAI> m_ai;
		
	bool m_isDrawHUD = false;//HUDを表示するか
	HUDFont* m_ptrHUDFont = nullptr;//HUDフォント
	bool m_isBackMirror = false;//バックミラー状態

	//ターゲット
	IFu* m_target = nullptr;
	CVector3 m_targetPos;
};

