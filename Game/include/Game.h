#pragma once
#include"Shibuya.h"
#include"CPlayer.h"
#include"CountDown.h"

class Game
{
public:
	Game():m_player1(1), m_player2(0) {
		new CountDown();
	}
private:
	CPlayer m_player1, m_player2;
	Shibuya m_shibuya;	
};

