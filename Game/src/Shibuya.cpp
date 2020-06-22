#include "stdafx.h"
#include "Shibuya.h"
#include "Ari.h"
#include "CSmoke.h"
#include "FlyingGrass.h"

#include "BP_FishHead.h"
#include "BP_BirdWing.h"
#include "BP_KaniArm.h"
#include "BP_HumanLeg.h"
#include "BP_HumanMantle.h"

Shibuya::Shibuya(bool isTreeGene)
{	
	//Ari* ari = new Ari(CVector3::AxisY()*900.0f + CVector3::AxisX()*50.0f, {});
	//ari->SetTarget(&m_hotoke);

	//ライト作成
	m_directionLight.SetDirection(CVector3::AxisZ()*-1.0f);
	m_directionLight.SetColor(CVector3(0.98f,0.97f,0.91f)*0.5f);

	//街モデル
	m_graundModel.Init(L"Resource/modelData/tikei_flat.cmo");
	m_graundModel.SetScale({ 500.0f,500.0f,500.0f });
	//当たり判定
	m_graund.SetIsStaticObject(true);
	m_graund.CreateMesh(m_graundModel);
	m_graund.On_OneGroup(enField);
	m_graund.GetAttributes().set(enPhysical);
	m_graund.GetAttributes().set(enGraund);
	m_graund.SetCollisionFunc(
		[&](ReferenceCollision* H, SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (H->damege > 0.0f || H->velocity.LengthSq() > 1.0f) {
				new CSmoke(p.m_collisionPoint, 0.0f, { 0.8f,0.8f ,0.5f,0.8f });

				for (int i = 0; i < 12; i++) {
					CVector3 ramdamVec = CVector3::Up()*(15.0f*CMath::RandomZeroToOne() + 30.0f);
					CQuaternion rot = { CVector3::AxisX(), CMath::DegToRad(55.0f) - (CMath::RandomZeroToOne() * 2.0f * CMath::DegToRad(55.0f)) };
					rot.Multiply(ramdamVec);
					rot.SetRotation(CVector3::AxisY(), CMath::RandomZeroToOne() * CMath::PI2);
					rot.Multiply(ramdamVec);
					new FlyingGrass(p.m_collisionPoint, ramdamVec + H->velocity, 16);
				}
			}
		}
	);

	//テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tex, normaltex;	
	TextureFactory::GetInstance().Load(L"Resource/texture/moss3.png", nullptr, tex.ReleaseAndGetAddressOf(), nullptr, true);
	TextureFactory::GetInstance().Load(L"Resource/normalMap/moss3_n.bmp", nullptr, normaltex.ReleaseAndGetAddressOf(), nullptr, true);

	//モデルにシェーダとノーマルマップ設定
	m_graundModel.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			//mat->SetNormalTexture(normaltex.Get());
			mat->SetAlbedoTexture(tex.Get());
			mat->SetTriPlanarMappingPS();
			mat->SetTriPlanarMappingUVScale(0.002f);
			mat->SetShininess(0.3f);// m_shinnes);
			//mat->SetLightingEnable(false);
		}
	);
	m_graundModel.SetDrawPriority(DRAW_PRIORITY_MAX - 2);

	//城
	m_midCastle.Init(L"Resource/modelData/mid_graund.cmo");
	m_midCastle.SetScale(500.0f);
	//モデルにシェーダとノーマルマップ設定
	m_midCastle.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			if (mat->EqualMaterialName(L"Graund")) {
				mat->SetAlbedoTexture(tex.Get());
				mat->SetTriPlanarMappingPS();
				mat->SetTriPlanarMappingUVScale(0.002f);
				mat->SetShininess(0.3f);
			}
			else {
				mat->SetShininess(0.1f);
			}
		}
	);
	m_midCastle.SetDrawPriority(DRAW_PRIORITY_MAX - 2);

	//中景	
	m_midGraund.Init(L"Resource/modelData/far_graund.cmo");
	m_midGraund.SetRot({ CVector3::AxisY(), CMath::PI_HALF });
	m_midGraund.SetScale({ 500.0f,500.0f,500.0f });
	//モデルにシェーダとノーマルマップ設定
	m_midGraund.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			//mat->SetNormalTexture(normaltex.Get());
			mat->SetAlbedoTexture(tex.Get());
			mat->SetTriPlanarMappingPS();
			mat->SetTriPlanarMappingUVScale(0.002f);
			mat->SetShininess(0.3f);// m_shinnes);
		}
	);	
	m_midGraund.SetDrawPriority(DRAW_PRIORITY_MAX - 2);

	//遠景
	m_farGraund.Init(L"Resource/modelData/far_graund.cmo");
	m_farGraund.SetScale({ 500.0f,500.0f,500.0f });
	//モデルにシェーダとノーマルマップ設定
	m_farGraund.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			//mat->SetNormalTexture(normaltex.Get());
			//mat->SetAlbedoTexture(tex.Get());
			//mat->SetTriPlanarMappingPS();
			//mat->SetTriPlanarMappingUVScale(0.002f);
			mat->SetShininess(0.3f);// m_shinnes);
		}
	);
	//設定
	m_farGraund.SetDrawPriority(DRAW_PRIORITY_MAX - 1);
	m_farGraund.SetIsMostDepth(true);
	//m_farGraund.GetSkinModel().SetDepthBias(0.0f);
	m_farGraund.SetIsShadowCaster(false);
	//m_farGraund.GetSkinModel().SetDepthStencilState(GetGraphicsEngine().GetCommonStates().DepthNone());
	//描画前処理を設定
	m_farGraund.GetSkinModel().SetPreCullingFunction(
		[&](SkinModel* model) {
			m_farGraund.SetPos(GetMainCamera()->GetPos());
			//m_farGraund.SetPos({ GetMainCamera()->GetPos().x, 0.0f, GetMainCamera()->GetPos().z });
			m_farGraund.RefreshWorldMatrix();
		}
	);

	//空
	m_sky.Init(L"Resource/cubemap/cube2.dds",-1.0f,false);
	SetAmbientCubeMap(L"Resource/cubemap/cubemap.dds", CVector3::One());

	//フォグを有効化
	SetEnableFog(true);
	SetFogDistance(30000.0f*1.5f);
	SetFogHeightScale(3.0f);
	//SetFogColor({ 0.58f,0.69f,0.84f });
	SetFogColor({0.28f,0.4f,0.65f});

