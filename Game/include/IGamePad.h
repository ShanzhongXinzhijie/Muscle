#pragma once
class IGamePad
{
public:
	IGamePad() = default;
	virtual ~IGamePad() {}

	const CVector2& GetStick(enLR lr) {
		return Pad(0).GetStick(L);
	}
};

