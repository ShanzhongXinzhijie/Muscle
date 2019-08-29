#include "stdafx.h"
#include "BeamModel.h"

 const BeamModel::BeamType BeamModel::m_s_beamTypes[BEAM_TYPE_MAXNUM] = {
	   {L"BLUE",{0.0f,0.0f,1.0f,1.0f}},
	   {L"Red",{1.0f,0.0f,0.0f,1.0f}},
	   {L"Yellow",{1.0f,1.0f,0.0f,1.0f}},
};

 void BeamModel::Init(const wchar_t** identifiers) {
	 m_model.Init(BEAM_MODEL_MAXNUM, L"Resource/modelData/beam.cmo"
		 , nullptr, 0, enFbxUpAxisZ, enFbxRightHanded,
		 identifiers);
	 m_model.GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);
	 //色変更
	 bool isChanged = false;
	 for (auto& type : m_s_beamTypes) {
		 if (identifiers && wcscmp(type.name, *identifiers) == 0 || !identifiers && !isChanged) {
			 m_model.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
				 [&](MaterialSetting* mat) {
					 if (mat->EqualMaterialName(L"outside")) {
						 mat->SetAlbedoScale(type.color);
						 mat->SetEmissive(16.0f);
					 }
					 else {
						 mat->SetEmissive(0.25f);
					 }
					 mat->SetLightingEnable(false);
					 mat->SetIsMotionBlur(false);
				 }
			 );
			 isChanged = true;
		 }
	 }
	 DW_ERRORBOX(!isChanged, "BeamModel::Init() こんな色はない");
 }

 void BeamModel::UpdateModel(){
	 m_model.SetPos(m_rootPos);

	 CVector3 move = m_tipPos - m_rootPos;
	 float moveLength = 0.0f;
	 if (move.LengthSq() > FLT_EPSILON) {
		 moveLength = move.Length();
		 //クォータニオン作成
		 CQuaternion rot;
		 rot.MakeLookToUseXYAxis(move);
		 //設定
		 m_model.SetRot(rot);
	 }

	 m_model.SetScale({ m_radius,m_radius,m_radius + moveLength * 0.5f });
 }