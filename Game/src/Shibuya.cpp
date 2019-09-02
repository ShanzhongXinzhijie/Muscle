#include "stdafx.h"
#include "Shibuya.h"
#include "DemolisherWeapon/Graphic/FrustumCulling.h"

Shibuya::Shibuya() : m_hotoke(nullptr,false)
{
	//ライト作成
	m_directionLight.SetDirection(CVector3::AxisZ()*-1.0f);
	m_directionLight.SetColor(CVector3::One() * 0.5f);

	//街モデル
	m_model.Init(L"Resource/modelData/tikei.cmo");//city
	//m_model.GetSkinModel().FindMaterialSetting([](MaterialSetting* mat) {
		//mat->SetLightingEnable(false);
	//});
	m_model.SetScale(CVector3::One()*50.0f);
	m_phyStaticObject.CreateMesh(m_model);

	//ノーマルマップ
	ID3D11ShaderResourceView* tex = nullptr, *normaltex = nullptr;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/spriteData/n_land.dds", nullptr, &normaltex);
	hr = DirectX::CreateDDSTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/spriteData/land.dds", nullptr, &tex);
	DW_ERRORBOX(FAILED(hr), "地形のノーマルマップ読み込みに失敗");
	
	//モデルにシェーダとノーマルマップ設定
	m_model.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(normaltex);
			mat->SetAlbedoTexture(tex);
			mat->SetTriPlanarMappingPS();
			mat->SetTriPlanarMappingUVScale(0.02f);
			//mat->SetShininess(0.9f);
		}
	);

	//ノーマルマップ、リリース
	if (tex) {
		tex->Release();
	}
	normaltex->Release();

	//雲
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/texture/colud2.png", nullptr, &m_cloudtex);
	int cloud_i = 0;
	for (auto& cloud : m_cloud) {
		cloud.Init(L"Resource/modelData/sphere.cmo");
		DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/texture/colud.png", nullptr, &tex);

		D3D_SHADER_MACRO macros[] = {
			//	"USE_LOCALPOS", "1",
				"CLOUD", "1",
				NULL, NULL
		};
		m_psCloud.Load("Preset/shader/TriPlanarMapping.fx", "PS_TriPlanarMapping", Shader::EnType::PS, "CLOUD", macros);

		cloud.GetSkinModel().FindMaterialSetting(
			[&](MaterialSetting* mat) {
				mat->SetAlbedoTexture(tex);
				mat->SetPS(&m_psCloud);
				mat->SetTriPlanarMappingUVScale(0.0025f);
			}
		);
		cloud.GetSkinModel().SetPreDrawFunction(
			[&](SkinModel*) {
				GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(7, 1, m_cloudtex.GetAddressOf());
			}
		);
		if (tex) { tex->Release(); }

		cloud.SetPos(CVector3::AxisY()*(5200.0f+1000.0f*CMath::RandomZeroToOne()) +CVector3::AxisZ()*700.0f*(float)cloud_i + CVector3::AxisX()*1000.0f*CMath::RandomZeroToOne());
		cloud.SetScale(28.0f+20.0f*CMath::RandomZeroToOne());
		cloud_i++;
	}	

	//m_knight.Init(L"Resource/modelData/knight.cmo");
	//m_knight.SetPos(CVector3::AxisY()*850.0f);
	//m_knight.SetScale(0.4f);
	//m_knight.GetSkinModel().FindMaterial([](ModelEffect* mat) {mat->SetEmissive(4.0f); });
	
	//m_uni.Init(L"Resource/modelData/unityChan.cmo");
	//m_uni.SetPos(CVector3::AxisY()*900.0f + CVector3::AxisX()*50.0f + CVector3::AxisZ()*100.0f);
	//m_uni.SetIsDrawBoundingBox(true);
	//m_uni.SetScale(3.5f); //0.15f 3.5f

	m_dinosaur.Init(L"Resource/modelData/dinosaur.cmo");
	m_dinosaur.SetPos(CVector3::AxisY()*900.0f+ CVector3::AxisX()*50.0f);
	m_dinosaur.SetScale(CVector3::One()*0.09f);

	//空
	m_sky.Init(L"Resource/cubemap/cube2.dds");
	
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
	SetFogDistance(30000.0f);

	//木々
	//float area_min = -8000.0f - 500.0f, area_max = -8000.0f + 500.0f;
	//m_treeGene.Generate({ area_min,-70.0f*500.0f,-500.0f }, { area_max,70.0f*50.0f,500.0f }, 500);
	
/*
	Tree::m_sInstancingMax = 4000;
	m_objGene.Generate<Tree>({ -70.0f*50.0f,-70.0f*50.0f,-70.0f*50.0f }, { 70.0f*50.0f,70.0f*50.0f,70.0f*50.0f }, Tree::m_sInstancingMax);
	
	Stone::m_sInstancingMax = 4000;
	m_objGene.Generate<Stone>({ -70.0f*50.0f,-70.0f*50.0f,-70.0f*50.0f }, { 70.0f*50.0f,70.0f*50.0f,70.0f*50.0f }, Stone::m_sInstancingMax);
*/
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
	m_shadowmap.Init(2,//分割数
		m_directionLight.GetDirection(),//ライトの方向
		1.0f//シャドウマップの範囲(メインカメラのFarにかかる係数です)
	);
	m_shadowmap.SetNear(50.0f);
	m_shadowmap.SetFar(20000.0f);
}

void Shibuya::PostLoopUpdate() {
	//軸
	CVector3 right; right.Cross(CVector3::AxisY(), CVector3::AxisZ());//右方向
	DrawLine(CVector3::Zero(), right*100.0f, { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(CVector3::Zero(), CVector3::AxisZ().GetCross(right)*100.0f, { 0.0f,1.0f,0.0f,1.0f });//上方向
	DrawLine(CVector3::Zero(), CVector3::AxisZ()*100.0f, { 0.0f,0.0f,1.0f,1.0f });

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