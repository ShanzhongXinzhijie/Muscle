#include "stdafx.h"
#include "BeamModel.h"

 const BeamModel::BeamType BeamModel::m_s_beamTypes[BEAM_TYPE_MAXNUM] = {
	{L"White",{1.0f,1.0f,1.0f,1.0f},0.25f},
	{L"BLUE",{0.0f,0.0f,1.0f,1.0f},16.0f},
	{L"Red",{1.0f,0.0f,0.0f,1.0f},8.0f},
	{L"Yellow",{1.0f,1.0f,0.0f,1.0f},16.0f},
	{L"BLOOD",{0.8f,0.0f,0.005f,1.0f},0.0f},
};

 void BeamModel::Init(const wchar_t* beamName, bool isUseInside) {
	 m_isUseInside = isUseInside;
	 m_model = std::make_unique<GameObj::CInstancingModelRender[]>(m_isUseInside ? enBeamModelNum : 1);

	 int i2 = 0;
	 for (int i = 0; i < enBeamModelNum; i++) {
		//������`�悵�Ȃ�
		if (!m_isUseInside && enInSide == i) {
			continue;
		}

		//�r�[���^�C�v�����[�h
		const BeamType* ptrBeamType = nullptr;
		if (i == enInSide) {
			//���r�[��
			ptrBeamType = &m_s_beamTypes[0];
		}
		else {
			//identifiers�Ɩ��O����v����r�[���^�C�v�����[�h
			bool isChanged = false;
			for (auto& type : m_s_beamTypes) {
				if (beamName && wcscmp(type.name, beamName) == 0 || !beamName && !isChanged) {
					ptrBeamType = &type;
					isChanged = true;
				}
			}
			//��v����r�[�����Ȃ���Δ��r�[���ɂ���
			if (!isChanged) { 
				ptrBeamType = &m_s_beamTypes[0]; 
			}
			DW_ERRORBOX(!isChanged, "BeamModel::Init() ����ȐF�͂Ȃ�");
		}

		//���f�������[�h
		const wchar_t* identArray[1] = { ptrBeamType->name };
		m_model[i2].Init(BEAM_MODEL_MAXNUM, L"Resource/modelData/beam.cmo"
			, nullptr, 0, enFbxUpAxisZ, enFbxRightHanded,
			identArray);
		//�O���͔��]�`��
		if (i == enOutSide && m_isUseInside) { m_model[i2].GetInstancingModel()->GetModelRender().SetIsDrawReverse(true); }
		//�e�͗��Ƃ��Ȃ�
		if (m_isUseInside) { m_model[i2].GetInstancingModel()->GetModelRender().SetIsShadowCaster(false); }
		//�}�e���A���ݒ�	
		m_model[i2].GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
			[&](MaterialSetting* mat) {
				if (i == enOutSide) {
					mat->SetAlbedoScale(ptrBeamType->color);//�F�ύX
					mat->SetEmissive(ptrBeamType->emissive);//����
				}
				else {
					mat->SetEmissive(0.25f);//����
				}
				mat->SetLightingEnable(false);//���C�e�B���O���Ȃ�
				if (m_isUseInside) {
					mat->SetIsMotionBlur(false);//���[�V�����u���[�����Ȃ�
				}
				else {
					mat->SetShininess(0.9f);
				}
			}
		);	
		i2++;
	 }
 }

 void BeamModel::UpdateModel(){
	 CQuaternion rot;
	 CVector3 move = m_tipPos - m_rootPos;
	 float moveLength = 0.0f;
	 if (move.LengthSq() > FLT_EPSILON) {
		 moveLength = move.Length();
		 //�N�H�[�^�j�I���쐬
		 rot.MakeLookToUseXYAxis(move);
	 }

	 //�ݒ�
	 int i2 = 0;
	 for (int i = 0; i < enBeamModelNum; i++) {
		 //������`�悵�Ȃ�
		 if (!m_isUseInside && enInSide == i) {
			 continue;
		 }

		 if (i == enOutSide) {
			 m_model[i2].SetScale({ m_radius*1.5f,m_radius*1.5f,m_radius*1.5f + moveLength * 0.5f });
		 }
		 else {
			 m_model[i2].SetScale({ m_radius,m_radius,m_radius + moveLength * 0.5f });
		 }
		 m_model[i2].SetPos(m_rootPos + move * 0.5f);
		 m_model[i2].SetRot(rot);
		 i2++;
	 }
 }

 void BeamModel::Move(const CVector3& moveVec) {
	 CVector3 soutaiPos = m_tipPos;
	 CMatrix viewMat = GetMainCamera()->GetViewMatrixOld();
	 viewMat.Mul(soutaiPos);
	 viewMat = GetMainCamera()->GetViewMatrix();
	 viewMat.Inverse();
	 viewMat.Mul(soutaiPos);

	 m_rootPos = soutaiPos;

	 m_tipPos += moveVec;

	 constexpr float blurScale = 1.0f;// 0.9f;
	 m_rootPos = m_tipPos + (m_rootPos - m_tipPos)*blurScale;// *MotionBlurScale;

	 UpdateModel();
 }