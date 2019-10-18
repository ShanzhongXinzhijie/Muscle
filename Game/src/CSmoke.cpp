#include "stdafx.h"
#include "CSmoke.h"
#include "DemolisherWeapon/Graphic/Model/SkinModelShaderConst.h"

bool CSmoke::m_isStaticInited = false;
Shader CSmoke::m_ps;

CSmoke::CSmoke(const CVector3& pos, const CVector3& move, const CVector4& color)
{
	if (!m_isStaticInited) {//��������
		//�f�B�]���u�V�F�[�_�����[�h
		D3D_SHADER_MACRO macros[] = { "TEXTURE", "1", "SOFT_PARTICLE", "1", NULL, NULL };
		m_ps.Load("Preset/shader/modelDisolve.fx", "PSMain_DisolveSozaiNoAzi_ConvertToPMA", Shader::EnType::PS, "TEXTURE+SOFT_PARTICLE", macros);
		
		//����������
		m_isStaticInited = true;
	}

	//�r���{�[�h�ǂݍ���
	std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>();
	billboard->Init(L"Resource/spriteData/smoke.png", MAX_NUM);
	billboard->GetModel().InitPostDraw(PostDrawModelRender::enAlpha,false,true);//�|�X�g�h���[(�\�t�g�p�[�e�B�N��)
	billboard->GetModel().SetIsShadowCaster(false);
	billboard->GetModel().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetAlbedoScale(color);
			mat->SetPS(&m_ps);//�V�F�[�_�ݒ�
		}
	);

	//IInstanceData��ݒ�
	if (!billboard->GetInstancingModel().GetInstancingModel()->GetIInstanceData(L"InstancingSmokeParamManager")) {
		//�V�K�쐬
		billboard->GetInstancingModel().GetInstancingModel()->AddIInstanceData(L"InstancingSmokeParamManager", std::make_unique<InstancingSmokeParamManager>(MAX_NUM));
	}
	//�|�C���^��ݒ�
	billboard->GetInstancingModel().SetParamPtr(&m_t);

	//�X�e�[�^�X
	//TODO
	billboard->SetPos(pos);
	billboard->SetScale(200.0f);
	billboard->SetRot({ CVector3::AxisZ(),CMath::PI2*CMath::RandomZeroToOne() });
	SetMove(move*40);
	SetScaling(1.2f);

	//�p�[�e�B�N���Đ�
	Play(std::move(billboard), m_maxLifeTime);
}
void CSmoke::Update() {
	m_t = 1.0f - (float)GetLifeTime() / m_maxLifeTime;
	SuicideObj::CParticle<CBillboard>::Update();
}


void InstancingSmokeParamManager::Reset(int instancingMaxNum) {
	m_instanceMax = instancingMaxNum;	
	m_cashe = std::make_unique<float[]>(m_instanceMax);
	m_dissolve_t.Init(m_instanceMax);//�X�g���N�`���[�o�b�t�@

}
InstancingSmokeParamManager::InstancingSmokeParamManager(int instancingMaxNum) {
	Reset(instancingMaxNum);
	//�e�N�X�`��
	TextureFactory::GetInstance().Load(L"Resource/texture/colud2.png", nullptr, &m_dissolveTextureView, nullptr, true);
}
void InstancingSmokeParamManager::PreDraw(int instanceNum, int drawInstanceNum, const std::unique_ptr<bool[]>& drawInstanceMask) {
	//�J�����O����ĂȂ����̂̂݃R�s�[
	int drawNum = 0;
	std::unique_ptr<float[]>& data = m_dissolve_t.GetData();
	for (int i = 0; i < instanceNum; i++) {
		if (drawInstanceMask[i]) {
			data[drawNum] = m_cashe[i];
			drawNum++;
		}
	}
	//StructuredBuffer���X�V
	m_dissolve_t.UpdateSubresource();
	//�V�F�[�_�[���\�[�X�ɃZ�b�g
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSkinModelSRVReg_Disolve_t, 1, m_dissolve_t.GetAddressOfSRV()
	);
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSkinModelSRVReg_DisolveTexture, 1, m_dissolveTextureView.GetAddressOf()
	);
}
void InstancingSmokeParamManager::AddDrawInstance(int instanceIndex, const CMatrix& SRTMatrix, const CVector3& scale, void *param) {
	m_cashe[instanceIndex] = *(float*)param;
}
void InstancingSmokeParamManager::SetInstanceMax(int instanceMax) {
	if (instanceMax > m_instanceMax) {
		Reset(instanceMax);
	}
}