//#ifdef DW_MASTER
	//木
	if (isTreeGene) {
		Global_M::g_treeRunner.Init(m_objGene);
	}

	//鉄塔
	//for (int i = 0; i < 3;i++) 
	{
		constexpr float lineLength = 70.0f*50.0f*7.0f*1.5f*2.0f;
		constexpr float offsetLength = 70.0f*50.0f*7.0f*0.15f;
		CQuaternion dirrot(CVector3::AxisY(), CMath::RandomZeroToOne()* CMath::PI2);
		CVector3 gendir = CVector3::Front();
		CVector2 offset = {CMath::Lerp(CMath::RandomZeroToOne(),-offsetLength,offsetLength),CMath::Lerp(CMath::RandomZeroToOne(),-offsetLength,offsetLength) };
		dirrot.Multiply(gendir);
		m_towerGene.LinearGenerate<TransmissionTower>(offset + CVector2(gendir.x, gendir.z)*-lineLength, offset + CVector2(gendir.x, gendir.z)*lineLength, 70.0f*50.0f, 32, 300.0f);
	}

	//ヘリコプター
	{
		constexpr float areaLength = 70.0f*50.0f*7.0f*0.5f;
		m_helicoGene.RectangularGenerate<CHelicopter>({ -areaLength,-70.0f*50.0f,-areaLength }, { areaLength,70.0f*50.0f,areaLength }, CHelicopter::m_sInstancingMax, 1200.0f);
	}
