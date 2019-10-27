#pragma once
#include"Shibuya.h"
#include"CPlayer.h"

class Game
{
public:
	Game():m_player1(1), m_player2(0) {
		CFont::LoadDefaultFont(L"Resource/font/eunomia_0200/Eunomia.spritefont");
	}
private:
	CPlayer m_player1, m_player2;
	Shibuya m_shibuya;	
};

