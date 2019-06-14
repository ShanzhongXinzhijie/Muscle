#pragma once
#include"CDeathHotoke.h"
#include"CPlayerCamera.h"

class CPlayer
{
public:
	CPlayer();
	~CPlayer();

private:
	CDeathHotoke m_hotoke;
	CPlayerCamera m_cam;
};

