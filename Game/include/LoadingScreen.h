#pragma once
#include "Game.h"

class LoadingScreen : public IGameObject
{
public:
	LoadingScreen() = default;

	bool Start()override;
	void PostLoopUpdate()override;
	void PostRender()override;

private:
	std::unique_ptr<GameObj::NoRegisterOrthoCamera> m_gomiCamera;
	bool m_isOneLooped = false;
	CFont m_font;
	CSprite m_sprite;

	std::unique_ptr<Game> m_game;
};

