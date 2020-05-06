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

Shibuya::Shibuya() //: m_hotoke(-1,nullptr,false,nullptr,std::make_unique<TestAI>(&m_hotoke))
{
	
	/*m_hotoke.SetBodyPart(CDeathHotoke::enHead, std::make_unique<BP_FishHead>(&m_hotoke));
	m_hotoke.SetBodyPart(CDeathHotoke::enArm, std::make_unique<BP_KaniArm>(&m_hotoke));
	m_hotoke.SetBodyPart(CDeathHotoke::enWing, std::make_unique<BP_BirdWing>(&m_hotoke));
	m_hotoke.SetBodyPart(CDeathHotoke::enLeg, std::make_unique<BP_HumanLeg>(&m_hotoke));*/
	

	//Ari* ari = new Ari(CVector3::AxisY()*900.0f + CVector3::AxisX()*50.0f, {});
	//ari->SetTarget(&m_hotoke);

	//ライト作成
	m_directionLight.SetDirection(CVector3::AxisZ()*-1.0f);
	m_directionLight.SetColor(CVector3(0.98f,0.97f,0.91f)*0.5f);
	//m_directionLight.SetColor(CVector3::One() * 0.5f);

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

				//SE
				/*
				float length = min(150.0f, H->velocity.Length()*0.1f);
				switch (CMath::RandomInt() % 3) {
				case 0:
					new GameSE(L"Resource/sound/Hit_Hurt.wav", p.m_collisionPoint, length, -1);
					break;
				case 1:
					new GameSE(L"Resource/sound/Hit_Hurt2.wav", p.m_collisionPoint, length, -1);
					break;
				case 2:
					new GameSE(L"Resource/sound/Hit_Hurt5.wav", p.m_collisionPoint, length, -1);
					break;
				default:
					break;
				}
				*/
			}
		}
	);

	//エリア範囲
	//m_areaWallModel.Init(L"Preset/modelData/sky.cmo");
	//m_areaWallModel.SetScale({ 500.f,50.f,500.f });

	//テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tex, normaltex;	
	TextureFactory::GetInstance().Load(L"Resource/texture/moss3.png", nullptr, tex.ReleaseAndGetAddressOf(), nullptr, true);
	TextureFactory::GetInstance().Load(L"Resource/normalMap/moss3_n.bmp", nullptr, normaltex.ReleaseAndGetAddressOf(), nullptr, true);

	//モデルにシェーダとノーマルマップ設定
	m_graundModel.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(normaltex.Get());
			mat->SetAlbedoTexture(tex.Get());
			mat->SetTriPlanarMappingPS();
			mat->SetTriPlanarMappingUVScale(0.002f);
			mat->SetShininess(0.3f);// m_shinnes);
		}
	);

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

	//中景	
	m_midGraund.Init(L"Resource/modelData/far_graund.cmo");
	m_midGraund.SetRot({ CVector3::AxisY(), CMath::PI_HALF });
	m_midGraund.SetScale(500.0f);
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

	//遠景
	m_farGraund.Init(L"Resource/modelData/far_graund.cmo");
	m_farGraund.SetScale(500.0f);
	//モデルにシェーダとノーマルマップ設定
	m_farGraund.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			//mat->SetNormalTexture(normaltex.Get());
			mat->SetAlbedoTexture(tex.Get());
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

	//雲
	//DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/texture/colud2.png", nullptr, &m_cloudtex);
	//int cloud_i = 0;
	//for (auto& cloud : m_cloud) {
	//	cloud.Init(L"Resource/modelData/sphere.cmo");
	//	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/texture/colud.png", nullptr, tex.ReleaseAndGetAddressOf());

	//	D3D_SHADER_MACRO macros[] = {
	//		//	"USE_LOCALPOS", "1",
	//			"CLOUD", "1",
	//			NULL, NULL
	//	};
	//	m_psCloud.Load("Preset/shader/TriPlanarMapping.fx", "PS_TriPlanarMapping", Shader::EnType::PS, "CLOUD", macros);

	//	cloud.GetSkinModel().FindMaterialSetting(
	//		[&](MaterialSetting* mat) {
	//			mat->SetAlbedoTexture(tex.Get());
	//			mat->SetPS(&m_psCloud);
	//			mat->SetTriPlanarMappingUVScale(0.0025f);
	//		}
	//	);
	//	cloud.GetSkinModel().SetPreDrawFunction(
	//		L"SetCLOUDTEX",
	//		[&](SkinModel*) {
	//			GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(7, 1, m_cloudtex.GetAddressOf());
	//		}
	//	);

	//	cloud.SetPos(CVector3::AxisY()*(5200.0f+1000.0f*CMath::RandomZeroToOne()) +CVector3::AxisZ()*700.0f*(float)cloud_i + CVector3::AxisX()*1000.0f*CMath::RandomZeroToOne());
	//	cloud.SetScale(28.0f+20.0f*CMath::RandomZeroToOne());
	//	cloud_i++;
	//}	

	//m_knight.Init(L"Resource/modelData/knight.cmo");
	//m_knight.SetPos(CVector3::AxisY()*850.0f);
	//m_knight.SetScale(0.4f);
	//m_knight.GetSkinModel().FindMaterial([](ModelEffect* mat) {mat->SetEmissive(4.0f); });
	
	//m_uni.Init(L"Resource/modelData/unityChan.cmo");
	//m_uni.SetPos(CVector3::AxisY()*900.0f + CVector3::AxisX()*50.0f + CVector3::AxisZ()*100.0f);
	//m_uni.SetIsDrawBoundingBox(true);
	//m_uni.SetScale(3.5f); //0.15f 3.5f

	/*m_dinosaur.Init(L"Resource/modelData/dinosaur.cmo");
	m_dinosaur.SetPos(CVector3::AxisY()*900.0f+ CVector3::AxisX()*50.0f);
	m_dinosaur.SetScale(CVector3::One()*0.09f);*/

	//空
	m_sky.Init(L"Resource/cubemap/cube2.dds",-1.0f,false);
	SetAmbientCubeMap(L"Resource/cubemap/cubemap.dds", CVector3::One());
	
	//m_skyModel.Init(L"Resource/modelData/sky.cmo");
	////設定
	//m_skyModel.SetDrawPriority(DRAW_PRIORITY_MAX - 1);
	//m_skyModel.SetIsMostDepth(true);
	//m_skyModel.SetIsShadowCaster(false);
	//m_skyModel.SetScale(100.0f);	
	//m_skyModel.GetSkinModel().FindMaterialSetting(
	//	[&](MaterialSetting* mat) {
	//		mat->SetLightingEnable(false);
	//	}
	//);

	//フォグを有効化
	SetEnableFog(true);
	SetFogDistance(30000.0f*1.5f);
	SetFogHeightScale(3.0f);
	//SetFogColor({ 0.58f,0.69f,0.84f });
	SetFogColor({0.28f,0.4f,0.65f});