//#endif

	//TODO 木が倒れても影が動かない
	//TODO グラフィック設定で動的シャドウかどうか
	//シャドウマップのベイク
	/*CVector3 forcusPoint = CVector3::Up()*1000.0f - CVector3::AxisX()*5000.0f - CVector3::AxisZ()*5000.0f;
	for (int i = 0; i < 4; i++) {
		m_model.RefreshWorldMatrix();
		m_shadowMapBaker[i].AddDrawModel(m_model);
		for (int i = 0; i < m_treeGene.GetTreeNum(); i++) {
			m_treeGene.GetTreeModel(i).SetIsDraw(true);
			m_treeGene.GetTreeModel(i).PostLoopUpdate();
		}
		m_shadowMapBaker[i].AddDrawModel(m_treeGene.GetTreeModel(0).GetInstancingModel()->GetModelRender());
		m_shadowMapBaker[i].Bake(2048, 2048, m_directionLight.GetDirection(), { 5000.0f,5000.0f,20000.0f }, forcusPoint);
		m_treeGene.GetTreeModel(0).GetInstancingModel()->PreLoopUpdate();
		
		if (i == 0 || i == 2) {
			forcusPoint += CVector3::AxisX()*5000.0f;
		}
		else {
			forcusPoint -= CVector3::AxisX()*5000.0f;
			forcusPoint += CVector3::AxisZ()*5000.0f;
		}
	}*/
	//シャドウマップ
	//m_shadowmap.Init(2,//分割数
	//	m_directionLight.GetDirection(),//ライトの方向
	//	1.0f//シャドウマップの範囲(メインカメラのFarにかかる係数です)
	//);
	//m_shadowmap.SetNear(50.0f);
	//m_shadowmap.SetFar(20000.0f);

	//btSoftBody* rope = btSoftBodyHelpers::CreateRope(*GetPhysicsWorld().GetSoftBodyWorldInfo(),
	//	btVector3(-100, 1200, 0), // ロープの端点1
	//	btVector3(100, 1200, 0), // ロープの端点2
	//	16,   // 分割数(ばねの数)
	//	1 + 2); // 端点の固定フラグ
	//rope->m_cfg.piterations = 4;     // ばねによる位置修正の最大反復回数
	//rope->m_materials[0]->m_kLST = 0.5; // 剛性(Linear Stiffness Coefficient) (変形のしやすさ)
	//rope->setTotalMass(1.0);            // 全体の質量
	//rope->getCollisionShape()->setMargin(0.01);
	//GetPhysicsWorld().GetDynamicWorld()->addSoftBody(rope);
}

