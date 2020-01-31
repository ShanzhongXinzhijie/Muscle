#include "stdafx.h"
#include "AssembleScene.h"
#include "WindowSizeManager.h"
#include "GameManager.h"
#include "LoadingScreen.h"
#include "CGameMode.h"
#include "CTitle.h"

#include "BP_FishHead.h"
#include "BP_BirdWing.h"
#include "BP_KaniArm.h"
#include "BP_Heads.h"
#include "BP_HumanLeg.h"
#include "BP_HumanMantle.h"
#include "BP_HumanArm.h"

namespace {
	const CVector3 HOTOKE_POS = { 300.0f,150.0f,-300.0f };
	const CVector3 PLAYER_POS_OFFSET = HOTOKE_POS.GetNorm().GetCross(CVector3::AxisY()) * 10000.0f;
}

AssembleScene::AssembleScene() : m_playerNum(FindGO<CGameMode>(L"CGameMode")->GetPlayerNum()) {}

bool AssembleScene::Start() {
	//画面サイズ変更
	FindGO<WindowSizeManager>(L"WindowSizeManager")->ChangeWindowSize(m_playerNum > 1);

	//フォント初期化
	m_fontBold.LoadFont(L"Resource/font/eunomia_0200/EunomiaBold.spritefont");

	//パーツリスト初期化
	m_parts[CDeathHotoke::enHead].push_back(std::make_unique<BP_FishHead>());
	m_parts[CDeathHotoke::enHead].push_back(std::make_unique<BP_HumanHead>());	
	m_parts[CDeathHotoke::enHead].push_back(std::make_unique<BP_SnailHead>());
	m_parts[CDeathHotoke::enHead].push_back(std::make_unique<BP_RockHead>());
	m_parts[CDeathHotoke::enHead].push_back(std::make_unique<BP_RockIIHead>());
	m_parts[CDeathHotoke::enHead].push_back(std::make_unique<BP_EggHead>());
	m_parts[CDeathHotoke::enHead].push_back(std::make_unique<BP_ElephantHead>());
	m_parts[CDeathHotoke::enHead].push_back(std::make_unique<BP_PalmHead>());

	m_parts[CDeathHotoke::enArm].push_back(std::make_unique<BP_KaniArm>());
	m_parts[CDeathHotoke::enArm].push_back(std::make_unique<BP_HumanArm>());

	m_parts[CDeathHotoke::enWing].push_back(std::make_unique<BP_BirdWing>());
	m_parts[CDeathHotoke::enWing].push_back(std::make_unique<BP_HumanMantle>());

	m_parts[CDeathHotoke::enLeg].push_back(std::make_unique<BP_HumanLeg>());

	//表示用ホトケ初期化
	for (int i = 0; i < m_playerNum; i++) {
		m_hotokes.push_back(std::make_unique<CDeathHotoke>());
	}	
	for (auto& hotoke : m_hotokes) {
		hotoke->SetBodyPart(CDeathHotoke::enHead, std::unique_ptr<IBodyPart>(m_parts[CDeathHotoke::enHead][0]->Create()));
		hotoke->SetBodyPart(CDeathHotoke::enArm, std::unique_ptr<IBodyPart>(m_parts[CDeathHotoke::enArm][0]->Create()));
		hotoke->SetBodyPart(CDeathHotoke::enWing, std::unique_ptr<IBodyPart>(m_parts[CDeathHotoke::enWing][0]->Create()));
		hotoke->SetBodyPart(CDeathHotoke::enLeg, std::unique_ptr<IBodyPart>(m_parts[CDeathHotoke::enLeg][0]->Create()));
	}

	//カメラ初期化
	for (int i = 0; i < m_playerNum; i++) {
		m_cameras.push_back(std::make_unique<GameObj::PerspectiveCamera>());
	}
	int i = 0;
	for (auto& camera : m_cameras) {
		camera->SetPos(PLAYER_POS_OFFSET * (float)i);
		camera->SetTarget(HOTOKE_POS + PLAYER_POS_OFFSET * (float)i);
		m_toCameraPos[i] = camera->GetPos() - camera->GetTarget();
		camera->SetViewAngleDeg(60.0f);
		camera->SetFar(15000.0f);

		if (m_playerNum == 1) {
			SetMainCamera(camera.get());
		}
		else {
			SetCameraToList(i, camera.get());
		}

		i++;
	}

	//ライト初期化
	m_light.SetDirection({0.5f, -0.5f, -0.8f});
	m_light.SetColor(1.0f);
	//SetAmbientLight(CVector3( 0.8f,0.1f,0.35f )*0.2f);
	SetAmbientCubeMap(L"Resource/cubemap/cube2.dds", CVector3(0.8f, 0.1f, 0.35f)*0.2f);

	//フォグを有効化
	//SetEnableFog(true);
	//SetFogDistance(1000.0f*1.5f);
	//SetFogHeightScale(3.0f);
	////SetFogColor({ 0.58f,0.69f,0.84f });
	//SetFogColor({ 0.28f,0.4f,0.65f });

	return true;
}

