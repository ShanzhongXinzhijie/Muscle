#pragma once
class IGamePad
{
public:
	IGamePad() = default;
	virtual ~IGamePad() {}

	[[nodiscard]] const CVector2& GetStick(enLR lr) {
		return Pad(0).GetStick(L);
	}

	[[nodiscard]] bool GetFire(enLR lr) {
		bool isFire = Pad(0).GetButton((lr == L) ? enButtonLB : enButtonRB);
		if (!isFire) {
			isFire = GetKeyInput((lr == L) ? VK_LBUTTON : VK_RBUTTON);
		}
		return isFire;
	}
};

