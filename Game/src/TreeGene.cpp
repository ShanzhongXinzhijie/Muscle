#include "stdafx.h"
#include "TreeGene.h"
#include "CDeathHotoke.h"
#include "CGameMode.h"
//#include "DemolisherWeapon/physics/CollisionAttr.h"
//#include "DHCollision.h"

namespace Global_M {
	GrassRunner g_grassRunner;
	TreeRunner g_treeRunner;
}

/// <summary>
/// ヘリコプター
/// </summary>
void CHelicopter::Init(const CVector3& pos, const CVector3& normal) {
	//テストモデル
	m_animHeri.Load(L"Resource/animation/herico.tka", true);
	m_model.Get().Init(m_sInstancingMax, L"Resource/modelData/herico.cmo", &m_animHeri, 1);
	m_model.Get().SetScale(0.3f);
	m_model.Get().GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
		[](MaterialSetting* mat) {
			mat->SetAlbedoScale({ 0.01f, 0.01f, 0.05f, 1.0f });
		}
	);
	m_model.Get().SetPos(pos + CVector3::Back()*25.f + CVector3::Up() * (1200.0f*CMath::RandomZeroToOne()+ 700.0f) );
	m_model.Get().SetRot(CQuaternion(CVector3::AxisY(), CMath::PI2*CMath::RandomZeroToOne()));

	//LOD初期化
	m_lodSwitcher.AddDrawObject(&m_model, 8500.0f);
	m_lodSwitcher.AddDrawObject(&m_noDraw);
	m_lodSwitcher.SetPos(m_model.Get().GetPos());

	//当たり判定
	SetPos(m_model.Get().GetPos());
	SetIsStaticObject(true);
	CreateSphere({ 0.0f, 225.0f * 0.5f * m_model.Get().GetScale().y ,0.0f }, {}, 225.0f * m_model.Get().GetScale().y);
	On_OneGroup(enField);
	GetAttributes().set(enPhysical);
	GetAttributes().set(enGraund);
}

