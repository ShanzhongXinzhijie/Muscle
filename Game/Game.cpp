#include "stdafx.h"
#include "Game.h"


Game::Game()
{
	//ƒ‰ƒCƒgì¬
	l.SetDirection(CVector3::Down());
	l.SetColor(CVector3::One() * 0.5f);

	//ƒJƒƒ‰ì¬
	SetMainCamera(&cam);
	cam.SetPos(CVector3::Up() * 200.0f + CVector3::AxisZ() * -400.0f);
}

Game::~Game()
{
}
