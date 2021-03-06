#pragma once

class WindowSizeManager;
class CGameMode;

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

	int m_pushCnt = 0;
	int MAX_PUSH = 6;

	WindowSizeManager* m_ptrWinSizeMane = nullptr;
	CGameMode* m_ptrGameMode = nullptr;
};