/// <summary>
/// 鉄塔
/// </summary>
void TransmissionTower::Init(const CVector3& pos, const CVector3& normal) {
	m_model.Init(L"Resource/modelData/tettou.cmo");
	m_model.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetMetallic(0.5f);
			me->SetShininess(0.4f);
			me->SetAlbedoScale({ 1.04f, 0.04f, 0.04f, 1.0f });
		}
	);
	m_model.SetPos(pos);
	m_model.SetRot(CQuaternion(CVector3::AxisY(), CMath::RandomZeroToOne()*CMath::PI2));
	m_model.SetScale(1.4f*2.0f);

	//鉄塔は4つの足で高さ判定	
	CVector3 setpos = pos;
	for (int i = 0; i < 4; i++) {
		btVector3 rayStart;
		switch (i)
		{
		case 0:
			rayStart = m_model.GetBonePos(m_model.FindBoneID(L"Graund0"));
			break;
		case 1:
			rayStart = m_model.GetBonePos(m_model.FindBoneID(L"Graund1"));
			break;
		case 2:
			rayStart = m_model.GetBonePos(m_model.FindBoneID(L"Graund2")); 
			break;
		case 3:
			rayStart = m_model.GetBonePos(m_model.FindBoneID(L"Graund3")); 
			break;
		default:
			break;
		}
		btVector3 rayEnd = rayStart;
		rayStart.setY(rayStart.y() + 1000.0f);
		rayEnd.setY(rayEnd.y() - 10000.0f);
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		//地形と判定
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			//土台設置
			/*m_dodai[i] = std::make_unique<GameObj::CSkinModelRender>();
			m_dodai[i]->Init(L"Resource/modelData/beam.cmo");
			m_dodai[i]->SetPos(gnd_ray.m_hitPointWorld);
			m_dodai[i]->SetRot(m_model.GetRot());
			m_dodai[i]->SetScale(15.0f);*/

			//モデル座標より接触点が低い位置なら
			if (gnd_ray.m_hitPointWorld.y() < setpos.y) {
				//接触点を座標に
				setpos.y = gnd_ray.m_hitPointWorld.y();
			}
		}
	}
	setpos.y -= 5.0f;
	m_model.SetPos(setpos);

	//当たり判定
	SetPos(setpos);
	SetIsStaticObject(true);
	CreateCapsule({ 0.0f, 686.0f * 0.5f * m_model.GetScale().y ,0.0f }, {}, 70.0f * m_model.GetScale().x, 686.0f * m_model.GetScale().y - (70.0f * m_model.GetScale().x * 2.0f));
	On_OneGroup(enField);
	GetAttributes().set(enPhysical);
	GetAttributes().set(enGraund);

	//ワイヤー通し
	SetName(L"TransmissionTower");
	QueryGOs<TransmissionTower>(L"TransmissionTower", 
		[&](auto* go) {
			if (go == this) { return true; }
			if (!go->IsWired()) {
				//向き変更
				CQuaternion rot;
				rot.MakeLookToUseXYAxis(((m_model.GetPos() - go->GetModel().GetPos())*CVector3(1, 0, 1)).GetNorm());
				m_model.SetRot(rot);
				go->GetModel().SetRot(rot);

				//ワイヤー作成
				for (int i = 0; i < 6; i++) {
					CVector3 start, end;
					switch (i)
					{
					case 0:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire0"));
						end = go->GetModel().GetBonePos(go->GetModel().FindBoneID(L"Wire0"));
						break;
					case 1:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire1"));
						end = go->GetModel().GetBonePos(go->GetModel().FindBoneID(L"Wire1"));
						break; 
					case 2:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire2"));
						end = go->GetModel().GetBonePos(go->GetModel().FindBoneID(L"Wire2"));
						break;
					case 3:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire3"));
						end = go->GetModel().GetBonePos(go->GetModel().FindBoneID(L"Wire3"));
						break; 
					case 4:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire4"));
						end = go->GetModel().GetBonePos(go->GetModel().FindBoneID(L"Wire4"));
						break;
					case 5:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire5"));
						end = go->GetModel().GetBonePos(go->GetModel().FindBoneID(L"Wire5"));
						break; 
					default:
						break;
					}
					m_wire[i] = std::make_unique<GameObj::CSkinModelRender>();
					m_wire[i]->Init(L"Resource/modelData/wire.cmo");
					m_wire[i]->SetPos(start);
					CQuaternion rot;
					rot.MakeLookToUseXYAxis((end - start).GetNorm());
					m_wire[i]->SetRot(rot);
					m_wire[i]->SetScale({0.3f*2.0f,0.3f*2.0f,(start - end).Length()});
					m_wire[i]->GetSkinModel().FindMaterialSetting(
						[&](MaterialSetting* me) {
							me->SetShininess(0.9f);
							me->SetAlbedoScale({ 0.4f, 0.4f, 0.4f, 1.0f });
						}
					);
				}
				go->SetIsWired(true);
				return false;
			}
			return true;
		}
	);
}

/// <summary>
/// 草
/// </summary>
namespace {
	constexpr float GRASS_VIEW_DISTANCE_XZ = 400.0f*4.0f;
	constexpr float GRASS_VIEW_DISTANCE_XZ_SQ = CMath::Square(GRASS_VIEW_DISTANCE_XZ);

	constexpr float GRASS_VIEW_DISTANCE_XZ_NEAR = 200.0f;
	constexpr float GRASS_VIEW_DISTANCE_XZ_SQ_NEAR = CMath::Square(GRASS_VIEW_DISTANCE_XZ_NEAR);

