#pragma once

#include"CPlayer.h"

//CPlayerを操作するクラス
class CPlayerController : public IGameObject
{
public:
	CPlayerController(CPlayer* ptrPlayer);
	~CPlayerController();

	void Update()override;

private:
	CPlayer* m_ptrPlayer = nullptr;
};

