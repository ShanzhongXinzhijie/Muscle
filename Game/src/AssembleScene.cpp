#include "stdafx.h"
#include "AssembleScene.h"

#include "BP_FishHead.h"
#include "BP_BirdWing.h"
#include "BP_KaniArm.h"
#include "BP_HumanLeg.h"
#include "BP_HumanMantle.h"
#include "BP_HumanArm.h"

bool AssembleScene::Start() {
	//フォント初期化
	m_fontBold.LoadFont(L"Resource/font/eunomia_0200/EunomiaBold.spritefont");

	//パーツリスト初期化
	m_parts[CDeathHotoke::enHead].push_back(std::make_unique<BP_FishHead>());

	m_parts[CDeathHotoke::enArm].push_back(std::make_unique<BP_KaniArm>());
	m_parts[CDeathHotoke::enArm].push_back(std::make_unique<BP_HumanArm>());

	m_parts[CDeathHotoke::enWing].push_back(std::make_unique<BP_BirdWing>());
	m_parts[CDeathHotoke::enWing].push_back(std::make_unique<BP_HumanMantle>());

	m_parts[CDeathHotoke::enLeg].push_back(std::make_unique<BP_HumanLeg>());

	//表示用ホトケ初期化

	return true;
}

void AssembleScene::Update() {
	for (int i = 0; i < PLAYER_NUM; i++) {
		if (Pad(i).GetDown(enButtonA)) {
			m_isReady = !m_isReady;
		}

		if (Pad(i).GetDown(enButtonLeft)) {
		}
		if (Pad(i).GetDown(enButtonRight)) {
		}

		if (Pad(i).GetDown(enButtonUp)) {
		}
		if (Pad(i).GetDown(enButtonDown)) {
		}
	}
}

void AssembleScene::PostUpdate() {
	for (auto& hotoke : m_hotokes) {
		hotoke.SetPos(CVector3::Left()*50.0f);
	}
}

void AssembleScene::PostLoopUpdate() {
	//背景
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void AssembleScene::HUDRender(int HUDNum) {
	m_font.Draw(L"HEAD:", { 0.0f,0.0f }, CVector4::Black(), CVector2::One(), { 0.0f,0.0f });
	m_font.Draw(L" \nなまえ", { 0.0f,0.0f }, CVector4::Black(), CVector2::One(), { 0.0f,0.0f });

	//準備完了
	if (m_isReady) {
		m_fontBold.Draw(L"READY", { 0.5f,0.5f }, CVector4::Red(), CVector2::One(), { 0.5f,0.5f });
	}
}