	constexpr float GRASS_VIEW_HEIGHT = 200.0f;
}
void Grass::Start(bool isNear){
	m_isNear = isNear;

	//モデル初期化
	GameObj::CInstancingModelRender& insModel = m_model;
	bool isType2 = CMath::RandomZeroToOne() > 0.5f;
	if (isType2) {
		insModel.Init(m_sInstancingMax, L"Resource/modelData/pinGrass2.cmo", nullptr, 0, enFbxUpAxisY);
	}
	else {
		insModel.Init(m_sInstancingMax, L"Resource/modelData/pinGrass.cmo", nullptr, 0, enFbxUpAxisY);
	}
	//回転
	CQuaternion rot(CVector3::AxisX(), -CMath::PI_HALF*0.9f + CMath::PI_HALF*0.9f*2.0f*CMath::RandomZeroToOne());
	rot.Concatenate(CQuaternion(CVector3::AxisY(), -CMath::PI2 + CMath::PI2*2.0f*CMath::RandomZeroToOne()));
	insModel.SetRot(rot);
	//拡大
	insModel.SetScale((CMath::RandomZeroToOne()*0.0015f + 0.003f)*(isType2 ? 1.5f : 1.0f));
	insModel.SetScale(insModel.GetScale()*CVector3(1.0f, (CMath::Square(CMath::RandomZeroToOne()) * 4.0f + 1.0f), 1.0f));
	//表示
	insModel.SetIsDraw(false);
	//insModel.GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);
	//insModel.GetInstancingModel()->GetModelRender().InitPostDraw(PostDrawModelRender::enAlpha);// , false, true);//ポストドロー(ソフトパーティクル)
	//insModel.GetInstancingModel()->GetModelRender().GetSkinModel().SetCullMode(D3D11_CULL_NONE);//バックカリングしない	
	//材質
	insModel.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetShininess(0.4f);
			me->SetAlbedoScale({ 3.0f, 3.0f, 3.0f, 1.0f });
			//me->SetAlbedoScale({ 40.0f, 0.0f, 40.0f, 1.0f});
		}
	);
	//位置
	RePos(ViewCameraList().at(m_cameraNum),true);
}
void Grass::RePos(GameObj::ICamera* mainCamera, bool isInitSet) {
	CVector3 pos;
	
	//初期化設置
	if (1){//isInitSet) {
		//ランダムな位置
		if (m_isNear) {
			pos = CVector3::AxisX()*(sqrt(CMath::RandomZeroToOne()) * GRASS_VIEW_DISTANCE_XZ_NEAR);
		}
		else {
			pos = CVector3::AxisX()*(sqrt(CMath::RandomZeroToOne()) * GRASS_VIEW_DISTANCE_XZ);
		}
		CQuaternion(CVector3::AxisY(), CMath::PI2*CMath::RandomZeroToOne()).Multiply(pos);
		//カメラを中心に
		pos += mainCamera->GetPos();
		//移動速度分ずらす
		pos += (mainCamera->GetPos() - mainCamera->GetPosOld());		
	}
	else {
		//プレイヤー位置が移動した分ずらす
		//CVector2 geneCenterPos = { mainCamera->GetPos().x, mainCamera->GetPos().z };
		//geneCenterPos -= m_geneCenterPos;

		CVector2 move = { mainCamera->GetPos().x, mainCamera->GetPos().z };
		move -= CVector2(m_model.GetPos().x, m_model.GetPos().z);
		move *= 1.5f;
		pos = m_model.GetPos();
		pos += {move.x, 0.0f, move.y};
	}
	//生成中心記録
	//m_geneCenterPos = { mainCamera->GetPos().x, mainCamera->GetPos().z };

	//レイで判定
	btVector3 rayStart = btVector3(pos.x, 70.0f*50.0f, pos.z);
	btVector3 rayEnd = btVector3(pos.x, -70.0f*50.0f, pos.z);
	btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
	GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
	//衝突
	if (gnd_ray.hasHit()) {
		//接触点を座標に
		pos = gnd_ray.m_hitPointWorld;
	}

	//座標更新
	m_model.SetPos(pos);//再設置
	m_model.ResetWorldMatrixOld();//旧ワールド行列のリセット
}
void Grass::PostLoopUpdate() {
	//描画状態初期化
	m_model.SetIsDraw(true);

	//カメラとの距離が遠いものは近くに生成し直し
	GameObj::ICamera* mainCamera = ViewCameraList().at(m_cameraNum);
	if (((m_model.GetPos() - mainCamera->GetPos())*CVector3(1.0f,0.0f,1.0f)).LengthSq() > (m_isNear ? GRASS_VIEW_DISTANCE_XZ_SQ_NEAR : GRASS_VIEW_DISTANCE_XZ_SQ)) {
		RePos(mainCamera,false);
	}
}
void Grass::Pre3DRender(int screenNum) {
	//指定のカメラ以外には描画しない
	if (screenNum != m_cameraNum || abs(m_model.GetPos().y - GetMainCamera()->GetPos().y) > GRASS_VIEW_HEIGHT) {
		m_model.SetIsDraw(false);
		return;
	}
	else {
		m_model.SetIsDraw(true);
	}	
}