//void Shibuya::PostLoopUpdate() {
	//軸
	//CVector3 right; right.Cross(CVector3::AxisY(), CVector3::AxisZ());//右方向
	//DrawLine3D(CVector3::Zero(), right*100.0f, { 1.0f,0.0f,0.0f,1.0f });
	//DrawLine3D(CVector3::Zero(), CVector3::AxisZ().GetCross(right)*100.0f, { 0.0f,1.0f,0.0f,1.0f });//上方向
	//DrawLine3D(CVector3::Zero(), CVector3::AxisZ()*100.0f, { 0.0f,0.0f,1.0f,1.0f });

	//ビュー軸0
	/*
	CVector3 pos = GetMainCamera()->GetPos() + GetMainCamera()->GetFront()*1000.0f;
	DrawLine(pos, GetMainCamera()->GetLeft()*2000.0f + pos, { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(pos, GetMainCamera()->GetLeft().GetCross(GetMainCamera()->GetFront())*2000.0f + pos, { 0.0f,1.0f,0.0f,1.0f });
	*/

	/*
	//視錐台
	GameObj::ICamera* Icam = GetMainCamera();
	GameObj::PerspectiveCamera cam(false);
	cam.SetPos({ 100.0f,1500.0f,-100.0f });
	cam.SetTarget({ 0.0f,1500.0f,0.0f });
	cam.SetFar(150.0f);
	cam.UpdateMatrix();
	SetMainCamera(&cam);

	//視錐台の各方向
	CVector3 vZ = GetMainCamera()->GetFront();
	CVector3 vX; vX.Cross(GetMainCamera()->GetUp(), vZ);
	CVector3 vY; vY.Cross(vZ, vX);

	//アスペクト比と視野角の取得
	float aspect = GetMainCamera()->GetAspect();
	float fov = GetMainCamera()->GetFOV();

	//近平面の高さと幅
	float nearPlaneHalfHeight = tanf(fov * 0.5f) * GetMainCamera()->GetNear();
	float nearPlaneHalfWidth = nearPlaneHalfHeight * aspect;

	//遠平面の高さと幅
	float farPlaneHalfHeight = tanf(fov * 0.5f) * GetMainCamera()->GetFar();
	float farPlaneHalfWidth = farPlaneHalfHeight * aspect;

	//近・遠平面の中心座標
	CVector3 nearPlaneCenter = GetMainCamera()->GetPos() + vZ * GetMainCamera()->GetNear();
	CVector3 farPlaneCenter = GetMainCamera()->GetPos() + vZ * GetMainCamera()->GetFar();

	//視錐台の頂点を求める
	CVector3 corners[8];

	corners[0] = nearPlaneCenter - vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight;
	corners[1] = nearPlaneCenter - vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;
	corners[2] = nearPlaneCenter + vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;
	corners[3] = nearPlaneCenter + vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight;

	corners[4] = farPlaneCenter - vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;
	corners[5] = farPlaneCenter - vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
	corners[6] = farPlaneCenter + vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
	corners[7] = farPlaneCenter + vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;

	DrawLine(corners[0], corners[1], { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(corners[0], corners[3], { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(corners[2], corners[1], { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(corners[2], corners[3], { 1.0f,0.0f,0.0f,1.0f });

	DrawLine(corners[4], corners[5], { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(corners[4], corners[7], { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(corners[6], corners[5], { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(corners[6], corners[7], { 1.0f,0.0f,0.0f,1.0f });

	DrawLine(corners[4], corners[0], { 1.0f,1.0f,0.0f,1.0f });
	DrawLine(corners[5], corners[1], { 1.0f,1.0f,0.0f,1.0f });
	DrawLine(corners[6], corners[2], { 1.0f,1.0f,0.0f,1.0f });
	DrawLine(corners[7], corners[3], { 1.0f,1.0f,0.0f,1.0f });

	//視錐台の各平面を求める
	//※法線は内向き
	CVector3 planes[6][2];//法線・平面上の一点
	CVector3 a, b, c;
	for (int i = 0; i < 6; i++) {
		if (i == 0) {//近平面
			a = nearPlaneCenter + vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;//++
			b = nearPlaneCenter + vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight;//+-
			c = nearPlaneCenter - vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;//-+
		}
		if (i == 1) {//遠平面
			a = farPlaneCenter - vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;//-+
			b = farPlaneCenter - vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;//--
			c = farPlaneCenter + vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;//++
		}
		if (i == 2) {//右
			a = farPlaneCenter + vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
			b = farPlaneCenter + vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;
			c = nearPlaneCenter + vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;
		}
		if (i == 3) {//左
			a = farPlaneCenter - vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;
			b = farPlaneCenter - vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
			c = nearPlaneCenter - vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight;
		}
		if (i == 4) {//下
			a = farPlaneCenter + vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
			b = nearPlaneCenter + vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;
			c = farPlaneCenter - vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
		}
		if (i == 5) {//上
			a = farPlaneCenter - vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;
			b = nearPlaneCenter - vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight;
			c = farPlaneCenter + vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;
		}

		planes[i][0].Cross(b - a, c - a); planes[i][0] *= -1.0f; planes[i][0].Normalize();//法線
		planes[i][1] = a + b + c;//平面上の一点
		planes[i][1] /= 3.0f;

		DrawLine(planes[i][1], planes[i][1]+ planes[i][0]*50.0f, { 0.0f,1.0f,1.0f,1.0f });
		
		DrawLine(planes[i][1] + planes[i][0] * 50.0f + CVector3::AxisX()*10.0f, planes[i][1] + planes[i][0] * 50.0f - CVector3::AxisX()*10.0f, { 0.0f,0.0f,1.0f,1.0f });
		DrawLine(planes[i][1] + planes[i][0] * 50.0f + CVector3::AxisY()*10.0f, planes[i][1] + planes[i][0] * 50.0f - CVector3::AxisY()*10.0f, { 0.0f,0.0f,1.0f,1.0f });
		DrawLine(planes[i][1] + planes[i][0] * 50.0f + CVector3::AxisZ()*10.0f, planes[i][1] + planes[i][0] * 50.0f - CVector3::AxisZ()*10.0f, { 0.0f,0.0f,1.0f,1.0f });
		
		DrawLine(planes[i][1] + CVector3::AxisX()*5.0f, planes[i][1] - CVector3::AxisX()*5.0f, { 1.0f,0.0f,1.0f,1.0f });
		DrawLine(planes[i][1] + CVector3::AxisY()*5.0f, planes[i][1] - CVector3::AxisY()*5.0f, { 1.0f,0.0f,1.0f,1.0f });
		DrawLine(planes[i][1] + CVector3::AxisZ()*5.0f, planes[i][1] - CVector3::AxisZ()*5.0f, { 1.0f,0.0f,1.0f,1.0f });
	}

	//Box
	CVector3 aabb[2] = { {-100.0f,-100.0f + 1500.0f,-100.0f},{100.0f,100.0f + 1500.0f,100.0f} };
	DrawLine(aabb[0], aabb[1], { 1.0f,1.0f*FrustumCulling::AABBTest(GetMainCamera(), aabb[0], aabb[1]),0.0f,1.0f });

	SetMainCamera(Icam);
	*/
//}