void AssembleScene::Update() {
	int readyCnt = 0;//準備完了数

	for (int i = 0; i < m_playerNum; i++) {
		//もどる
		if (Pad(i).GetDown(enButtonB)) {
			m_pushCnt++;
		}

		//準備完了
		if (Pad(i).GetDown(enButtonA) || Pad(i).GetDown(enButtonStart)) {
			m_isReady[i] = !m_isReady[i];
		}

		//準備完了カウント
		if (m_isReady[i]) {
			readyCnt++;
		}
		
		if (!m_isReady[i]) {
			//編集パーツ選択
			bool isSelect = false;
			if (Pad(i).GetDown(enButtonUp) || Pad(i).GetDown(enButtonLSUp)) {
				m_nowEditBody[i]--; if (m_nowEditBody[i] < 0) { m_nowEditBody[i] = 0; }
				isSelect = true;
			}
			if (Pad(i).GetDown(enButtonDown) || Pad(i).GetDown(enButtonLSDown)) {
				m_nowEditBody[i]++; if (m_nowEditBody[i] >= CDeathHotoke::enPartsNum) { m_nowEditBody[i] = CDeathHotoke::enPartsNum - 1; }
				isSelect = true;
			}

			//パーツ選択
			if (!isSelect) {
				bool isEdit = false;
				if (Pad(i).GetDown(enButtonLeft) || Pad(i).GetDown(enButtonLSLeft)) {
					m_nowBodyType[i][m_nowEditBody[i]]--;
					if (m_nowBodyType[i][m_nowEditBody[i]] < 0) {
						m_nowBodyType[i][m_nowEditBody[i]] = (int)m_parts[m_nowEditBody[i]].size() - 1;
					}
					isEdit = true;
				}
				if (Pad(i).GetDown(enButtonRight) || Pad(i).GetDown(enButtonLSRight)) {
					m_nowBodyType[i][m_nowEditBody[i]]++;
					if (m_nowBodyType[i][m_nowEditBody[i]] >= m_parts[m_nowEditBody[i]].size()) {
						m_nowBodyType[i][m_nowEditBody[i]] = 0;
					}
					isEdit = true;
				}
				if (isEdit) {
					//パーツ入れ替え...
					m_hotokes[i]->SetBodyPart((CDeathHotoke::enBodyParts)m_nowEditBody[i], std::unique_ptr<IBodyPart>(m_parts[m_nowEditBody[i]][m_nowBodyType[i][m_nowEditBody[i]]]->Create()));
				}
			}
		}

		//カメラ移動
		m_cameraRots[i].x += -Pad(i).GetStick(R).y*0.1f;
		m_cameraRots[i].x = CMath::Clamp(m_cameraRots[i].x, -1.20727777f, CMath::PI_HALF + (CMath::PI_HALF - 1.20727777f));
		m_cameraRots[i].y += Pad(i).GetStick(R).x*0.1f;
		if (m_cameraRots[i].y < -CMath::PI2) {
			m_cameraRots[i].y += CMath::PI2;
		}
		if (m_cameraRots[i].y > CMath::PI2) {
			m_cameraRots[i].y -= CMath::PI2;
		}
		CVector3 toPos = m_toCameraPos[i];
		CQuaternion rotAX(toPos.GetNorm().GetCross(CVector3::AxisY()), m_cameraRots[i].x);
		CQuaternion rotAY(CVector3::AxisY(), m_cameraRots[i].y);
		rotAX.Multiply(toPos);
		rotAY.Multiply(toPos);
		m_cameras[i]->SetPos(m_cameras[i]->GetTarget()+toPos);

		//光背非表示
		if (toPos.GetNorm().z < -0.5f) {
			m_hotokes[i]->SetIsDrawKouhai(false);
		}
		else {
			m_hotokes[i]->SetIsDrawKouhai(true);
		}
	}

	//戻る
	if (m_pushCnt >= MAX_PUSH) {
		new LoadingScreen([]() {new CTitle; });
		delete this;
		return;
	}

	//規定数準備完了でゲームに移行
	if (readyCnt >= m_playerNum) {
		//アセンブル設定
		HotokeAssembleManager* assembleManager = FindGO<HotokeAssembleManager>(L"HotokeAssembleManager");
		for (int i = 0; i < m_playerNum; i++) {
			HotokeAssembleManager::HotokeAssemble& assem = assembleManager->GetHotokeAssemble(i);
			//パーツ設定
			for (int partsNum = 0; partsNum < CDeathHotoke::enPartsNum; partsNum++) {
				assem.parts[partsNum] = m_hotokes[i]->GetBodyPart((CDeathHotoke::enBodyParts)partsNum).Create();
			}
			//AIはナシ
			assem.ai = nullptr;
		}

		//CPU設定
		if (m_playerNum == 1) {
			HotokeAssembleManager::HotokeAssemble& assem = assembleManager->GetHotokeAssemble(1);
			//パーツ設定
			for (int partsNum = 0; partsNum < CDeathHotoke::enPartsNum; partsNum++) {
				assem.parts[partsNum] = m_hotokes[0]->GetBodyPart((CDeathHotoke::enBodyParts)partsNum).Create();
			}
			//AI
			assem.ai = new TestAI();
		}

		//ゲームクラス作成
		new LoadingScreen([]() {new GameManager; });
		//自殺
		delete this;
		return;
	}
}

