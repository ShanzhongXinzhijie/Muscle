#include "stdafx.h"
#include "TreeGene.h"
#include "CDeathHotoke.h"

/// <summary>
/// ��
/// </summary>
/*void Stone::Init(const CVector3& pos, const CVector3& normal) {
	float scale = 1.0f;
	scale = (CMath::RandomZeroToOne() > 0.5f ? 0.05f : 0.1f)*(0.5f + CMath::RandomZeroToOne());
	switch (CMath::RandomInt()%100)
	{
	case 0:
		m_model.Init(m_sInstancingMax, L"Resource/modelData/stone_square.cmo", nullptr, 0, enFbxUpAxisY);
		break;
	case 1:
		m_model.Init(m_sInstancingMax, L"Resource/modelData/stone_tri.cmo", nullptr, 0, enFbxUpAxisY);
		break;
	case 2:
		m_model.Init(m_sInstancingMax, L"Resource/modelData/stone_pillar.cmo", nullptr, 0, enFbxUpAxisY);
		break;
	case 3:
		m_model.Init(m_sInstancingMax, L"Resource/modelData/minitree.cmo", nullptr, 0, enFbxUpAxisY);
		break;
	case 4:
		m_model.Init(m_sInstancingMax, L"Resource/modelData/tettou.cmo");
		scale = 1.4f;
		m_model.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
			[&](MaterialSetting* me) {
				me->SetEmissive(0.0f);
				me->SetMetallic(0.5f);
				//me->SetShininess(0.5f);
			}
		);
		break;
	default:
		//return;
		m_model.Init(m_sInstancingMax, L"Resource/modelData/sigemi.cmo", nullptr, 0, enFbxUpAxisY);
		break;
	}
	m_model.SetPos(pos);
	CQuaternion rot(CVector3::AxisX(), CMath::RandomZeroToOne()*CMath::PI2);
	rot.Concatenate(CQuaternion(CVector3::AxisY(), CMath::RandomZeroToOne()*CMath::PI2));
	rot.Concatenate(CQuaternion(CVector3::AxisZ(), CMath::RandomZeroToOne()*CMath::PI2));
	//m_model.SetRot(rot);
	m_model.SetScale(scale);
	m_model.SetIsDraw(true);
	m_model.GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);
	m_model.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetEmissive(0.0f);
		}
	);
}*/

