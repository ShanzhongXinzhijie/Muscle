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
	CDeathHotoke() {
		//検索用の名前登録
		SetName(L"CDeathHotoke");
		SetLockableObjectName(L"CDeathHotoke");
		//ロックオンされた際に参照されるHP変数を設定
		SetHPRef(&m_hp);
	}
	CDeathHotoke(int playernum, IGamePad* ptrPad, bool isDrawHUD, HUDFont* ptrFont, std::unique_ptr<IAI> AI){
		//検索用の名前登録
		SetName(L"CDeathHotoke");
		SetLockableObjectName(L"CDeathHotoke");
		//ロックオンされた際に参照されるHP変数を設定
		SetHPRef(&m_hp);		
		//初期化
		Init(playernum, ptrPad, isDrawHUD, ptrFont, std::move(AI));
	}

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="playernum">プレイヤー番号</param>
	/// <param name="ptrPad">パッドのポインタ</param>
	/// <param name="isDrawHUD">HUDを描画するか</param>
	/// <param name="ptrFont">HUD用フォントのポインタ</param>
	/// <param name="AI">AI</param>
	void Init(int playernum, IGamePad* ptrPad, bool isDrawHUD, HUDFont* ptrFont, std::unique_ptr<IAI> AI) {
		//初期化
		m_playerNum = playernum;
		m_ptrPad = ptrPad;
		m_isDrawHUD = isDrawHUD;
		m_ptrHUDFont = ptrFont;
		m_ai = std::move(AI);

		//初期位置
		SetPos(CVector3::AxisY()*2000.0f);
		if (m_playerNum == 0) {
			SetPos({ 0.0f, 1000.0f, -2400.0f*1.0f });
		}
		if (m_playerNum == 1) {
			SetPos({ 0.0f, 1000.0f, 2400.0f*1.0f });
			SetRot({ CVector3::AxisY(),CMath::PI });
		}
	}

	//フォントを設定
	void SetFonts(HUDFont* warningfont, HUDFont* japanesefont) {
		m_ptrWarningHUDFont = warningfont;
		m_ptrJapaneseHUDFont = japanesefont;
	}

	//ボディパーツの種類
	enum enBodyParts {
		enHead, enArm, enWing, enLeg, enPartsNum,
	};
	//パーツ設定
	void SetBodyPart(enBodyParts partsType, std::unique_ptr<IBodyPart> part);
	//パーツを取得
	const IBodyPart& GetBodyPart(enBodyParts partsType)const {
		return *m_parts[partsType].get();
	}

	//座標から足元までの距離を設定
	void SetToFootDistance(float distance) {
		m_toFootDistance = distance;
	}
	//足元カメラオフセット座標
	void SetFootCameraOffsetPos(const CVector3& offsetpos) {
		m_footCamOffsetPos = offsetpos;
	}
	//固定カメラを使用するかのフラグを設定
	void SetUseFixedCamera(bool enable) {
		m_useFixedCamera = enable;
	}
	//固定カメラの位置を設定
	void SetFixedCameraPoint(const CVector3& pos) {
		m_fixedCameraPos = pos;
	}

	//振動値を設定
	void SetShakePower(float shakePower) {
		m_shakePower = shakePower;
	}

	//操作可能かどうか設定
	void SetIsControl(bool isControl) {
		m_isControl = isControl;
	}

	//IGameObject関係
	bool Start()override;
	void PreLoopUpdate()override;
	void PreUpdate()override;
	void Update()override;
	void PostLoopUpdate()override;
	void Pre3DRender(int)override;
	void HUDRender(int)override;

	//移動量を加える
	void AddVelocity(const CVector3& vec) { m_veloxity += vec; }
	void AddLinearVelocity(const CVector3& vec) { m_linearVelocity += vec; }
	//各要素の絶対値が大きい方をセット
	void SetMaxLinearVelocity(const CVector3& vec){
		m_linearVelocity.x = abs(m_linearVelocity.x) > abs(vec.x) ? m_linearVelocity.x : vec.x;
		m_linearVelocity.y = abs(m_linearVelocity.y) > abs(vec.y) ? m_linearVelocity.y : vec.y;
		m_linearVelocity.z = abs(m_linearVelocity.z) > abs(vec.z) ? m_linearVelocity.z : vec.z;
	}
	//回転量を加える
	void AddAngularVelocity(const CVector3& axis, float radAngle) { 
		m_angularVelocity = CQuaternion(axis,radAngle*GetRotatability()) * m_angularVelocity;
	}

	//抵抗を乗算
	void MulDrag(float mulDrag) { m_drag[enNext] *= mulDrag; }
	void MulAngularDrag(float mulDrag) { m_angularDrag[enNext] *= mulDrag; }

	//回転しやすさを乗算
	void MulRotatability(float rotatability) { m_rotatability[enNext] *= rotatability; }

	//不死設定
	void SetIsImmortal(bool isImmortal) {
		m_isImmortal = isImmortal;
	}

	//無敵状態化
	void Invincible() {
		if (m_isInvincible) {
			return;
		}
		m_isInvincible = true;
		SetCollisionEnable(false);
		m_coreModel.GetSkinModel().FindMaterialSetting(
			[](MaterialSetting* mat) {
				mat->SetAlbedoScale({5.0f,5.0f,5.0f,1.0f});// { 1.0f, 0.01f, 0.08f, 0.9f });
			}
		);
	}

	//ダメージをあたえる
	void Damage(const ReferenceCollision& ref, const CVector3& pos);

	//ターゲット位置を設定
	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }
	//ターゲット対象設定
	void SetTarget(LockableWrapper* target) { m_target = target; }
	//消失点を設定
	void SetVanisingPoint(const CVector3& pos) { m_vanisingPoint = pos; }

	//HUDを表示するか設定
	void SetIsDrawHUD(bool enable) { m_isDrawHUD = enable; }
	//バックミラー状態か設定
	void SetIsBackMirror(bool isMirror) { m_isBackMirror = isMirror; }

	//光背を表示するか設定
	void SetIsDrawKouhai(bool isDraw) {
		m_isDrawKouhai = isDraw;
	}
	
	//ゲッター//

	//座標から足元までの距離
	float GetToFootDistance()const {
		return m_toFootDistance;
	}
	//足元の座標を取得
	CVector3 GetFootPos()const {
		return GetPos() + CVector3::Down()*GetToFootDistance();
	}
	//足元カメラオフセット座標
	const CVector3& GetFootCameraOffsetPos()const {
		return m_footCamOffsetPos;
	}
	//固定カメラを使用するか
	bool GetUseFixedCamera()const {
		return m_useFixedCamera;
	}
	//固定カメラの位置
	const CVector3& GetFixedCameraPoint()const {
		return m_fixedCameraPos;
	}

	//振動値を取得
	float GetShakePower()const {
		return m_shakePower;
	}

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
		return !(m_stunTimeSec < FLT_EPSILON);
	}
	//

	//スケールを取得
	[[nodiscard]] const CVector3& GetScale()const { 
		return m_scale;
	}
	//移動したベクトルを取得
	[[nodiscard]] CVector3 GetMove() const { 
		return GetPos() - m_posOld; 
	}
	//ベロシティを取得
	[[nodiscard]] CVector3 GetTotalVelocity() const {
		return m_veloxity + m_linearVelocity;
	}
	//抵抗を取得
	[[nodiscard]] float GetDrag()const {
		return m_drag[enNow]; 
	}
	//回転しやすさを取得
	[[nodiscard]] float GetRotatability()const {
		return m_rotatability[enNow];
	}

	//ターゲット位置を取得
	[[nodiscard]] const CVector3& GetTargetPos() const {
		return m_targetPos; 
	}
	//ターゲットを取得
	[[nodiscard]] const LockableWrapper* GetTarget() const {
		return m_target;
	}
	[[nodiscard]] LockableWrapper* GetTarget() {
		return m_target;
	}
	[[nodiscard]] const IFu* GetTargetFu() const { 
		if (m_target) {
			return m_target->GetFu();
		}
		else {
			return nullptr;
		}
	}
	[[nodiscard]] IFu* GetTargetFu() { 
		if (m_target) {
			return m_target->GetFu();
		}
		else {
			return nullptr;
		}
	}
	//消失点を取得
	[[nodiscard]] const CVector3& GetVanisingPoint() const { return m_vanisingPoint; }

	//プレイヤー番号の取得
	[[nodiscard]] int GetPlayerNum() { return m_playerNum; }

	//パッドの取得
	[[nodiscard]] const IGamePad* GetPad()const { return m_ptrPad; }

	//フォントの取得
	[[nodiscard]] HUDFont* GetFont()const { return m_ptrHUDFont; }
	[[nodiscard]] HUDFont* GetWarningFont()const{ return m_ptrWarningHUDFont ? m_ptrWarningHUDFont : m_ptrHUDFont; }
	[[nodiscard]] HUDFont* GetJapaneseFont()const { return m_ptrJapaneseHUDFont ? m_ptrJapaneseHUDFont : m_ptrHUDFont; }

	//HUDを表示するか取得
	[[nodiscard]] bool GetIsDrawHUD()const { return m_isDrawHUD; }
	//バックミラー状態か設定
	[[nodiscard]] bool GetIsBackMirror()const { return m_isBackMirror; }

	//AIの生み出すステータスを取得
	[[nodiscard]] const AIStatus* GetAIStatus()const { if (m_ai) { return &m_ai->GetOutputStatus(); } return nullptr; }

	//ズームアウト方向を取得
	const CVector3& GetZoomoutDirection()const {
		return m_zoomoutDir;
	}

	//落下速度を取得
	[[nodiscard]] constexpr float GetGravity()const { return GRAVITY; }

	//操作可能か取得
	[[nodiscard]] bool GetIsControl()const {
		return m_isControl;
	}

