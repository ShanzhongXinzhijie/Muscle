#include "stdafx.h"
#include "Shibuya.h"
#include "DemolisherWeapon/Graphic/FrustumCulling.h"

Shibuya::Shibuya()
{
	//���C�g�쐬
	m_directionLight.SetDirection(CVector3::AxisZ()*-1.0f);
	m_directionLight.SetColor(CVector3::One() * 0.5f);

	//�X���f��
	m_model.Init(L"Resource/modelData/tikei.cmo");//city
	//m_model.GetSkinModel().FindMaterialSetting([](MaterialSetting* mat) {
		//mat->SetLightingEnable(false);
	//});
	m_model.SetScale(CVector3::One()*50.0f);
	m_phyStaticObject.CreateMesh(m_model);

	//�m�[�}���}�b�v
	ID3D11ShaderResourceView* tex = nullptr, *normaltex = nullptr;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/spriteData/n_land.dds", nullptr, &normaltex);
	hr = DirectX::CreateDDSTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/spriteData/land.dds", nullptr, &tex);
	DW_ERRORBOX(FAILED(hr), "�n�`�̃m�[�}���}�b�v�ǂݍ��݂Ɏ��s");
	
	//���f���ɃV�F�[�_�ƃm�[�}���}�b�v�ݒ�
	m_model.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(normaltex);
			mat->SetAlbedoTexture(tex);
			mat->SetTriPlanarMappingPS();
			mat->SetTriPlanarMappingUVScale(0.02f);
		}
	);

	//�m�[�}���}�b�v�A�����[�X
	if (tex) {
		tex->Release();
	}
	normaltex->Release();

	//m_knight.Init(L"Resource/modelData/knight.cmo");
	//m_knight.SetPos(CVector3::AxisY()*850.0f);
	//m_knight.SetScale(0.4f);
	//m_knight.GetSkinModel().FindMaterial([](ModelEffect* mat) {mat->SetEmissive(4.0f); });
	
	m_dinosaur.Init(L"Resource/modelData/dinosaur.cmo");
	m_dinosaur.SetPos(CVector3::AxisY()*900.0f+ CVector3::AxisX()*50.0f);
	m_dinosaur.SetScale(CVector3::One()*0.09f);

	//��
	m_sky.Init(L"Resource/cubemap/cube2.dds");

	//�؁X
	//float area_min = -8000.0f - 500.0f, area_max = -8000.0f + 500.0f;
	//m_treeGene.Generate({ area_min,-70.0f*500.0f,-500.0f }, { area_max,70.0f*50.0f,500.0f }, 500);
	m_treeGene.Generate({ -70.0f*50.0f,-70.0f*50.0f,-70.0f*50.0f }, { 70.0f*50.0f,70.0f*50.0f,70.0f*50.0f }, 4000);

	//�V���h�E�}�b�v
	//m_shadowmap.Init(3,//������
	//	m_directionLight.GetDirection(),//���C�g�̕���
	//	1.0f//�V���h�E�}�b�v�͈̔�(���C���J������Far�ɂ�����W���ł�)
	//);
	//m_shadowmap.SetNear(50.0f);
	//m_shadowmap.SetFar(20000.0f);

	//�r���{�e�X�g
	m_billboard.Init(L"Resource/spriteData/test.png");
	m_billboard.SetPos(CVector3::Up()*1000.0f);
	m_billboard.SetScale(100.0f);

	m_billboard2.Init(L"Resource/spriteData/test.png",256);
	m_billboard2.SetPos(CVector3::Up()*1200.0f);
	m_billboard2.SetScale(50.0f);

	/*m_imp.Init(L"Resource/modelData/knight.cmo", { 2048*2,2048*2 }, { 19,19 },256);
	m_imp.SetPos(CVector3::Up()*1500.0f+ CVector3::AxisX()*300.0f);
	m_imp.SetScale(0.4f*10.0f);*/

	m_imp2.Init(L"Resource/modelData/knight.cmo", { 2048 * 2,2048 * 2 }, { 19,19 });
	m_imp2.SetPos(CVector3::Up()*1000.0f + CVector3::AxisX()*300.0f);
	m_imp2.SetScale(0.4f*10.0f);

	//m_knight.SetPos(CVector3::AxisY()*1500.0f);
	//m_knight.SetScale(0.4f*10.0f);
}

