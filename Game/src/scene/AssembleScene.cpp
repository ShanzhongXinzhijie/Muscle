#include "stdafx.h"
#include "AssembleScene.h"

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

bool AssembleScene::Start() {
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
	for (auto& hotoke : m_hotokes) {
		hotoke.SetBodyPart(CDeathHotoke::enHead, std::unique_ptr<IBodyPart>(m_parts[CDeathHotoke::enHead][0]->Create()));
		hotoke.SetBodyPart(CDeathHotoke::enArm, std::unique_ptr<IBodyPart>(m_parts[CDeathHotoke::enArm][0]->Create()));
		hotoke.SetBodyPart(CDeathHotoke::enWing, std::unique_ptr<IBodyPart>(m_parts[CDeathHotoke::enWing][0]->Create()));
		hotoke.SetBodyPart(CDeathHotoke::enLeg, std::unique_ptr<IBodyPart>(m_parts[CDeathHotoke::enLeg][0]->Create()));
	}

	//カメラ初期化
	int i = 0;
	for (auto& camera : m_cameras) {
		camera.SetPos(PLAYER_POS_OFFSET * (float)i);
		camera.SetTarget(HOTOKE_POS + PLAYER_POS_OFFSET * (float)i);
		camera.SetViewAngleDeg(60.0f);
		camera.SetFar(15000.0f);
		SetCameraToList(i, &camera);
		i++;
	}

	//ライト初期化
	m_light.SetDirection({0.5f, -0.5f, -0.8f});
	m_light.SetColor(1.0f);
	SetAmbientLight(CVector3( 0.8f,0.1f,0.35f )*0.2f);

	//フォグを有効化
	//SetEnableFog(true);
	//SetFogDistance(1000.0f*1.5f);
	//SetFogHeightScale(3.0f);
	////SetFogColor({ 0.58f,0.69f,0.84f });
	//SetFogColor({ 0.28f,0.4f,0.65f });

	return true;
}

void AssembleScene::Update() {
	for (int i = 0; i < PLAYER_NUM; i++) {
		//準備完了
		if (Pad(i).GetDown(enButtonA) || Pad(i).GetDown(enButtonStart)) {
			m_isReady = !m_isReady;
		}
		
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
				m_hotokes[i].SetBodyPart((CDeathHotoke::enBodyParts)m_nowEditBody[i], std::unique_ptr<IBodyPart>(m_parts[m_nowEditBody[i]][m_nowBodyType[i][m_nowEditBody[i]]]->Create()));
			}
		}
	}
}

void AssembleScene::PostUpdate() {
	//ホトケの位置とか設定
	int i = 0;
	for (auto& hotoke : m_hotokes) {
		hotoke.SetPos(HOTOKE_POS + PLAYER_POS_OFFSET * (float)i);
		hotoke.SetTargetPos(hotoke.GetPos()+hotoke.GetFront()*10000.0f + CVector3::AxisY()*-18000.0f);
		hotoke.SetVanisingPoint(hotoke.GetPos() + hotoke.GetFront()*10000.0f + CVector3::AxisY()*-18000.0f);
		hotoke.SetTarget(nullptr);
		i++;
	}
}

//void AssembleScene::PostLoopUpdate() {
	////背景
	//DrawQuad2D(0.0f, 1.0f, CVector4::White());
//}

void AssembleScene::HUDRender(int HUDNum) {
	//パーツ名表示
	wchar_t string[256];
	for (int i = 0; i < CDeathHotoke::enPartsNum; i++) {
		switch (i)
		{
		case CDeathHotoke::enHead:			
			swprintf_s(string, 256, L"HEAD:%ls", m_hotokes[HUDNum].GetBodyPart((CDeathHotoke::enBodyParts)i).GetName());
			break;
		case CDeathHotoke::enArm:
			swprintf_s(string, 256, L"ARMS:%ls", m_hotokes[HUDNum].GetBodyPart((CDeathHotoke::enBodyParts)i).GetName());
			break;
		case CDeathHotoke::enWing:
			swprintf_s(string, 256, L"WING:%ls", m_hotokes[HUDNum].GetBodyPart((CDeathHotoke::enBodyParts)i).GetName());
			break;
		case CDeathHotoke::enLeg:
			swprintf_s(string, 256, L"LEGS:%ls", m_hotokes[HUDNum].GetBodyPart((CDeathHotoke::enBodyParts)i).GetName());
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
	if (m_isReady) {
		m_fontBold.Draw(L"READY", { 0.9f,0.85f }, CVector4::Red(), CVector2::One(), { 1.0f,1.0f });
	}
}
