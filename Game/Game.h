#pragma once
#include"Shibuya.h"
#include"Camera.h"

class Game
{
public:
	Game();
	~Game();
private:
	Camera m_cam;
	Shibuya m_shibuya;	
	GameObj::CDirectionLight l;	
};

