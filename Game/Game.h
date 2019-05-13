#pragma once
#include"Shibuya.h"
#include"CDeathHotoke.h"
class Game
{
public:
	Game();
	~Game();
private:
	CDeathHotoke m_hotoke;
	Shibuya m_shibuya;	
	GameObj::CDirectionLight l;	
};

