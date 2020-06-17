#pragma once

class CGameMode :
	public IGameObject
{
public:
	CGameMode() {
		SetName(L"CGameMode");
	}

	void PostLoopUpdate()override {
		//ÉQÅ[ÉÄèIóπ
		if (GetAsyncKeyState(VK_ESCAPE)) {
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