void TreeRunner::Init(StageObjectGenerator& objGene) {
	m_isEnable = true;
	m_enableTreeNum = 0;

	if (!m_isInit) {
		for (int i = 0; i < m_sInstancingMax; i++) {
			CreateTree(CVector3::Zero());
		}
		Disable();
		m_isInit = true;
	}

	m_isEnable = true;
	m_enableTreeNum = 0;

	//森
	constexpr int FOREST_NUM = 5;
	constexpr float FOREST_SIZE = 70.0f*50.0f*0.5f;
	constexpr float GEN_POINT_AREA = 70.0f*50.0f*3.33f;

	//森の中心点作る
	std::vector<CVector2> genPoints;
	CMath::GenerateBlueNoise(FOREST_NUM, -GEN_POINT_AREA, GEN_POINT_AREA, FOREST_SIZE, genPoints);

	int geneInd = 0;
	
	//森生成
	for (const auto& forestPoint : genPoints) {
		geneInd += objGene.CircularPoint(
			[&](const CVector3& pos) {
				CreateTree(pos);
			}
			, { forestPoint.x,0.0f,forestPoint.y }, FOREST_SIZE, 70.0f*50.0f, 4000 / FOREST_NUM - 1, 120.0f);
	}
	
	//木々生成
	CGameMode::TreeNumMode numMode = FindGO<CGameMode>(L"CGameMode")->GetTreeNum();
	int treeNum = 4000 * 6;
	switch (numMode)
	{
	case CGameMode::enZero:
		treeNum = 0;
		break;
	case CGameMode::enLow:
		treeNum /= 3;
		break;
	default:
		break;
	}
	geneInd += objGene.CircularPoint(
		[&](const CVector3& pos) {
			CreateTree(pos);
		}
		, 0.f, 70.0f*50.0f*7.0f*1.5f, 70.0f*50.0f, treeNum, 120.0f);

	//m_enableTreeNum = geneInd;
}

