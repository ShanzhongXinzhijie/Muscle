#include "stdafx.h"
#include "CShockRing.h"
#include "shaderDefine.h"

CShockRing::CShockRing(
	const CVector3& pos,
	const CVector4& color,
	const CVector3& scale
){
	if (!m_isStaticInited) {//��������
		//�V�F�[�_�����[�h
		D3D_SHADER_MACRO macros[] = { "TEXTURE", "1", "SOFT_PARTICLE", "1", NULL, NULL };
		m_ps.Load("Preset/shader/smoke_u_.fx", "PSMain_ColorChange", Shader::EnType::PS, "TEXTURE+SOFT_PARTICLE", macros);
		//����������
		m_isStaticInited = true;
	}

	//�r���{�[�h�ǂݍ���
	std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>();
	billboard->Init(L"Resource/effect/airblurring3.png", MAX_NUM);
	billboard->GetModel().InitPostDraw(PostDrawModelRender::enAlpha, false, true);//�|�X�g�h���[(�\�t�g�p�[�e�B�N��)
	billboard->GetModel().SetIsShadowCaster(false);
	billboard->GetModel().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetPS(&m_ps);//�V�F�[�_�ݒ�
		}
	);
	//�F
	m_color = color;

	//IInstanceData��ݒ�
	if (!billboard->GetInstancingModel().GetInstancingModel()->GetIInstanceData(L"InstancingShockRingParamManager")) {
		//�V�K�쐬
		billboard->GetInstancingModel().GetInstancingModel()->AddIInstanceData(L"InstancingShockRingParamManager", std::make_unique<InstancingShockRingParamManager>(MAX_NUM));
	}
	//�|�C���^��ݒ�
	billboard->GetInstancingModel().SetParamPtr(&m_color);

	//�X�e�[�^�X
	billboard->SetPos(pos);
	billboard->SetScale(scale);
	billboard->SetRot({ CVector3::AxisZ(),CMath::PI2*CMath::RandomZeroToOne() });
	SetScaling(1.2f);

	//�p�[�e�B�N���Đ�
	Play(std::move(billboard), 6);
}


void InstancingShockRingParamManager::Reset(int instancingMaxNum) {
	m_instanceMax = instancingMaxNum;
	//�L���b�V��
	m_cashe_color = std::make_unique<CVector4[]>(m_instanceMax);
	//�X�g���N�`���[�o�b�t�@
	m_color.Init(m_instanceMax);
}
InstancingShockRingParamManager::InstancingShockRingParamManager(int instancingMaxNum) {
	Reset(instancingMaxNum);
}
void InstancingShockRingParamManager::PreDraw(int instanceNum, int drawInstanceNum, const std::unique_ptr<bool[]>& drawInstanceMask) {
	//�J�����O����ĂȂ����̂̂݃R�s�[
	int drawNum = 0;
	std::unique_ptr<CVector4[]>& data_color = m_color.GetData();
	for (int i = 0; i < instanceNum; i++) {
		if (drawInstanceMask[i]) {
			data_color[drawNum] = m_cashe_color[i];
			drawNum++;
		}
	}
	//StructuredBuffer���X�V
	m_color.UpdateSubresource();
	//�V�F�[�_�[���\�[�X�ɃZ�b�g
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSmokeColor, 1, m_color.GetAddressOfSRV()
	);
}
void InstancingShockRingParamManager::AddDrawInstance(int instanceIndex, const CMatrix& SRTMatrix, const CVector3& scale, void *param) {
	m_cashe_color[instanceIndex] = *(CVector4*)param;
}
void InstancingShockRingParamManager::SetInstanceMax(int instanceMax) {
	if (instanceMax > m_instanceMax) {
		Reset(instanceMax);
	}
}