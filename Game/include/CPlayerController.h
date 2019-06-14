#pragma once

#include"CPlayer.h"

//CPlayer‚ğ‘€ì‚·‚éƒNƒ‰ƒX
class CPlayerController : public IGameObject
{
public:
	CPlayerController(CPlayer* ptrPlayer);
	~CPlayerController();

	void Update()override;

private:
	CPlayer* m_ptrPlayer = nullptr;
};

