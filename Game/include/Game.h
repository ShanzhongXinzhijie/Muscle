#pragma once
#include"Shibuya.h"
#include"CPlayer.h"

class Game
{
public:
	Game():m_player1(0){//, m_player2(1) {
	}
private:
	CPlayer m_player1;//, m_player2;
	Shibuya m_shibuya;	
};