/// <summary>
/// �S��
/// </summary>
void TransmissionTower::Init(const CVector3& pos, const CVector3& normal) {
	m_model.Init(L"Resource/modelData/tettou.cmo");
	m_model.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetMetallic(0.5f);
			me->SetShininess(0.4f);
			//me->SetAlbedoScale({ 1.0f,1.0f, 1.0f, 1.0f });
		}
	);
	m_model.SetPos(pos);
	m_model.SetRot(CQuaternion(CVector3::AxisY(), CMath::RandomZeroToOne()*CMath::PI2));
	m_model.SetScale(1.4f*0.4f*2.0f);

	//�S����4�̑��ō�������	
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
		//�n�`�Ɣ���
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			//�y��ݒu
			/*m_dodai[i] = std::make_unique<GameObj::CSkinModelRender>();
			m_dodai[i]->Init(L"Resource/modelData/beam.cmo");
			m_dodai[i]->SetPos(gnd_ray.m_hitPointWorld);
			m_dodai[i]->SetRot(m_model.GetRot());
			m_dodai[i]->SetScale(15.0f);*/

			//���f�����W���ڐG�_���Ⴂ�ʒu�Ȃ�
			if (gnd_ray.m_hitPointWorld.y() < setpos.y) {
				//�ڐG�_�����W��
				setpos.y = gnd_ray.m_hitPointWorld.y();
			}
		}
	}
	setpos.y -= 5.0f;
	m_model.SetPos(setpos);

	//���C���[�ʂ�
	SetName(L"TransmissionTower");
	QueryGOs<TransmissionTower>(L"TransmissionTower", 
		[&](auto* go) {
			if (go == this) { return true; }
			if (!go->IsWired()) {
				//���C���[�쐬
				for (int i = 0; i < 6; i++) {
					CVector3 start, end;
					switch (i)
					{
					case 0:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire0"));
						end = go->GetModel().GetBonePos(m_model.FindBoneID(L"Wire0"));
						break;
					case 1:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire1"));
						end = go->GetModel().GetBonePos(m_model.FindBoneID(L"Wire1"));
						break; 
					case 2:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire2"));
						end = go->GetModel().GetBonePos(m_model.FindBoneID(L"Wire2"));
						break;
					case 3:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire3"));
						end = go->GetModel().GetBonePos(m_model.FindBoneID(L"Wire3"));
						break; 
					case 4:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire4"));
						end = go->GetModel().GetBonePos(m_model.FindBoneID(L"Wire4"));
						break;
					case 5:
						start = m_model.GetBonePos(m_model.FindBoneID(L"Wire5"));
						end = go->GetModel().GetBonePos(m_model.FindBoneID(L"Wire5"));
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
/// ��
/// </summary>
namespace {
	constexpr float GRASS_VIEW_DISTANCE_XZ = 400.0f;
	constexpr float GRASS_VIEW_DISTANCE_XZ_SQ = CMath::Square(GRASS_VIEW_DISTANCE_XZ);
	constexpr float GRASS_VIEW_HEIGHT = 200.0f;
}
bool Grass::Start(){
	//���f��������
	GameObj::CInstancingModelRender& insModel = m_model;
	bool isType2 = CMath::RandomZeroToOne() > 0.5f;
	if (isType2) {
		insModel.Init(m_sInstancingMax, L"Resource/modelData/pinGrass2.cmo", nullptr, 0, enFbxUpAxisY);
	}
	else {
		insModel.Init(m_sInstancingMax, L"Resource/modelData/pinGrass.cmo", nullptr, 0, enFbxUpAxisY);
	}
	//��]
	CQuaternion rot(CVector3::AxisX(), -CMath::PI_HALF*0.9f + CMath::PI_HALF*0.9f*2.0f*CMath::RandomZeroToOne());
	rot.Concatenate(CQuaternion(CVector3::AxisY(), -CMath::PI2 + CMath::PI2*2.0f*CMath::RandomZeroToOne()));
	insModel.SetRot(rot);
	//�g��
	insModel.SetScale((CMath::RandomZeroToOne()*0.0015f + 0.003f)*(isType2 ? 1.5f : 1.0f));
	//insModel.SetIsDraw(false);
	//insModel.GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);
	//insModel.GetInstancingModel()->GetModelRender().InitPostDraw(PostDrawModelRender::enAlpha);// , false, true);//�|�X�g�h���[(�\�t�g�p�[�e�B�N��)
	//insModel.GetInstancingModel()->GetModelRender().GetSkinModel().SetCullMode(D3D11_CULL_NONE);//�o�b�N�J�����O���Ȃ�	
	//�ގ�
	insModel.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetShininess(0.4f);
			me->SetAlbedoScale({40.0f, 0.0f, 40.0f, 1.0f});
		}
	);
	//�ʒu
	RePos(ViewCameraList().at(m_cameraNum));

	return true;
}
void Grass::RePos(GameObj::ICamera* mainCamera) {
	//�����_���Ȉʒu
	CVector3 pos = CVector3::AxisX()*(GRASS_VIEW_DISTANCE_XZ * CMath::RandomZeroToOne());
	CQuaternion(CVector3::AxisY(), CMath::PI2*CMath::RandomZeroToOne()).Multiply(pos);
	//�J�����𒆐S��
	pos += mainCamera->GetPos();
	//�ړ����x�����炷
	pos += mainCamera->GetPos() - mainCamera->GetPosOld();

	//���C�Ŕ���
	btVector3 rayStart = btVector3(pos.x, 70.0f*50.0f, pos.z);
	btVector3 rayEnd = btVector3(pos.x, -70.0f*50.0f, pos.z);
	btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
	GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
	//�Փ�
	if (gnd_ray.hasHit()) {
		//�ڐG�_�����W��
		pos = gnd_ray.m_hitPointWorld;
	}

	m_model.SetPos(pos);//�Đݒu
	m_model.ResetWorldMatrixOld();//�����[���h�s��̃��Z�b�g
}
void Grass::PostLoopUpdate() {
	//�`���ԏ�����
	m_model.SetIsDraw(true);

	//�J�����Ƃ̋������������̂͋߂��ɐ���������
	//TODO �\�ߍ��W�}�b�s���O����Ă����@���邢�̓t���N�^�� or�@�i�s�����ɐ���
	GameObj::ICamera* mainCamera = ViewCameraList().at(m_cameraNum);
	if (((m_model.GetPos() - mainCamera->GetPos())*CVector3(1.0f,0.0f,1.0f)).LengthSq() > GRASS_VIEW_DISTANCE_XZ_SQ) {
		RePos(mainCamera);
	}
}
void Grass::Pre3DRender(int screenNum) {
	//�w��̃J�����ȊO�ɂ͕`�悵�Ȃ�
	if (screenNum != m_cameraNum || abs(m_model.GetPos().y - GetMainCamera()->GetPos().y) > GRASS_VIEW_HEIGHT) {
		m_model.SetIsDraw(false);
		return;
	}
	else {
		m_model.SetIsDraw(true);
	}	
}

/// <summary>
/// ��
/// </summary>
void Tree::Init(const CVector3& pos, const CVector3& normal){
	m_pos = pos;

	//LOD������
	CVector2 FrustumSize; GetMainCamera()->GetFrustumPlaneSize(2400.0f/3.0f, FrustumSize);//TODO Scale�ɘA��
	m_lodSwitcher.AddDrawObject(&m_model, FrustumSize.y);
	m_lodSwitcher.AddDrawObject(&m_imposter);
	m_lodSwitcher.SetPos(m_pos);

	GameObj::CInstancingModelRender& insModel = m_model.Get();
	CImposter& imposter = m_imposter.Get();

	//�؂̎��
	constexpr wchar_t treeModelFilePath[2][64] = {
		L"Resource/modelData/realTree_S.cmo",
		L"Resource/modelData/realTree2_S.cmo"
	};

	//�o���G�[�V����
	//float sizeScale = 0.5f*(1.0f + CMath::RandomZeroToOne()*1.2f);		//�X�P�[��
	float sizeScale = 0.5f*(1.0f + CMath::RandomZeroToOne()*0.3f);
	float radY = -CMath::PI2 + CMath::PI2*2.0f*CMath::RandomZeroToOne();//��]
	m_rot.SetRotation(CVector3::AxisY(), radY);
	int treeTypeInd = 0;// CMath::RandomZeroToOne() > 0.5f ? 1 : 0;			//���f�����
	
	//TODO
	//�؂̐F�E�n�ʂ̐F�����@�p�[�����m�C�Y

	//�ߌi���f��
	insModel.Init(m_sInstancingMax, treeModelFilePath[treeTypeInd]);
	insModel.SetPos(m_pos);
	insModel.SetRot(m_rot);
	insModel.SetScale(sizeScale);
	insModel.SetIsDraw(false);
	insModel.GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);
	//�t�@�N�g���Ńm�[�}���}�b�v�ǂݍ���
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> leafNormaltex, barkNormaltex, leafTranstex;
	//TextureFactory::GetInstance().Load(L"Resource/normalMap/leaf fern_n.png", nullptr, &leafNormaltex, nullptr, true);
	//TextureFactory::GetInstance().Load(L"Resource/normalMap/bark03_n.png", nullptr, &barkNormaltex, nullptr, true);
	//TextureFactory::GetInstance().Load(L"Resource/translucentMap/leaf fern_t.png", nullptr, &leafTranstex, nullptr, true);
	
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barktex, leaftex;
	TextureFactory::GetInstance().Load(L"Resource/texture/nadeln4.dds", nullptr, &leaftex);
	TextureFactory::GetInstance().Load(L"Resource/texture/stamm2.dds", nullptr, &barktex);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barktexN, leaftexN;
	TextureFactory::GetInstance().Load(L"Resource/normalMap/nadeln4_n.png", nullptr, &leaftexN, nullptr, true);
	TextureFactory::GetInstance().Load(L"Resource/normalMap/stamm2_n.png", nullptr, &barktexN, nullptr, true);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> leaftexT;
	TextureFactory::GetInstance().Load(L"Resource/translucentMap/nadeln4_t.png", nullptr, &leaftexT, nullptr, true);

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

	insModel.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(setMaterial);

	////�}�e���A���ݒ�
	//std::function setMaterial
	//= [&](MaterialSetting* me) {
	//	me->SetShininess(0.1f);
	//	if (me->EqualMaterialName(L"leaf fern")) {
	//		me->SetIsUseTexZShader(true);
	//		me->SetNormalTexture(leafNormaltex.Get());
	//		me->SetTranslucentTexture(leafTranstex.Get());
	//	}
	//	else {
	//		me->SetNormalTexture(barkNormaltex.Get());
	//	}
	//};
	//insModel.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(setMaterial);

	//���f���̍����擾
	CVector3 min, max;
	insModel.GetInstancingModel()->GetModelRender().GetSkinModel().GetBoundingBox(min, max);
	if (insModel.GetInstancingModel()->GetModelRender().GetSkinModel().GetFBXUpAxis() == enFbxUpAxisZ) {
		m_modelHeight = max.z;//Z-UP
		m_modelRadius = (abs(max.x) + abs(max.y) + abs(min.x) + abs(min.y)) / 4.0f;
	}
	else {
		m_modelHeight = max.y;//Y-UP
		m_modelRadius = (abs(max.x) + abs(max.z) + abs(min.x) + abs(min.z)) / 4.0f;
	}

	//���i���f��
	//if (insModel.GetInstancingModel()->GetModelRender().GetSkinModel().EqualModelName(L"tree_notall")) {
	//	imposter.Init(L"Resource/modelData/tree_notall.cmo", { 2048 * 4, 2048 * 4 }, { 69,35 }, m_sInstancingMax);
	//}
	//else {
	if (!imposter.Init(treeModelFilePath[treeTypeInd], m_sInstancingMax)) {
		SkinModel model;
		model.Init(treeModelFilePath[treeTypeInd]);
		model.FindMaterialSetting(setMaterial);//�}�e���A���ݒ�
		imposter.Init(treeModelFilePath[treeTypeInd], model, { 2048 * 2, 2048 * 2 }, { 35,35 }, m_sInstancingMax);
	}
	//}
	imposter.SetPos(m_pos);
	imposter.SetRotY(radY);
	imposter.SetScale(sizeScale);
	imposter.SetIsDraw(true);
	imposter.SetIsShadowCaster(false);

	//�����蔻��
	constexpr float radius = 50.0f;
	//m_col.m_collision.CreateSphere(m_pos + CVector3::AxisY()*radius*sizeScale, {}, radius*sizeScale);
	m_col.m_reference.position = m_pos + CVector3::AxisY()*radius*sizeScale;
	m_col.m_collision.SetIsHurtCollision(true);
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			//�Ȃ��|�����
			if (m_isHited) { return; }//���łɓ|��Ă�
			if (p.EqualName(L"ReferenceCollision")) {
				//�N���X���o��
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				//�|�������Z�o
				CVector3 dir = m_pos - H->position; dir.y = 0.0f; 
				if (dir.LengthSq() > FLT_EPSILON) {
					dir.Normalize();
					float rad = CVector3::AngleOf2NormalizeVector(dir, CVector3::AxisZ());
					float sign = 1.0f; if (CVector2(dir.x,dir.z).Cross({ 0.0f, 1.0f }) < 0.0f) { sign = -1.0f; }
					m_rotOffset = CQuaternion(CVector3::AxisY(), sign*rad) * CQuaternion(CVector3::AxisX(), CMath::DegToRad(80.0f));
					
					//���C�Ŕ���
					CVector3 topPos = m_pos+CVector3::AxisY()*(m_modelHeight * m_model.Get().GetScale().y);
					btVector3 rayStart = topPos;
					CVector3 topPosRotaed = topPos;
					m_rotOffset.Multiply(topPosRotaed);
					btVector3 rayEnd = topPosRotaed + CVector3::AxisY()*(topPosRotaed.y - topPos.y);
					btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
					GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
					if (gnd_ray.hasHit()) {
						CVector3 dir2 = (gnd_ray.m_hitPointWorld + CVector3::AxisY()*m_modelRadius) - m_pos; dir2.Normalize();
						//�p�x
						float rad = CVector3::AngleOf2NormalizeVector(CVector3::AxisY(), dir2);
						if (rad > FLT_EPSILON) {
							//��]��
							CVector3 rotationAxis;
							rotationAxis.Cross(CVector3::AxisY(), dir2);
							rotationAxis.Normalize();
							m_rotOffset.SetRotation(rotationAxis, rad);
						}
						else {
							return;
						}
					}
					//�ݒ�		
					m_model.Get().SetRot(m_rotOffset*m_rot);
					m_isHited = true;
				}
			}
		}
	);
	//m_col.m_collision.SetEnable(false);
	//m_col.IGameObject::SetEnable(false);
}

//void Tree::PostLoopUpdate() {
	
	//TODO ����̖������@�e�J�����Ƃ̋�����
	//if (m_model.Get().GetIsDraw()) {
	//	m_col.m_collision.SetEnable(true);
	//}
	//else {
	//	m_col.m_collision.SetEnable(false);//�����Ɣ����������
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
