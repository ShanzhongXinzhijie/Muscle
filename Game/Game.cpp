#include "stdafx.h"
#include "Game.h"


Game::Game()
{
	//ライト作成
	l.SetDirection(CVector3::Down());
	l.SetColor(CVector3::One() * 0.5f);	
}

Game::~Game()
{
}
