#include "stdafx.h"
#include "Game.h"


Game::Game()
{
	//���C�g�쐬
	l.SetDirection(CVector3::Down());
	l.SetColor(CVector3::One() * 0.5f);	
}

Game::~Game()
{
}
