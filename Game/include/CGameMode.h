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

	void SwitchIsEasy() {
		m_isEasy = !m_isEasy;
	}
	bool GetIsEasy()const {
		return m_isEasy;
	}

	enum TreeNumMode {
		enZero,
		enLow,
		enHigh,
		enTreeNumModeNum
	};
	void SwitchTreeNum() {
		switch (m_treeNum)
		{
		case CGameMode::enZero:
			m_treeNum = enLow;
			break;
		case CGameMode::enLow:
			m_treeNum = enHigh;
			break;
		case CGameMode::enHigh:
			m_treeNum = enZero;
			break;
		default:
			break;
		}
	}
	TreeNumMode GetTreeNum()const {
		return m_treeNum;
	}

private:
	int m_playerNum = 0;
	bool m_isEasy = false;
	TreeNumMode m_treeNum = enHigh;
};

