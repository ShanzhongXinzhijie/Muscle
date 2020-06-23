#pragma once

class CGameMode :
	public IGameObject
{
public:
	CGameMode() {
		SetName(L"CGameMode");
	}

	void Update()override {
		//デバッグ表示切り抱え
		if (GetKeyDown(VK_TAB)) {
			SetIsDebugDraw(!GetIsDebugDraw());
		}
		//ゲーム終了
		if (GetKeyInput(VK_ESCAPE)) {
			BreakGameLoop();
		}
	}

	void SetPlayerNum(int num) {
		m_playerNum = num;
	}
	int GetPlayerNum()const {
		return m_playerNum;
	}

private:
	int m_playerNum = 0;
};