Shibuya::~Shibuya()
{
}

void Shibuya::PostLoopUpdate() {
	//��
	DrawLine(CVector3::Zero(), CVector3::AxisX()*1000.0f, { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(CVector3::Zero(), CVector3::AxisY()*1000.0f, { 0.0f,1.0f,0.0f,1.0f });
	DrawLine(CVector3::Zero(), CVector3::AxisZ()*1000.0f, { 0.0f,0.0f,1.0f,1.0f });
	
	/*
	//������
	GameObj::ICamera* Icam = GetMainCamera();
	GameObj::PerspectiveCamera cam(false);
	cam.SetPos({ 100.0f,1500.0f,-100.0f });
	cam.SetTarget({ 0.0f,1500.0f,0.0f });
	cam.SetFar(150.0f);
	cam.UpdateMatrix();
	SetMainCamera(&cam);

	//������̊e����
	CVector3 vZ = GetMainCamera()->GetFront();
	CVector3 vX; vX.Cross(GetMainCamera()->GetUp(), vZ);
	CVector3 vY; vY.Cross(vZ, vX);

	//�A�X�y�N�g��Ǝ���p�̎擾
	float aspect = GetMainCamera()->GetAspect();
	float fov = GetMainCamera()->GetFOV();

	//�ߕ��ʂ̍����ƕ�
	float nearPlaneHalfHeight = tanf(fov * 0.5f) * GetMainCamera()->GetNear();
	float nearPlaneHalfWidth = nearPlaneHalfHeight * aspect;

	//�����ʂ̍����ƕ�
	float farPlaneHalfHeight = tanf(fov * 0.5f) * GetMainCamera()->GetFar();
	float farPlaneHalfWidth = farPlaneHalfHeight * aspect;

	//�߁E�����ʂ̒��S���W
	CVector3 nearPlaneCenter = GetMainCamera()->GetPos() + vZ * GetMainCamera()->GetNear();
	CVector3 farPlaneCenter = GetMainCamera()->GetPos() + vZ * GetMainCamera()->GetFar();

	//������̒��_�����߂�
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

	//������̊e���ʂ����߂�
	//���@���͓�����
	CVector3 planes[6][2];//�@���E���ʏ�̈�_
	CVector3 a, b, c;
	for (int i = 0; i < 6; i++) {
		if (i == 0) {//�ߕ���
			a = nearPlaneCenter + vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;//++
			b = nearPlaneCenter + vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight;//+-
			c = nearPlaneCenter - vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;//-+
		}
		if (i == 1) {//������
			a = farPlaneCenter - vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;//-+
			b = farPlaneCenter - vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;//--
			c = farPlaneCenter + vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;//++
		}
		if (i == 2) {//�E
			a = farPlaneCenter + vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
			b = farPlaneCenter + vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;
			c = nearPlaneCenter + vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;
		}
		if (i == 3) {//��
			a = farPlaneCenter - vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;
			b = farPlaneCenter - vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
			c = nearPlaneCenter - vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight;
		}
		if (i == 4) {//��
			a = farPlaneCenter + vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
			b = nearPlaneCenter + vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;
			c = farPlaneCenter - vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
		}
		if (i == 5) {//��
			a = farPlaneCenter - vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;
			b = nearPlaneCenter - vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight;
			c = farPlaneCenter + vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;
		}

		planes[i][0].Cross(b - a, c - a); planes[i][0] *= -1.0f; planes[i][0].Normalize();//�@��
		planes[i][1] = a + b + c;//���ʏ�̈�_
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