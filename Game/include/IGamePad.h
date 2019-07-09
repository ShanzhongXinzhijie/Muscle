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
		if (lr == L) {
			return Pad(0).GetButton(enButtonLB);
		}
		else {
			return Pad(0).GetButton(enButtonRB);
		}
	}
};

