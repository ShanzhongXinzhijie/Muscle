#pragma once
#include"Shibuya.h"

class Game
{
public:
	Game();
	~Game();
private:
	Shibuya m_shibuya;
	GameObj::PerspectiveCamera cam;
	GameObj::CDirectionLight l;

};

