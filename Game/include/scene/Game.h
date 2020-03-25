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
	}

	void PreUpdate()override;
	void PostRender()override;

private:
	//�v���C���[
	std::unique_ptr<CPlayer> m_player[PLAYER_NUM];
	//�X�e�[�W
	std::unique_ptr<Shibuya> m_shibuya;

	//��������
	float m_timeLimitSec = 15.0f;

	//�t�H���g
	HUDFont m_font;

	//�Q�[���}�l�[�W���[
	GameManager* m_manager = nullptr;

	int m_pushCnt = 0;
	int MAX_PUSH = 20;
	int m_pushCntNoCon = 0;
	int MAX_PUSH_NOCON = 20;
};