void TreeRunner::CreateTree(const CVector3& pos) {
	//取得
	GameObj::CInstancingModelRender& insModel = m_model[m_enableTreeNum].Get();
	CImposter& imposter = m_imposter[m_enableTreeNum].Get();

	//初期化済みならここまで
	if (m_isInit) {
		//座標設定
		m_lodSwitcher[m_enableTreeNum].SetPos(pos);
		imposter.SetPos(pos);
		insModel.SetPos(pos);
		m_enableTreeNum++;
		return;
	}

	//サイズ
	int type = CMath::IntUniDist(1) + 1;
	float sizeScale = (1 * 0.25f + 0.5f)*(1.0f + CMath::RandomZeroToOne()*0.3f);
	float LODScale = sizeScale / (0.5f*1.15f);

	//LOD初期化
	CVector2 FrustumSize;
	GetMainCamera()->GetFrustumPlaneSize(2400.0f / 3.0f*1.5f, FrustumSize);
	m_lodSwitcher[m_enableTreeNum].AddDrawObject(&m_model[m_enableTreeNum], (FrustumSize.y*LODScale) / 2.0f / tan(GetMainCamera()->GetFOV()*0.5f));
	GetMainCamera()->GetFrustumPlaneSize(2400.0f*8.0f*0.75f, FrustumSize);
	m_lodSwitcher[m_enableTreeNum].AddDrawObject(&m_imposter[m_enableTreeNum], (FrustumSize.y*LODScale) / 2.0f / tan(GetMainCamera()->GetFOV()*0.5f));
	m_lodSwitcher[m_enableTreeNum].AddDrawObject(&m_noDraw);

	//木の種類
	constexpr wchar_t treeModelFilePath[2][64] = {
		L"Resource/modelData/realTree_S.cmo",
		L"Resource/modelData/realTree.cmo"
	};

	//バリエーション
	float radY = -CMath::PI2 + CMath::PI2*2.0f*CMath::RandomZeroToOne();//回転
	CQuaternion rot(CVector3::AxisY(), radY);
	int treeTypeInd = type - 1;//モデル種類

	//近景モデル
	insModel.Init(m_sInstancingMax, treeModelFilePath[treeTypeInd]);
	insModel.SetRot(rot);
	insModel.SetScale(sizeScale);
	insModel.SetIsDraw(false);
	insModel.GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);

	//テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barktex, leaftex;
	TextureFactory::GetInstance().Load(L"Resource/texture/nadeln4.dds", nullptr, &leaftex);
	TextureFactory::GetInstance().Load(L"Resource/texture/stamm2.dds", nullptr, &barktex);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barktexN, leaftexN;
	TextureFactory::GetInstance().Load(L"Resource/normalMap/nadeln4_n.png", nullptr, &leaftexN, nullptr, true);
	TextureFactory::GetInstance().Load(L"Resource/normalMap/stamm2_n.png", nullptr, &barktexN, nullptr, true);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> leaftexT;
	TextureFactory::GetInstance().Load(L"Resource/translucentMap/nadeln4_t.png", nullptr, &leaftexT, nullptr, true);

	//マテリアル設定関数
	std::function setMaterial = [&](MaterialSetting* me) {
		me->SetShininess(0.01f);
		if (me->EqualMaterialName(L"leaves")) {
			me->SetIsUseTexZShader(true);
			me->SetAlbedoTexture(leaftex.Get());
			me->SetNormalTexture(leaftexN.Get());
			me->SetTranslucentTexture(leaftexT.Get());
			me->SetTranslucent(0.8f);
		}
		else {
			me->SetAlbedoTexture(barktex.Get());
			me->SetNormalTexture(barktexN.Get());
		}
	};

	//マテリアル設定
	insModel.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(setMaterial);

	//遠景モデル
	if (!imposter.Init(treeModelFilePath[treeTypeInd], m_sInstancingMax)) {
		//初回ロード
		SkinModel model;
		model.Init(treeModelFilePath[treeTypeInd]);
		model.FindMaterialSetting(setMaterial);//マテリアル設定
		imposter.Init(treeModelFilePath[treeTypeInd], model, { 2048 * 2, 2048 * 2 }, { 35,35 }, m_sInstancingMax);
	}
	imposter.SetRotY(radY);
	imposter.SetScale(sizeScale);
	imposter.SetIsDraw(true);
	imposter.SetIsShadowCaster(false);

	//座標設定
	m_lodSwitcher[m_enableTreeNum].SetPos(pos);
	imposter.SetPos(pos);
	insModel.SetPos(pos);

	m_enableTreeNum++;
}


