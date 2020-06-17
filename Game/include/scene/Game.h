#pragma once
#include"Shibuya.h"
#include"CPlayer.h"
#include"CountDown.h"

class GameManager;

class Game : public IGameObject
{
public:
	Game(GameManager* manager);
	~Game() {
		//�������폜
		ICreateObject::AllDestroy();
		//�Q�Ɖ���
		GameSE::SetGamePtr(nullptr);
		//�����ނ���
		g_grassRunner.Disable();
	}

	void PreUpdate()override;
	void PostLoopUpdate()override;
	void PostRender()override;

	/// <summary>
	/// �v���C���[�̎Q�Ƃ��擾
	/// </summary>
	/// <param name="num">�v���C���[�ԍ�</param>
	const std::unique_ptr<CPlayer>& GetPlayer(int num)const{
		for (int i = 0; i < PLAYER_NUM; i++) {
			if (m_player[i]->GetPlayerNum() == num) {
				return m_player[i];
			}
		}
		DW_ERRORBOX(true,"Game::GetPlayer()�v���C���[��������Ȃ�")
		return m_player[0];
	}

private:
	//�v���C���[��
	int m_playerNum = 1;

	//�v���C���[
	std::unique_ptr<CPlayer> m_player[PLAYER_NUM];
	//�X�e�[�W
	std::unique_ptr<Shibuya> m_shibuya;

	//��������
	float m_timeLimitSec = 15.0f;
	//�I���܂ł̎���
	float m_toEndTimeSec = 0.0f;

	//�t�H���g
	HUDFont m_font;
	CFont m_bigFont;

	//�Q�[���}�l�[�W���[
	GameManager* m_manager = nullptr;

	//�I���{�^��
	int m_pushCnt = 0;
	int MAX_PUSH = 20;
	int m_pushCntNoCon = 0;
	int MAX_PUSH_NOCON = 20;
};

