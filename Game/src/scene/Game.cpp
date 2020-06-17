#include "stdafx.h"
#include "Game.h"
#include "GameManager.h"
#include "CGameMode.h"

Game::Game(GameManager* manager) 
	: m_manager(manager), m_timeLimitSec(static_cast<float>(manager->GetTimeLimitSec()) + 0.9f)
{
	//SE�ɎQ�Ƃ�n��
	GameSE::SetGamePtr(this);

	//�J�E���g�_�E���N���X����
	int scores[PLAYER_NUM] = { m_manager->GetPlayerScore(0),m_manager->GetPlayerScore(1) };
	new CountDown(m_manager->GetRoundCount(), m_manager->GetMaxRound(), scores, m_manager->GetTimeLimitSec());

	//�v���C���[����
	m_player[0] = std::make_unique<CPlayer>(1, m_manager->GetIsPracticeRound());
	m_player[1] = std::make_unique<CPlayer>(0, m_manager->GetIsPracticeRound());

	//��
	g_grassRunner.Init();

	//���K���E���h�Ȃ�
	if (m_manager->GetIsPracticeRound()) {
		for (auto& player : m_player) {
			//�s���ݒ�
			player->GetDeathHotoke().SetIsImmortal(true);
			//��������\��
			player->GetDeathHotoke().SetIsDrawHowto(true);
		}
	}

	//�X�e�[�W����
	m_shibuya = std::make_unique<Shibuya>();

	//�t�H���g������
	//m_font.SetColor(CVector4::Black());
	m_bigFont.LoadFont(L"Resource/font/eunomia_0200/EunomiaBold.spritefont");

	//�v���C���[�l���̎擾
	m_playerNum = FindGO<CGameMode>(L"CGameMode")->GetPlayerNum();
	if (m_playerNum == 1) {
		MAX_PUSH = 6;
		MAX_PUSH_NOCON = 6;
	}
}

void Game::PreUpdate() {
	if (m_toEndTimeSec < FLT_EPSILON) {
		//�v���C���[�̎��S�����m
		bool isDeath = false;
		bool isDeathPlayer[PLAYER_NUM] = {};
		for (int i = 0; i < PLAYER_NUM; i++) {
			if (m_player[i]->GetIsDeath()) {
				isDeath = true;
				isDeathPlayer[m_player[i]->GetPlayerNum()] = true;
			}
		}
		//�v���C���[���S�ŃQ�[���I��
		if (isDeath) {
			//�b��Ƀ��E���h�I��
			m_toEndTimeSec = 2.3f;
			//�X���[���[�V����
			TimeManager::GetInstance().SetFrameRate(1.4f, 6, 1.0f);
			//SE
			SuicideObj::CSE* se = NewGO<SuicideObj::CSE>(L"Resource/sound/Pickup_Coin2.wav");
			se->Play();
			//���G��
			for (auto& player : m_player) {
				player->GetDeathHotoke().SetIsImmortal(true);
			}
		}

		//��������
		m_timeLimitSec -= FRAME_PER_SECOND;
		if (m_timeLimitSec < 0.0f) {
			//�b��Ƀ��E���h�I��
			m_toEndTimeSec = 2.3f;
			//�X���[���[�V����
			TimeManager::GetInstance().SetFrameRate(1.4f, 6, 1.0f);
			//SE
			SuicideObj::CSE* se = NewGO<SuicideObj::CSE>(L"Resource/sound/Randomize.wav");
			se->Play();			
			//���G��
			for (auto& player : m_player) {
				player->GetDeathHotoke().SetIsImmortal(true);
			}
			return;
		}
	}

	//���K���E���h�Ȃ烉�E���h�I�����삠��
	if (m_manager->GetIsPracticeRound()) {
		for (int i = 0; i < PLAYER_NUM; i++) {
			if (Pad(i).GetDown(enButtonY)) {
				m_pushCnt++;
			}
		}
		if (m_pushCnt >= MAX_PUSH) {
			m_timeLimitSec = -1.0f;
		}		
	}

	//�m�[�R���e�X�g
	for (int i = 0; i < PLAYER_NUM; i++) {
		if (Pad(i).GetDown(enButtonB)) {
			m_pushCntNoCon++;
		}
	}
	if (m_pushCntNoCon >= MAX_PUSH_NOCON) {
		m_manager->NoContest();
		return;
	}
}

