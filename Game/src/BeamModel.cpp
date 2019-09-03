#include "stdafx.h"
#include "BeamModel.h"

 const BeamModel::BeamType BeamModel::m_s_beamTypes[BEAM_TYPE_MAXNUM] = {
	   {L"White",{1.0f,1.0f,1.0f,1.0f}},
	   {L"BLUE",{0.0f,0.0f,1.0f,1.0f}},
	   {L"Red",{1.0f,0.0f,0.0f,1.0f}},
	   {L"Yellow",{1.0f,1.0f,0.0f,1.0f}},
};

 void BeamModel::Init(const wchar_t* beamName) {
	 for (int i = 0; i < enBeamModelNum; i++) {
		//ビームタイプをロード
		const BeamType* ptrBeamType = nullptr;
		if (i == enInSide) {
			//白ビーム
			ptrBeamType = &m_s_beamTypes[0];
		}
		else {
			//identifiersと名前が一致するビームタイプをロード
			bool isChanged = false;
			for (auto& type : m_s_beamTypes) {
				if (beamName && wcscmp(type.name, beamName) == 0 || !beamName && !isChanged) {
					ptrBeamType = &type;
					isChanged = true;
				}
			}
			//一致するビームがなければ白ビームにする
			if (!isChanged) { ptrBeamType = &m_s_beamTypes[0]; }
			DW_ERRORBOX(!isChanged, "BeamModel::Init() こんな色はない");
		}

		//モデルをロード
		const wchar_t* identArray[1] = { ptrBeamType->name };
		m_model[i].Init(BEAM_MODEL_MAXNUM, L"Resource/modelData/beam.cmo"
			, nullptr, 0, enFbxUpAxisZ, enFbxRightHanded,
			identArray);
		//外側は反転描画
		if (i == enOutSide) { m_model[i].GetInstancingModel()->GetModelRender().SetIsDrawReverse(true); }		
		//影は落とさない
		m_model[i].GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);		
		//マテリアル設定	
		m_model[i].GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
			[&](MaterialSetting* mat) {
				if (i == enOutSide) {
					mat->SetAlbedoScale(ptrBeamType->color);//色変更
					mat->SetEmissive(16.0f);//発光
				}
				else {
					mat->SetEmissive(0.25f);//発光
				}
				mat->SetLightingEnable(false);//ライティングしない
				mat->SetIsMotionBlur(false);//モーションブラーかけない
			}
		);			
	 }
 }

 void BeamModel::UpdateModel(){
	 CQuaternion rot;
	 CVector3 move = m_tipPos - m_rootPos;
	 float moveLength = 0.0f;
	 if (move.LengthSq() > FLT_EPSILON) {
		 moveLength = move.Length();
		 //クォータニオン作成
		 rot.MakeLookToUseXYAxis(move);
	 }

	 //設定
	 for (int i = 0; i < enBeamModelNum; i++) {
		 if (i == enOutSide) {
			 m_model[i].SetScale({ m_radius*1.5f,m_radius*1.5f,m_radius*1.5f + moveLength * 0.5f });
		 }
		 else {
			 m_model[i].SetScale({ m_radius,m_radius,m_radius + moveLength * 0.5f });
		 }
		 m_model[i].SetPos(m_rootPos + move * 0.5f);
		 m_model[i].SetRot(rot);
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