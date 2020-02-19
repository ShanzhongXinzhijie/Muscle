#include "stdafx.h"
#include "CSmoke.h"
#include "DemolisherWeapon/Graphic/Model/SkinModelShaderConst.h"
#include "shaderDefine.h"

bool CSmoke::m_isStaticInited = false;
Shader CSmoke::m_ps;

CSmoke::CSmoke(
	const CVector3& pos, const CVector3& move,
	const CVector4& color,
	const CVector3& scale, const CVector3& scaling,
	int time
): m_maxLifeTime(time)
{
	if (!m_isStaticInited) {//��������
		//�f�B�]���u�V�F�[�_�����[�h
		D3D_SHADER_MACRO macros[] = { "TEXTURE", "1", "SOFT_PARTICLE", "1", NULL, NULL };
		m_ps.Load("Preset/shader/smoke_u_.fx", "PSMain_Smoke", Shader::EnType::PS, "TEXTURE+SOFT_PARTICLE", macros);		
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
			mat->SetPS(&m_ps);//�V�F�[�_�ݒ�
		}
	);
	m_param.color = color;

	//IInstanceData��ݒ�
	if (!billboard->GetInstancingModel().GetInstancingModel()->GetIInstanceData(L"InstancingSmokeParamManager")) {
		//�V�K�쐬
		billboard->GetInstancingModel().GetInstancingModel()->AddIInstanceData(L"InstancingSmokeParamManager", std::make_unique<InstancingSmokeParamManager>(MAX_NUM));
	}
	//�|�C���^��ݒ�
	billboard->GetInstancingModel().SetParamPtr(&m_param);

	//�X�e�[�^�X
	billboard->SetPos(pos);
	billboard->SetScale(scale);
	billboard->SetRot({ CVector3::AxisZ(),CMath::PI2*CMath::RandomZeroToOne() });
	SetMove(move);
	SetScaling(scaling);

	//�p�[�e�B�N���Đ�
	Play(std::move(billboard), m_maxLifeTime);
}
void CSmoke::Update() {
	m_param.t = 1.0f - (float)GetLifeTime() / m_maxLifeTime;
	SuicideObj::CParticle<CBillboard>::Update();
}


void InstancingSmokeParamManager::Reset(int instancingMaxNum) {
	m_instanceMax = instancingMaxNum;	
	//�L���b�V��
	m_cashe_t = std::make_unique<float[]>(m_instanceMax);
	m_cashe_color = std::make_unique<CVector4[]>(m_instanceMax);
	//�X�g���N�`���[�o�b�t�@
	m_dissolve_t.Init(m_instanceMax);
	m_color.Init(m_instanceMax);

}
InstancingSmokeParamManager::InstancingSmokeParamManager(int instancingMaxNum) {
	Reset(instancingMaxNum);
	//�e�N�X�`��
	TextureFactory::GetInstance().Load(L"Resource/texture/colud2.png", nullptr, &m_dissolveTextureView, nullptr, true);
}
void InstancingSmokeParamManager::PreDraw(int instanceNum, int drawInstanceNum, const std::unique_ptr<bool[]>& drawInstanceMask) {
	//�J�����O����ĂȂ����̂̂݃R�s�[
	int drawNum = 0;
	std::unique_ptr<float[]>& data_t = m_dissolve_t.GetData();
	std::unique_ptr<CVector4[]>& data_color = m_color.GetData();
	for (int i = 0; i < instanceNum; i++) {
		if (drawInstanceMask[i]) {
			data_t[drawNum] = m_cashe_t[i];
			data_color[drawNum] = m_cashe_color[i];

			//�J�����Ƃ̋������߂����͓����x������
			const CMatrix& worldmat = GetDrawInstanceWorldMatrix(drawNum);
			CVector3 pos = { worldmat.m[3][0], worldmat.m[3][1], worldmat.m[3][2] };
			data_color[drawNum].w *= CMath::Clamp((GetMainCamera()->GetPos() - pos).LengthSq() / CMath::Square(10.0f * METER), 0.25f, 1.0f);

			drawNum++;
		}
	}
	//StructuredBuffer���X�V
	m_dissolve_t.UpdateSubresource();
	m_color.UpdateSubresource();
	//�V�F�[�_�[���\�[�X�ɃZ�b�g
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSkinModelSRVReg_Disolve_t, 1, m_dissolve_t.GetAddressOfSRV()
	);
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSmokeColor, 1, m_color.GetAddressOfSRV()
	);
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSkinModelSRVReg_DisolveTexture, 1, m_dissolveTextureView.GetAddressOf()
	);
}
void InstancingSmokeParamManager::AddDrawInstance(int instanceIndex, const CMatrix& SRTMatrix, const CVector3& scale, void *param) {
	SmokeParam* ptr = (SmokeParam*)param;
	m_cashe_t[instanceIndex] = ptr->t;
	m_cashe_color[instanceIndex] = ptr->color;
}
void InstancingSmokeParamManager::SetInstanceMax(int instanceMax) {
	if (instanceMax > m_instanceMax) {
		Reset(instanceMax);
	}
}