#include "stdafx.h"
#include "Game.h"


Game::Game()
{
	//ライト作成
	l.SetDirection(CVector3::AxisZ()*-1.0f);
	l.SetColor(CVector3::One() * 0.5f);	
}

Game::~Game()
{
}
