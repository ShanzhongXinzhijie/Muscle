#pragma once
#include"Shibuya.h"
#include"CPlayer.h"

class Game
{
public:
	Game();
	~Game();
private:
	CPlayer m_player;
	Shibuya m_shibuya;	
};

