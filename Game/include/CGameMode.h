#pragma once

class CGameMode :
	public IGameObject
{
public:
	CGameMode() {
		SetName(L"CGameMode");
	}

	void Update()override {
		//�f�o�b�O�\���؂����
		if (GetKeyDown(VK_TAB)) {
			SetIsDebugDraw(!GetIsDebugDraw());
		}
		//�Q�[���I��
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

