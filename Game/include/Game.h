#pragma once
#include"Shibuya.h"
#include"CPlayer.h"
#include"CountDown.h"

class GameManager;

class Game : public IGameObject
{
public:
	Game(GameManager* manager);

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
};