void Game::PostLoopUpdate() {
	if (m_toEndTimeSec > 0.0f) {
		m_toEndTimeSec -= GetRealDeltaTimeSec();
		if (m_toEndTimeSec < FLT_EPSILON) {
			//�v���C���[�̎��S�����m
			bool isDeath = false;
			bool isDeathPlayer[PLAYER_NUM] = {};
			for (int i = 0; i < PLAYER_NUM; i++) {
				if (m_player[i]->GetIsDeath()) {
					isDeath = true;
					isDeathPlayer[m_player[i]->GetPlayerNum()] = true;
				}
			}
			//�I��
			m_manager->GameEnd(isDeathPlayer);
			return;
		}
	}
}

namespace {
	constexpr wchar_t TEXT[][64] = { L"[B]�m�[�R���e�X�g(%d/%d)\n\n��񂵂イ���E���h\n[Y]��񂾂ŃQ�[���X�^�[�g(%d/%d)", L"[B]�m�[�R���e�X�g(%d/%d)" };
}

void Game::PostRender() {
	//��������
	m_timeLimitSec = max(0.0f, m_timeLimitSec);

	if (GetScreenNum() == 1) {
		m_font.DrawFormat(
			L"%2d:%2d",
			{ 0.95f,0.0f }, { 1.0f,0.0f },
			static_cast<int>(m_timeLimitSec / 60.0f), static_cast<int>(m_timeLimitSec) % 60
		);
	}
	else {
		m_font.DrawFormat(
			L"%2d:%2d",
			{ 0.5f,0.0f }, { 0.5f,0.0f },
			//m_manager->GetRoundCount(), m_manager->GetMaxRound(),
			//m_manager->GetPlayerScore(0), m_manager->GetPlayerScore(1),
			static_cast<int>(m_timeLimitSec / 60.0f), static_cast<int>(m_timeLimitSec) % 60
		);
	}

	//���E���h�I������
	int ind = m_manager->GetIsPracticeRound() ? 0 : 1;
	CVector2 scale = m_font.GetScale();
	m_font.SetScale(scale*0.5f);
	m_font.SetUseFont(m_font.enJPN);
	if (GetScreenNum() == 1) {
		m_font.DrawFormat(
			TEXT[ind],
			{ 0.95f,0.08f }, { 1.0f,0.0f },
			m_pushCntNoCon, MAX_PUSH_NOCON,
			m_pushCnt, MAX_PUSH
		);
	}
	else {
		m_font.DrawFormat(
			TEXT[ind],
			{ 0.5f,0.08f }, { 0.5f,0.0f },
			m_pushCntNoCon, MAX_PUSH_NOCON,
			m_pushCnt, MAX_PUSH
		);
	}
	m_font.SetUseFont(m_font.enENG);
	m_font.SetScale(scale);

	//���s�\��
	if (m_toEndTimeSec > 0.0f) {
		//�v���C���[�̎��S�����m
		bool isDeathPlayer[PLAYER_NUM] = {};
		for (int i = 0; i < PLAYER_NUM; i++) {
			if (m_player[i]->GetIsDeath()) {
				isDeathPlayer[m_player[i]->GetPlayerNum()] = true;
			}
		}
		wchar_t string[64];
		if (isDeathPlayer[0] && isDeathPlayer[1]) {
			wcscpy_s(string, L"destroyed");
		}
		else if (isDeathPlayer[0]){
			if (m_playerNum == 1) {
				wcscpy_s(string, L"YOU\nLOSE");
			}
			else {
				wcscpy_s(string, L"Right Side\nWIN");
			}
		}
		else if (isDeathPlayer[1]) {
			if (m_playerNum == 1) {
				wcscpy_s(string, L"YOU\nWIN");
			}
			else {
				wcscpy_s(string, L"Left Side\nWIN");
			}
		}
		else {
			wcscpy_s(string, L"TIME UP");
		}
		m_bigFont.Draw(string, 0.5f, CVector4::Black(), 1.0f, { 0.575f, 0.6f });
	}
}
