#pragma once

/// <summary>
/// 操作入力クラス
/// </summary>
class IGamePad : public IGameObject
{
public:
	IGamePad(int num) {
		m_usePad = &Pad(num);
	}
	virtual ~IGamePad() {}

	void PreUpdate()override;

	/// <summary>
	/// スティック入力を取得
	/// </summary>
	/// <param name="lr">左右</param>
	[[nodiscard]] 
	const CVector2& GetStick(enLR lr) const {
		return m_usePad->GetStick(lr);
	}

	/// <summary>
	/// このフレームに弾き入力されたか
	/// </summary>
	/// <param name="lr">左右</param>
	[[nodiscard]]
	bool IsSmashInput(enLR lr)const {
		return m_isSmash[lr];
	}

	/// <summary>
	/// スティック入力の前フレームとの差角度を取得
	/// </summary>
	/// <param name="lr">左右</param>
	[[nodiscard]]
	float GetStickRollAngle(enLR lr) const {
		return m_stickRollAngle[lr];
	}

	//回転入力系
	enum enSpinDirection { enUp, enRight, enDown, enLeft, enDirNum, };//入力各方向
	enum enSpinMode { enNone, enClockwise, enBack, };//回転方向

	/// <summary>
	/// スティック回転数を取得
	/// </summary>
	/// <param name="lr">左右</param>
	[[nodiscard]]
	int GetStickCircleInput(enLR lr) const {
		return m_circleInputStatus[lr].m_spinCount / enDirNum;
	}

	/// <summary>
	/// このフレームでスティックが1回転したか?
	/// </summary>
	/// <param name="lr">左右</param>
	[[nodiscard]]
	bool GetIsStickCircleInputNow(enLR lr) const {
		return m_circleInputStatus[lr].m_spinCountBefore + enDirNum <= m_circleInputStatus[lr].m_spinCount;
	}

	/// <summary>
	/// スティックの回転方向(時計回りor反時計回り)を取得
	/// </summary>
	/// <param name="lr">左右</param>
	[[nodiscard]]
	enSpinMode GetStickCircleenSpinMode(enLR lr) const {
		return m_circleInputStatus[lr].m_spinMode;
	}

	/// <summary>
	/// 射撃入力を取得
	/// </summary>
	/// <param name="lr">左右</param>
	[[nodiscard]] 
	bool GetFire(enLR lr) const {
		bool isFire = m_usePad->GetButton((lr == L) ? enButtonLB : enButtonRB);
#ifndef DW_MASTER
		if (!isFire) {
			isFire = GetKeyInput((lr == L) ? VK_LBUTTON : VK_RBUTTON);
		}
#endif // DW_MASTER
		return isFire;
	}
	/// <summary>
	/// 射撃ダブルタップ入力を取得
	/// </summary>
	/// <param name="lr">左右</param>
	[[nodiscard]]
	bool GetDoubleTapFire(enLR lr) const{
		return m_isDoubleTapFire[lr];
	}

	/// <summary>
	/// 翼入力を取得
	/// </summary>
	[[nodiscard]]
	bool GetWingInput() const {
		return m_usePad->GetButton(enButtonLSB);
	}

	/// <summary>
	/// バックミラー入力を取得
	/// </summary>
	[[nodiscard]]
	bool GetBackMirrorDown() const {
		return m_usePad->GetDown(enButtonRSB);
	}

	/// <summary>
	/// 脚入力を取得
	/// </summary>
	[[nodiscard]]
	bool GetLegInput() const {
		return m_usePad->GetButton(enButtonLT) || m_usePad->GetButton(enButtonRT);
	}
	[[nodiscard]]
	bool GetLegDown() const {
		return m_usePad->GetDown(enButtonLT) || m_usePad->GetDown(enButtonRT);
	}
	//左右どちらか
	[[nodiscard]]
	bool GetLegInput(enLR lr) const {
		return m_usePad->GetButton(lr == L ? enButtonLT : enButtonRT);
	}
	//同時押し
	[[nodiscard]]
	bool GetDoubleLegDown() const {
		return (m_usePad->GetDown(enButtonLT) || m_usePad->GetDown(enButtonRT)) && (m_usePad->GetButton(enButtonLT) && m_usePad->GetButton(enButtonRT));
	}

private:
	XInputPad* m_usePad = nullptr;//使用するパッド

	//弾き入力
	bool m_isSmash[enLRNUM] = {};
	int m_smashInputStartTime[enLRNUM] = {};
	static constexpr float SMASH_INPUT_DEADZONE = 0.24f;//デッドゾーン
	static constexpr int SMASH_INPUT_TIME = 4;//入力猶予

	//角度入力系
	float m_stickRollAngle[enLRNUM] = {};//スティックの角度入力
	bool m_isInitBeforeStickInput = false;
	CVector2 m_beforeStickInput[enLRNUM];//前フレームのスティック入力
	static constexpr float ANGLE_INPUT_DEADZONE = 0.75f;//デッドゾーン

	//ダブルタップ系
	bool m_isDoubleTapFire[enLRNUM] = {};
	int m_doubleTapFirePhase[enLRNUM] = {};
	int m_doubleTapFireTimer[enLRNUM] = {};
	static constexpr int FIRE_DOUBLE_TAP_TIME = 20;//入力猶予

	//回転入力関係
	struct CircleInputStatus {
		float m_inputTimer = 0.0f;//入力猶予
		int m_spinCount = 0, m_spinCountBefore = 0;//回転カウント	
		enSpinDirection m_nextDirection = enDirNum;//次に入力する回転
		enSpinDirection m_backDirection = enDirNum;//次に入力する逆回転	
		enSpinMode m_spinMode = enNone;//回転モード
	};
	CircleInputStatus m_circleInputStatus[enLRNUM];
	static constexpr float CIRCLE_INPUT_DEADZONE = 0.75f;//デッドゾーン
	static constexpr float CIRCLE_INPUT_RANGE = CMath::PI2 / enDirNum / 2.0f;//各方向の入力範囲
	static constexpr float CIRCLE_INPUT_MAX_TIME = 30.0f;//入力猶予
};

