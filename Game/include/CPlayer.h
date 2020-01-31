#pragma once
#include"IGamePad.h"
#include"CDeathHotoke.h"
#include"HotokeCamera.h"
#include"ZoomOutCamera.h"
#include"HUDFont.h"
#include"TreeGene.h"
#include"AssembleScene.h"
#include"AI.h"

class HumanPlayer : public IGameObject {
public:
	HumanPlayer(int playernum, CDeathHotoke& hotoke);

	//IGameObject
	bool Start()override;
	void Update()override;
	void PostUpdate()override;
	void PostLoopUpdate()override;
	void HUDRender(int HUDNum)override;

	IGamePad& GetPad() {
		return m_pad;
	}

	HUDFont& GetHUDFont() {
		return m_HUDFont;
	}

private:
	int m_playerNum = -1;

	//�z�g�P�Q��
	CDeathHotoke& m_hotoke;

	//�J����
	HotokeCameraController m_cam;

	//�p�b�h
	IGamePad m_pad;

	//��
	Grass m_grass[Grass::m_sInstancingMax / PLAYER_NUM];

	//�e�X�g�p���f��
	AnimationClip m_anim, m_animHeri;
	GameObj::CSkinModelRender m_human, m_heri;
	GameObj::PerspectiveCamera m_humanCam;

	//HUD
	bool m_isDrawHUD = true;
	bool m_isLockon = false;
	float m_targetHP = 0.f;
	CVector3 m_targetPos;
	CVector4 m_HUDColor = { 0.0f,0.0f,0.0f,1.0f };
	CSprite m_guncross, m_wMark, m_velocityVector;
	//CVector3 m_guncrossPosOld, m_velocityPosOld;
	HUDFont m_HUDFont, m_warningFont, m_japaneseFont;
};

class CPlayer : public IGameObject
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="playernum">�v���C���[�ԍ�</param>
	CPlayer(int playernum);

	//IGameObject
	bool Start()override;

	/// <summary>
	/// �v���C���[�ԍ����擾
	/// </summary>
	int GetPlayerNum()const {
		return m_playerNum;
	}

	/// <summary>
	/// ����ł�?
	/// </summary>
	bool GetIsDeath()const {
		return m_hotoke.GetHP() <= 0.0f;
	}

private:
	//�v���C���[�ԍ�
	int m_playerNum = -1;

	//�l�ԗp�f�[�^
	std::unique_ptr<HumanPlayer> m_humanPlayer;
		
	HotokeAssembleManager::HotokeAssemble& m_assemble;//�A�Z���u���ݒ�
	CDeathHotoke m_hotoke;//�z�g�P�{��
};

