#pragma once

#include"CPlayer.h"

//CPlayer�𑀍삷��N���X
class CPlayerController : public IGameObject
{
public:
	CPlayerController(CPlayer* ptrPlayer);
	~CPlayerController();

	void Update()override;

private:
	CPlayer* m_ptrPlayer = nullptr;
};

