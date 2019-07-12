#pragma once
class IGamePad : public IGameObject
{
public:
	IGamePad() = default;
	virtual ~IGamePad() {}

	void PreUpdate()override;

	[[nodiscard]] 
	const CVector2& GetStick(enLR lr) const {
		return Pad(0).GetStick(lr);
	}

	[[nodiscard]] 
	bool GetFire(enLR lr) const {
		bool isFire = Pad(0).GetButton((lr == L) ? enButtonLB : enButtonRB);
		if (!isFire) {
			isFire = GetKeyInput((lr == L) ? VK_LBUTTON : VK_RBUTTON);
		}
		return isFire;
	}
	[[nodiscard]]
	bool GetDoubleTapFire(enLR lr) const{
		return m_isDoubleTapFire[lr];
	}

private:
	//ダブルタップ系
	bool m_isDoubleTapFire[enLRNUM] = {};
	int m_doubleTapFirePhase[enLRNUM] = {};
	int m_doubleTapFireTimer[enLRNUM] = {};
	static constexpr int FIRE_DOUBLE_TAP_TIME = 10;
};

