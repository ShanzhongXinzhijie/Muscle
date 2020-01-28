#pragma once

class LoadingScreen : public IGameObject
{
public:
	LoadingScreen() = default;

	bool Start()override;
	void Update()override;
	void PostLoopUpdate()override;
	void PostRender()override;

private:
	std::unique_ptr<GameObj::NoRegisterOrthoCamera> m_gomiCamera;
	bool m_isOneLooped = false;
	bool m_enter = false;
	CFont m_font;
	CSprite m_sprite;
};