#ifdef DW_MASTER

	//森
	constexpr int FOREST_NUM = 5;
	constexpr float FOREST_SIZE = 70.0f*50.0f*0.5f;
	Tree::m_sInstancingMax = 4000 + 1000;// *FOREST_NUM;

	//森の中心点作る
	std::vector<CVector2> genPoints;
	constexpr float GEN_POINT_AREA = 70.0f*50.0f*3.33f;
	CMath::GenerateBlueNoise(FOREST_NUM, -GEN_POINT_AREA, GEN_POINT_AREA, FOREST_SIZE, genPoints);

	//木々生成
	//TODO 木のモデルの描画負荷が高い 判定が重い 画面分割時は判定削除とか
	//IGameObjectとして登録しない
	//それを一個のGOで
	for (const auto& forestPoint : genPoints) {
		m_objGene.CircularGenerate<Tree>({ forestPoint.x,0.0f,forestPoint.y }, FOREST_SIZE, 70.0f*50.0f, 1000 / FOREST_NUM -1, 120.0f);
	}

	m_objGene.CircularGenerate<Tree>(0.f, 70.0f*50.0f*7.0f, 70.0f*50.0f, 4000, 120.0f);

	//鉄塔
	//for (int i = 0; i < 3;i++) {
	{
		constexpr float lineLength = 70.0f*50.0f*7.0f*1.5f;
		constexpr float offsetLength = 70.0f*50.0f*7.0f*0.15f;
		CQuaternion dirrot(CVector3::AxisY(), CMath::RandomZeroToOne()* CMath::PI2);
		CVector3 gendir = CVector3::Front();
		CVector2 offset = {CMath::Lerp(CMath::RandomZeroToOne(),-offsetLength,offsetLength),CMath::Lerp(CMath::RandomZeroToOne(),-offsetLength,offsetLength) };
		dirrot.Multiply(gendir);
		m_objGene.LinearGenerate<TransmissionTower>(offset + CVector2(gendir.x, gendir.z)*-lineLength, offset + CVector2(gendir.x, gendir.z)*lineLength, 70.0f*50.0f, 32, 300.0f);
		//m_objGene.RectangularGenerate<TransmissionTower>({ -70.0f*50.0f*7.0f,-70.0f*50.0f,-70.0f*50.0f*7.0f }, { 70.0f*50.0f*7.0f,70.0f*50.0f,70.0f*50.0f*7.0f }, 32, 300.0f);
	}

	//ヘリコプター
	{
		constexpr float areaLength = 70.0f*50.0f*7.0f*0.5f;
		m_helicoGene.RectangularGenerate<CHelicopter>({ -areaLength,-70.0f*50.0f,-areaLength }, { areaLength,70.0f*50.0f,areaLength }, CHelicopter::m_sInstancingMax, 1200.0f);
	}