private:
	//スタン処理
	void Stun();

private:
	//重力定数
	static constexpr float GRAVITY = ONE_G;

	//コアのモデル
	GameObj::CSkinModelRender m_coreModel;
	//光背
	CBillboard m_kouhai;
	bool m_isDrawKouhai = true;
	
	//スケール
	CVector3 m_scale;

	//足元への距離
	float m_toFootDistance = 205.0f;
	//足元カメラ
	CVector3 m_footCamOffsetPos;
	//固定カメラ
	bool m_useFixedCamera = false;	//使うか
	CVector3 m_fixedCameraPos;		//位置

	//振動値
	float m_shakePower = 0.0f;

	//前フレームの位置
	CVector3 m_posOld;
	
	//移動量
	CVector3 m_veloxity, m_linearVelocity;
	CQuaternion m_angularVelocity;

	//ステータス
	bool m_isImmortal = false;//不死か
	bool m_isInvincible = false;//無敵状態
	static constexpr float HP_MAX = 20.0f;
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
		
	//HUD
	bool m_isDrawHUD = false;//HUDを表示するか
	HUDFont* m_ptrHUDFont = nullptr, *m_ptrWarningHUDFont = nullptr, *m_ptrJapaneseHUDFont = nullptr;//HUDフォント
	bool m_isBackMirror = false;//バックミラー状態

	//ターゲット
	LockableWrapper* m_target = nullptr;
	CVector3 m_targetPos;
	CVector3 m_vanisingPoint;

	//ズームアウト方向
	CVector3 m_zoomoutDir = { 0.f, 400.f, 800.f };

	//操作可能か?
	bool m_isControl = false;
};

