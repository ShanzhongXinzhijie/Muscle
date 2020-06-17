#include "stdafx.h"
#include "CTitle.h"
#include "WindowSizeManager.h"
#include "LoadingScreen.h"
#include "AssembleScene.h"
#include "CGameMode.h"

bool CTitle::Start() {
	//画面サイズ変更
	m_ptrWinSizeMane = FindGO<WindowSizeManager>(L"WindowSizeManager");
	m_ptrWinSizeMane->ChangeWindowSize(true);

	//ロゴ
	m_sprite.Init(L"Resource/spriteData/logo.png");

	//BGM
	FindGO<BGMPlayer>(L"BGMPlayer")->Play(L"Resource/sound/bgm/the-night-of-blood-syokei-no-theme.wav");

	return true;
}

void CTitle::Update() {
	for (int i = 0; i < PLAYER_NUM; i++) {
		//画面サイズ変更
		if (Pad(i).GetDown(enButtonRB)) {
			m_ptrWinSizeMane->ChangeWindowScale();
			m_ptrWinSizeMane->ChangeWindowSize(true);
		}

		//終了
		if (Pad(i).GetDown(enButtonB)) {
			m_pushCnt++;
		}

		if (!m_enter) {
			//タイトル
			//PRESS ANY BUTTON
			if (Pad(i).GetAnyDown()) {
				m_enter = true;
				break;
			}
		}
		else {
			//モード選択
			if (Pad(i).GetDown(enButtonLeft) || Pad(i).GetDown(enButtonLSLeft)) {
				m_selectMode--; if (m_selectMode < 0) { m_selectMode = 0; }
			}
			if (Pad(i).GetDown(enButtonRight) || Pad(i).GetDown(enButtonLSRight)) {
				m_selectMode++; if (m_selectMode > 1) { m_selectMode = 1; }
			}
			//決定
			if (Pad(i).GetDown(enButtonA) || Pad(i).GetDown(enButtonStart)) {
				//画面サイズ変更
				FindGO<WindowSizeManager>(L"WindowSizeManager")->ChangeWindowSize(m_selectMode == 0);
				//プレイヤー数変更
				if (m_selectMode == 0) {
					FindGO<CGameMode>(L"CGameMode")->SetPlayerNum(2);
				}
				else {
					FindGO<CGameMode>(L"CGameMode")->SetPlayerNum(1);
				}
				//遷移
				new LoadingScreen([]() {new AssembleScene(); });
				//自殺
				delete this;
				return;
			}
		}
	}

	//終了
	if (m_pushCnt >= MAX_PUSH) {
		BreakGameLoop();
		delete this;
		return;
	}
}

void CTitle::PostLoopUpdate() {
	//背景
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void CTitle::PostRender() {
	//ロゴ
	m_sprite.Draw({ 0.5f,0.5f }, CVector2::One(), { 0.5f,0.5f });

	wchar_t string[64];
	swprintf_s(string, L"Bれんだでオワリ (%d/%d)", m_pushCnt, MAX_PUSH);
	m_font.Draw(string, { 0.5f,0.0f }, CVector4::Black(), CVector2::One()*0.5f, { 0.5f,0.0f });

	swprintf_s(string, L"[RB]がめんサイズへんこう(X%.2f)", m_ptrWinSizeMane->GetWindowScale());
	m_font.Draw(string, { 0.55f,0.6f }, CVector4::Black(), CVector2::One()*0.5f, { 0.0f,0.0f });

	if(!m_enter) {
		m_font.Draw(L"ｭｭｭｭｭデスホトケ", { 0.5f,0.09f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
		m_font.Draw(L"PRESS ANY BUTTON ->めきめき", { 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });		
		return;
	}

	//タイトル
	m_font.Draw(L"モードをせんたく", { 0.5f,0.09f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });

	//モードセレクト
	for (int i = 0; i < 2; i++) {
		CVector4 color = CVector4::Black();
		if (i == m_selectMode) {
			color = CVector4::Red();
		}
		if (i == 0) {
			m_font.Draw(L"ひだり VS みぎ", { 0.25f,0.85f }, color, CVector2::One(), { 0.0f,0.5f });
		}
		else {
			m_font.Draw(L"おまえ VS メカ", { 0.5f,0.85f }, color, CVector2::One(), { 0.0f,0.5f });
		}
	}

	//モード説明
	if (0 == m_selectMode) {
		m_font.Draw(L"ともだちとたたかいます", { 0.5f,0.95f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
	}
	else {
		m_font.Draw(L"きかいのともだちとたたかいます", { 0.5f,0.95f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
	}
}