void AssembleScene::PostUpdate() {
	//ホトケの位置とか設定
	int i = 0;
	for (auto& hotoke : m_hotokes) {
		hotoke->SetPos(HOTOKE_POS + PLAYER_POS_OFFSET * (float)i);
		hotoke->SetTargetPos(hotoke->GetPos()+hotoke->GetFront()*10000.0f + CVector3::AxisY()*-18000.0f);
		hotoke->SetVanisingPoint(hotoke->GetPos() + hotoke->GetFront()*10000.0f + CVector3::AxisY()*-18000.0f);
		hotoke->SetTarget(nullptr);
		i++;
	}
}

//void AssembleScene::PostLoopUpdate() {
	////背景
	//DrawQuad2D(0.0f, 1.0f, CVector4::White());
//}

void AssembleScene::PostRender() {
	wchar_t string[64];
	swprintf_s(string, L"Bれんだでもどる (%d/%d)", m_pushCnt, MAX_PUSH);
	m_font.Draw(string, { 0.5f,0.0f }, CVector4::White(), CVector2::One(), { 0.5f,0.0f });
}

void AssembleScene::HUDRender(int HUDNum) {
	//パーツ名表示
	wchar_t string[256];
	for (int i = 0; i < CDeathHotoke::enPartsNum; i++) {
		switch (i)
		{
		case CDeathHotoke::enHead:			
			swprintf_s(string, 256, L"HEAD:%ls", m_hotokes[HUDNum]->GetBodyPart((CDeathHotoke::enBodyParts)i).GetName());
			break;
		case CDeathHotoke::enArm:
			swprintf_s(string, 256, L"ARMS:%ls", m_hotokes[HUDNum]->GetBodyPart((CDeathHotoke::enBodyParts)i).GetName());
			break;
		case CDeathHotoke::enWing:
			swprintf_s(string, 256, L"WING:%ls", m_hotokes[HUDNum]->GetBodyPart((CDeathHotoke::enBodyParts)i).GetName());
			break;
		case CDeathHotoke::enLeg:
			swprintf_s(string, 256, L"LEGS:%ls", m_hotokes[HUDNum]->GetBodyPart((CDeathHotoke::enBodyParts)i).GetName());
			break;
		default:
			break;
		}
		if (m_nowEditBody[HUDNum] == i) {
			m_font.Draw(string, { 0.0f,0.7f + 0.05f*i }, CVector4::Green(), CVector2::One(), { 0.0f,0.0f });
		}
		else {
			m_font.Draw(string, { 0.0f,0.7f + 0.05f*i }, CVector4::Black(), CVector2::One(), { 0.0f,0.0f });
		}
	}

	//準備完了
	if (m_isReady[HUDNum]) {
		m_fontBold.Draw(L"READY", { 0.9f,0.85f }, CVector4::Red(), CVector2::One(), { 1.0f,1.0f });
	}
}