/// <summary>
/// 木
/// </summary>
//void Tree::Init(const CVector3& pos, const CVector3& normal){
//	//取得
//	GameObj::CInstancingModelRender& insModel = m_model.Get();
//	CImposter& imposter = m_imposter.Get();
//
//	//初期化済みならここまで
//	if (m_isInit) {
//		//座標設定
//		m_lodSwitcher.SetPos(pos);
//		imposter.SetPos(pos);
//		insModel.SetPos(pos);
//		return;
//	}
//
//	//サイズ
//	int type = CMath::IntUniDist(1) + 1;// CMath::IntUniDist(2);
//	float sizeScale = (1*0.25f + 0.5f)*(1.0f + CMath::RandomZeroToOne()*0.3f);
//	float LODScale = sizeScale / (0.5f*1.15f);
//
//	//LOD初期化
//	CVector2 FrustumSize; 
//	GetMainCamera()->GetFrustumPlaneSize(2400.0f/3.0f*1.5f, FrustumSize);
//	m_lodSwitcher.AddDrawObject(&m_model, (FrustumSize.y*LODScale) / 2.0f / tan(GetMainCamera()->GetFOV()*0.5f));
//	GetMainCamera()->GetFrustumPlaneSize(2400.0f*8.0f*0.75f, FrustumSize);
//	m_lodSwitcher.AddDrawObject(&m_imposter, (FrustumSize.y*LODScale) / 2.0f / tan(GetMainCamera()->GetFOV()*0.5f));
//	m_lodSwitcher.AddDrawObject(&m_noDraw);
//	
//	//木の種類
//	constexpr wchar_t treeModelFilePath[2][64] = {
//		L"Resource/modelData/realTree_S.cmo",
//		L"Resource/modelData/realTree.cmo"
//	};
//
//	//バリエーション
//	float radY = -CMath::PI2 + CMath::PI2*2.0f*CMath::RandomZeroToOne();//回転
//	CQuaternion rot(CVector3::AxisY(), radY);
//	int treeTypeInd = type-1;// CMath::RandomZeroToOne() > 0.5f ? 1 : 0;			//モデル種類
//	
//	//近景モデル
//	insModel.Init(m_sInstancingMax, treeModelFilePath[treeTypeInd]);
//	insModel.SetRot(rot);
//	insModel.SetScale(sizeScale);
//	insModel.SetIsDraw(false);
//	insModel.GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);
//	
//	//テクスチャ
//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barktex, leaftex;
//	TextureFactory::GetInstance().Load(L"Resource/texture/nadeln4.dds", nullptr, &leaftex);
//	TextureFactory::GetInstance().Load(L"Resource/texture/stamm2.dds", nullptr, &barktex);
//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barktexN, leaftexN;
//	TextureFactory::GetInstance().Load(L"Resource/normalMap/nadeln4_n.png", nullptr, &leaftexN, nullptr, true);
//	TextureFactory::GetInstance().Load(L"Resource/normalMap/stamm2_n.png", nullptr, &barktexN, nullptr, true);
//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> leaftexT;
//	TextureFactory::GetInstance().Load(L"Resource/translucentMap/nadeln4_t.png", nullptr, &leaftexT, nullptr, true);
//
//	//マテリアル設定関数
//	std::function setMaterial = [&](MaterialSetting* me) {
//		me->SetShininess(0.01f);
//		if (me->EqualMaterialName(L"leaves")) {
//			me->SetIsUseTexZShader(true);
//			me->SetAlbedoTexture(leaftex.Get());
//			me->SetNormalTexture(leaftexN.Get());
//			me->SetTranslucentTexture(leaftexT.Get());
//			me->SetTranslucent(0.8f);
//		}
//		else {
//			me->SetAlbedoTexture(barktex.Get());
//			me->SetNormalTexture(barktexN.Get());
//		}
//	};
//
//	//マテリアル設定
//	insModel.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(setMaterial);
//
//	//モデルの高さ取得
//	//CVector3 min, max;
//	//insModel.GetInstancingModel()->GetModelRender().GetSkinModel().GetBoundingBox(min, max);
//	//if (insModel.GetInstancingModel()->GetModelRender().GetSkinModel().GetFBXUpAxis() == enFbxUpAxisZ) {
//	//	m_modelHeight = max.z;//Z-UP
//	//	m_modelRadius = (abs(max.x) + abs(max.y) + abs(min.x) + abs(min.y)) / 4.0f;
//	//}
//	//else {
//	//	m_modelHeight = max.y;//Y-UP
//	//	m_modelRadius = (abs(max.x) + abs(max.z) + abs(min.x) + abs(min.z)) / 4.0f;
//	//}
//
//	//遠景モデル
//	if (!imposter.Init(treeModelFilePath[treeTypeInd], m_sInstancingMax)) {
//		//初回ロード
//		SkinModel model;
//		model.Init(treeModelFilePath[treeTypeInd]);
//		model.FindMaterialSetting(setMaterial);//マテリアル設定
//		imposter.Init(treeModelFilePath[treeTypeInd], model, { 2048 * 2, 2048 * 2 }, { 35,35 }, m_sInstancingMax);
//	}	
//	imposter.SetRotY(radY);
//	imposter.SetScale(sizeScale);
//	imposter.SetIsDraw(true);
//	imposter.SetIsShadowCaster(false);
//
//	//当たり判定
//	/*constexpr float radius = 50.0f;
//	//m_col.m_collision.CreateSphere(m_pos + CVector3::AxisY()*radius*sizeScale, {}, radius*sizeScale);
//	m_col.m_reference.position = m_pos + CVector3::AxisY()*radius*sizeScale;
//	m_col.m_collision.SetIsHurtCollision(true);
//	m_col.SetCollisionCallback(
//		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
//			//なぎ倒される
//			if (m_isHited) { return; }//すでに倒れてる
//			if (p.EqualName(L"ReferenceCollision")) {
//				//クラス取り出す
//				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
//				//倒れる方向算出
//				CVector3 dir = m_pos - H->position; dir.y = 0.0f; 
//				if (dir.LengthSq() > FLT_EPSILON) {
//					dir.Normalize();
//					float rad = CVector3::AngleOf2NormalizeVector(dir, CVector3::AxisZ());
//					float sign = 1.0f; if (CVector2(dir.x,dir.z).Cross({ 0.0f, 1.0f }) < 0.0f) { sign = -1.0f; }
//					m_rotOffset = CQuaternion(CVector3::AxisY(), sign*rad) * CQuaternion(CVector3::AxisX(), CMath::DegToRad(80.0f));
//					
//					//レイで判定
//					CVector3 topPos = m_pos+CVector3::AxisY()*(m_modelHeight * m_model.Get().GetScale().y);
//					btVector3 rayStart = topPos;
//					CVector3 topPosRotaed = topPos;
//					m_rotOffset.Multiply(topPosRotaed);
//					btVector3 rayEnd = topPosRotaed + CVector3::AxisY()*(topPosRotaed.y - topPos.y);
//					btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
//					GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
//					if (gnd_ray.hasHit()) {
//						CVector3 dir2 = (gnd_ray.m_hitPointWorld + CVector3::AxisY()*m_modelRadius) - m_pos; dir2.Normalize();
//						//角度
//						float rad = CVector3::AngleOf2NormalizeVector(CVector3::AxisY(), dir2);
//						if (rad > FLT_EPSILON) {
//							//回転軸
//							CVector3 rotationAxis;
//							rotationAxis.Cross(CVector3::AxisY(), dir2);
//							rotationAxis.Normalize();
//							m_rotOffset.SetRotation(rotationAxis, rad);
//						}
//						else {
//							return;
//						}
//					}
//					//設定		
//					m_model.Get().SetRot(m_rotOffset*m_rot);
//					m_isHited = true;
//				}
//			}
//		}
//	);*/
//	//m_col.m_collision.SetEnable(false);
//	//m_col.IGameObject::SetEnable(false);
//
//	//座標設定
//	m_lodSwitcher.SetPos(pos);
//	imposter.SetPos(pos);
//	insModel.SetPos(pos);
//
//	m_isInit = true;
//}

//void Tree::PostLoopUpdate() {
	
	//TODO 判定の無効化　各カメラとの距離で
	//if (m_model.Get().GetIsDraw()) {
	//	m_col.m_collision.SetEnable(true);
	//}
	//else {
	//	m_col.m_collision.SetEnable(false);//遠いと判定も無効化
	//	if (m_isHited) {
	//		m_isHited = false;
	//		m_rotOffset = CQuaternion::Identity();
	//		m_model.Get().SetRot(m_rotOffset*m_rot);
	//	}
	//}

	/*if (!GetKeyInput(VK_TAB)) {
		insModel.SetIsDraw(false);
		imposter.SetIsDraw(true);
	}*/
//}
