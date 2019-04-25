#include "stdafx.h"
#include "Game.h"


Game::Game()
{
	//���C�g�쐬
	l.SetDirection(CVector3::Down());
	l.SetColor(CVector3::One() * 0.5f);

	//�J�����쐬
	SetMainCamera(&cam);
	cam.SetPos(CVector3::Up() * 200.0f + CVector3::AxisZ() * -400.0f);
}

Game::~Game()
{
}
