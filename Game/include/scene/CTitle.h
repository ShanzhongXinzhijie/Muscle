#pragma once

/// <summary>
/// タイトルシーン
/// </summary>
class CTitle :
	public IGameObject
{
	bool Start()override;
	void Update()override;
	void PostLoopUpdate()override;
	void PostRender()override;

private:
	CSprite m_sprite;
	CFont m_font;

	bool m_enter = false;
	int m_selectMode = 0;
};

