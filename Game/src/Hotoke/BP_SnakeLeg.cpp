#include "stdafx.h"
#include "BP_SnakeLeg.h"

using namespace GameObj;

namespace {
	const CVector3 hitboxOffset = CVector3::Down()*50.f;
}

void BP_SnakeLeg::InnerStart() {
	m_name = L"スネーク";

	//アニメーション
	m_animation[0].Load(L"Resource/animation/snakeleg.tka", true);

	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/snakeleg.cmo", m_animation, 1);
	//ノーマルマップ読み込み
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	HRESULT hr = DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/SnakeNormalMapEX.png", nullptr, textureView.ReleaseAndGetAddressOf());
	//マテリアル設定
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetNormalTexture(textureView.Get());
			me->SetShininess(0.45f);
		}
	);

	//足のIK設定
	for (int i = 0; i < 1; i++) {
		m_ikSetting[i] = m_model->GetSkinModel().GetSkeleton().CreateIK();
		if (i == 0) {
			m_ikSetting[i]->tipBone = m_model->FindBone(L"Bone007");
			m_ikSetting[i]->rootBone = m_model->FindBone(L"Bone002");
		}
		else if (i == 1) {
			m_ikSetting[i]->tipBone = m_model->FindBone(L"Bone004");
			m_ikSetting[i]->rootBone = m_model->FindBone(L"Bone002");
		}
		else {
			m_ikSetting[i]->tipBone = m_model->FindBone(L"Bone003");
			m_ikSetting[i]->rootBone = m_model->FindBone(L"Bone002");
		}
		m_ikSetting[i]->InitFootIK();
		m_ikSetting[i]->footIKRayEndOffset = CVector3::AxisY()*-500.0f*m_ptrCore->GetScale().y;
	}

	//当たり判定(足)
	constexpr float radius = 50.0f;
	constexpr float height = 50.0f;
	const float modelScale = m_ptrCore->GetScale().GetMax() / (0.0188f*2.0f);
	{
		//形状初期化
		m_col.m_collision.CreateCapsule({}, {}, radius * modelScale, height*modelScale);
		//m_col[lr].m_collision.CreateSphere({}, {}, radius * modelScale);

		//ID設定
		m_col.m_reference.ownerID = m_ptrCore->GetFuID();
		m_col.m_reference.nonHitID = m_ptrCore->GetFuID();

		//位置
		m_col.SetPos(m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()) + hitboxOffset);
		m_beforePos = m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo());

		//物理属性
		m_col.m_reference.attributes.set(enPhysical);

		//衝突処理
		m_col.m_reference.m_preCollisionFunc = [&](ReferenceCollision* H) {
			//体当たり
			auto[damege, stunSec] = DHUtil::CalcRamDamege(m_col.m_reference.velocity, H->velocity);
			m_col.m_reference.damege = damege;
			m_col.m_reference.stunTimeSec = stunSec;
			return true;
		};
	}

	//コントローラー
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_SnakeLeg(this, m_ptrCore);
	}
}

void BP_SnakeLeg::Update() {
	//コントローラーに操作させる
	if (m_controller)m_controller->Update();

	//足の位置取得
	float footDistance;
	footDistance = m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()).y;

	//足と本体の位置が近ければ、本体位置を上へ
	const float modelScale = m_ptrCore->GetScale().y / (0.0188f*2.0f);
	const float minFootDistance = 20.0f*modelScale, maxFootDistance = 167.0f*modelScale;
	//これ以上、足は縮まない
	if (footDistance < minFootDistance) {
		//本体位置を上に
		m_ptrCore->SetPos(m_ptrCore->GetPos() + CVector3(0.0f, minFootDistance - footDistance, 0.0f));
	}
	//足が縮んでいる
	if (footDistance < maxFootDistance) {
		//徐々に足を伸ばす
		float stretchMax = maxFootDistance - footDistance;
		m_ptrCore->AddVelocity(CVector3(0.0f, min(stretchMax, m_ptrCore->GetGravity() + 1.0f), 0.0f));
	}
}

void BP_SnakeLeg::PostUTRSUpdate() {
	//判定の更新
	m_col.SetPos(m_ptrCore->GetPos() + hitboxOffset);
	m_col.SetVelocity(m_ptrCore->GetPos() - m_beforePos);
	m_beforePos = m_ptrCore->GetPos();

	//足の位置取得
	float footDistance;
	footDistance = m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()).y;

	const float modelScale = m_ptrCore->GetScale().y / (0.0188f*2.0f);
	const float minFootDistance = 20.0f*modelScale, maxFootDistance = 167.0f*modelScale;
	//ある程度足が縮んでいるなら接地している扱い
	if (footDistance < maxFootDistance - 1.0f) {
		//接地しているなら抵抗UP
		//m_ptrCore->MulDrag(20.0f + 10.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));
		//m_ptrCore->MulRotatability(2.0f + 1.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));//回転力もUP
		//振動
		m_ptrCore->SetShakePower(0.0015f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));
	}
}

void BP_SnakeLeg::Draw2D() {
}

void HCon_SnakeLeg::InnerUpdate() {
}