#endif

	//GetGraphicsEngine().GetAmbientOcclusionRender().SetEnable(false);

	//Stone::m_sInstancingMax = 4000;
	//m_objGene.Generate<Stone>({ -70.0f*50.0f,-70.0f*50.0f,-70.0f*50.0f }, { 70.0f*50.0f,70.0f*50.0f,70.0f*50.0f }, Stone::m_sInstancingMax);

	//ビルボテスト
	/*m_billboard.Init(L"Resource/spriteData/test.png");
	m_billboard.SetPos(CVector3::Up()*1000.0f);
	m_billboard.SetScale(100.0f);

	m_billboard2.Init(L"Resource/spriteData/test.png",256);
	m_billboard2.SetPos(CVector3::Up()*1200.0f);
	m_billboard2.SetScale(50.0f);*/

	/*m_imp.Init(L"Resource/modelData/knight.cmo", { 2048*2,2048*2 }, { 19,19 },256);
	m_imp.SetPos(CVector3::Up()*1500.0f+ CVector3::AxisX()*300.0f);
	m_imp.SetScale(0.4f*10.0f);*/

	/*m_imp2.Init(L"Resource/modelData/knight.cmo", { 2048 * 2,2048 * 2 }, { 19,19 });
	m_imp2.SetPos(CVector3::Up()*1000.0f + CVector3::AxisX()*300.0f);
	m_imp2.SetScale(0.4f*10.0f);
	m_imp2.SetRotY(CMath::PI2);

	m_knight.SetPos(CVector3::AxisY()*1500.0f);
	m_knight.SetScale(0.4f*10.0f);
	m_knight.SetRot(CQuaternion(CVector3::AxisY(), CMath::PI2));*/

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

//void Shibuya::Update() {
//	int i = 0;
//	for (auto& cloud : m_cloud) {
//		i++;
//		cloud.GetSkinModel().FindMaterialSetting(
//			[&](MaterialSetting* mat) {
//				mat->SetUVOffset({ m_cloudTimer + 0.33f*i , m_cloudTimer + 0.33f*i });
//			}
//		);
//	}
//	m_cloudTimer += 0.0005f;
//	if (m_cloudTimer > 1.0f) { m_cloudTimer -= 1.0f; }
//
//	//TODO 地面を分割して変化あったもののみ更新とか
//	//m_phyStaticObject.GetMeshCollider()->GetMeshShape()->buildOptimizedBvh();
//
//	//if (GetKeyDown(VK_UP)) { m_shinnes += 0.01f; }
//	//if (GetKeyDown(VK_DOWN)) { m_shinnes -= 0.01f; }
//}

//void Shibuya::PostRender() {
//	m_font.DrawFormat(L"%.2f", {0.f,0.3f}, {}, m_shinnes);
//	m_model.GetSkinModel().FindMaterialSetting(
//		[&](MaterialSetting* mat) {
//			if (GetKeyInput(VK_LEFT)) {
//				mat->SetShininess(0.0f);
//			}
//			else {
//				mat->SetShininess(m_shinnes);
//			}
//		}
//	);
//}

void Shibuya::PostLoopUpdate() {
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
}