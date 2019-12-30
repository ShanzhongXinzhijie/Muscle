#pragma once

class CTitle :
	public IGameObject
{
	bool Start()override;
	void PostLoopUpdate()override;
	void PostRender()override;

private:
	CSprite m_sprite;
	CFont m_font;
};

