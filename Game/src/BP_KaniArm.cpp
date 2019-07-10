#include "stdafx.h"
#include "BP_KaniArm.h"
#include "BulletKani.h"

using namespace GameObj;

void BP_KaniArm::InnerStart() {
	//アニメーション(ボーン初期化用)
	m_initPose.Load(L"Resource/animation/kaniarm.tka", true);

	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/kaniarm.cmo", &m_initPose, 1);

	//腕のIK設定
	m_ikSetting[R] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[R]->tipBone = m_model->FindBone(L"Bone031");
	m_ikSetting[R]->rootBone = m_model->FindBone(L"Bone026");
	m_ikSetting[R]->iteration = 3;
	//m_ikSetting[R]->footIKRayEndOffset = 

	m_ikSetting[L] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[L]->tipBone = m_model->FindBone(L"Bone031(mirrored)");
	m_ikSetting[L]->rootBone = m_model->FindBone(L"Bone026(mirrored)");
	m_ikSetting[L]->iteration = 3;

	//
	m_muzzleBoneID[R] = m_model->FindBoneID(L"Bone031");
	m_muzzleBoneID[L] = m_model->FindBoneID(L"Bone031(mirrored)");

	//エフェクト
	for (auto& model : m_muzzleFlash) {
		model.Init(L"Resource/spriteData/kaniFlare.png", 1, false);
		model.GetModel().InitPostDraw(PostDrawModelRender::enAdd);
		model.GetModel().GetSkinModel().SetCullMode(D3D11_CULL_NONE);
		model.GetModel().SetIsShadowCaster(false);
	}

	//コントローラー
	m_controller = new HCon_KaniArm(this,m_ptrCore);
}

void BP_KaniArm::Update() {
	//毎フレームの初期化
	for (int i = 0; i < enLRNUM; i++){
		m_isCharging[i] = false;
		m_isMachineGunning[i] = false;
		m_muzzleFlash[i].SetIsDraw(false);
	}	

	//コントローラーに操作させる
	m_controller->Update();

	//チャージ時間リセット
	for (int i = 0; i < enLRNUM; i++) {
		if (!m_isCharging[i]) { m_chargeTime[i] = 0; }
	}
}

void BP_KaniArm::PostUTRSUpdate() {
	for (int i = 0; i < enLRNUM; i++) {
		//IKの目標
		m_ikSetting[i]->targetPos = CVector3::Zero();

		if (m_isMachineGunning[i]) {
			if (m_chargeTime[i]%5 == 0) {				
				//マズルエフェクト
				m_muzzleTime[i] = 2;
				//発射
				new BulletKani(m_model->GetBonePos(m_muzzleBoneID[i]), (m_ikSetting[i]->targetPos - m_model->GetBonePos(m_muzzleBoneID[i])).GetNorm()*100.0f);
			}
		}

		//マズルエフェクト
		if (m_muzzleTime[i] > 0) {
			m_muzzleTime[i] --;
			m_muzzleFlash[i].SetIsDraw(true);
			m_muzzleFlash[i].SetPos(m_model->GetBonePos(m_muzzleBoneID[i]));
			m_muzzleFlash[i].SetScaleHoldAspectRatio(100.0f);
		}
	}

	//m_muzzleCnt++;
	//if (m_muzzleCnt <= 4) {
	//	m_muzzleFlash.SetIsDraw(true);
	//	m_muzzleFlash.SetPos(m_model->GetBonePos(m_muzzleBoneID[L]));
	//	m_muzzleFlash.SetScaleHoldAspectRatio(100.0f);

	//	std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>();
	//	billboard->Init(L"Resource/spriteData/smoke.png");
	//	//billboard->GetModel().InitPostDraw(PostDrawModelRender::enAlpha);
	//	billboard->SetPos(m_model->GetBonePos(m_muzzleBoneID[L]));
	//	billboard->SetScale(20.0f);
	//	SuicideObj::CParticle<CBillboard>* particle = new SuicideObj::CParticle<CBillboard>(std::move(billboard),30);
	//	particle->SetMove(CVector3(-1.0f,0.0f,-2.0f)*10.0f);
	//	particle->SetScaling(1.05f);

	//	new BulletKani(m_model->GetBonePos(m_muzzleBoneID[L]), (m_ikSetting[L]->targetPos - m_model->GetBonePos(m_muzzleBoneID[L])).GetNorm()*10.0f);
	//}
	//else {
	//	if (m_muzzleCnt >= 8) { m_muzzleCnt = 0; }
	//}
}

void BP_KaniArm::Charge(enLR lr) {
	//チャージ
	m_isCharging[lr] = true;
	m_chargeTime[lr] ++;
}
void BP_KaniArm::MachineGun(enLR lr) {
	if (m_chargeTime[lr] > m_machineGunTime) {
		//マシンガン
		m_isMachineGunning[lr] = true;
	}
}
void BP_KaniArm::Rocket(enLR lr) {
	//if (m_chargeTime > FLT_EPSILON && m_chargeTime < m_fullCharge) {
		//ダブルタップ
	//}
}
void BP_KaniArm::Lazer(enLR lr) {
	//レーザー出す
}
void BP_KaniArm::Stab() {
	//スタブ出す
}

//ヒューマンコントローラー
void HCon_KaniArm::Update() {
	for (auto lr : { L, R }) {
		if (m_ptrCore->GetPad()->GetFire(lr)) {
			m_ptrBody->Charge(lr);
			m_ptrBody->MachineGun(lr);
		}
	}
}