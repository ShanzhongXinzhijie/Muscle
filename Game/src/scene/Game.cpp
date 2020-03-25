#include "stdafx.h"
#include "Game.h"
#include "GameManager.h"
#include "CGameMode.h"

Game::Game(GameManager* manager) 
	: m_manager(manager), m_timeLimitSec(static_cast<float>(manager->GetTimeLimitSec()) + 0.9f)
{
	//�J�E���g�_�E���N���X����
	int scores[PLAYER_NUM] = { m_manager->GetPlayerScore(0),m_manager->GetPlayerScore(1) };
	new CountDown(m_manager->GetRoundCount(), m_manager->GetMaxRound(), scores, m_manager->GetTimeLimitSec());

	//�v���C���[����
	m_player[0] = std::make_unique<CPlayer>(1, m_manager->GetIsPracticeRound());
	m_player[1] = std::make_unique<CPlayer>(0, m_manager->GetIsPracticeRound());

	//���K���E���h�Ȃ�s���ݒ�ɂ���
	if (m_manager->GetIsPracticeRound()) {
		for (auto& player : m_player) {
			player->GetDeathHotoke().SetIsImmortal(true);
		}
	}

	//�X�e�[�W����
	m_shibuya = std::make_unique<Shibuya>();

	//m_font.SetColor(CVector4::Black());

	//�v���C���[�l���̎擾
	if (m_manager->GetIsPracticeRound() && FindGO<CGameMode>(L"CGameMode")->GetPlayerNum() == 1) {
		MAX_PUSH = 6;
		MAX_PUSH_NOCON = 6;
	}
}

void Game::PreUpdate() {
	//�v���C���[�̎��S�����m
	bool isDeath = false;
	bool isDeathPlayer[PLAYER_NUM] = {};
	for (int i = 0; i < PLAYER_NUM;i++) {
		if (m_player[i]->GetIsDeath()) {
			isDeath = true;
			isDeathPlayer[m_player[i]->GetPlayerNum()] = true;
		}
	}
	//�v���C���[���S�ŃQ�[���I��
	if (isDeath) {
		m_manager->GameEnd(isDeathPlayer);
		return;
	}

	//���K���E���h�Ȃ烉�E���h�I�����삠��
	if (m_manager->GetIsPracticeRound()) {
		for (int i = 0; i < PLAYER_NUM; i++) {
			if (Pad(i).GetDown(enButtonB)) {
				m_pushCnt++;
			}
		}
		if (m_pushCnt >= MAX_PUSH) {
			m_timeLimitSec = -1.0f;
		}		
	}

	//�m�[�R���e�X�g
	for (int i = 0; i < PLAYER_NUM; i++) {
		if (Pad(i).GetDown(enButtonX)) {
			m_pushCntNoCon++;
		}
	}
	if (m_pushCntNoCon >= MAX_PUSH_NOCON) {
		m_manager->NoContest();
		return;
	}

	//��������
	m_timeLimitSec -= FRAME_PER_SECOND;
	if (m_timeLimitSec < 0.0f) {
		m_manager->GameEnd(isDeathPlayer);
		return;
	}
}

namespace {
	constexpr wchar_t TEXT[][64] = { L"[X]�m�[�R���e�X�g(%d/%d)\n\n��񂵂イ���E���h\nB��񂾂ŃQ�[���X�^�[�g(%d/%d)", L"[X]�m�[�R���e�X�g(%d/%d)" };
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
}
