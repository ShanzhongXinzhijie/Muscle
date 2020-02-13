#include "stdafx.h"
#include "BodyPartsHeader.h"

void IBodyPart::Start() { 
	InnerStart(); 
	UpdateTRS();
}

void IBodyPart::UpdateTRS() {
	if (m_model) {
		CQuaternion rot = GetFinalRot();
		CVector3 pos = m_localPos;
		rot.Multiply(pos);
		m_model->SetPos(m_ptrCore->GetPos() + pos);
		m_model->SetRot(rot);
		m_model->SetScale(m_ptrCore->GetScale()*m_localScale);
	}
}

CQuaternion IBodyPart::GetFinalRot()const {
	return m_ptrCore->GetRot() * m_localRot;
}

CVector3 IBodyPart::TurnArm(const CVector3& armTargetPos, const CVector3& armTipPos, const CVector3& armAxis, float angleLimitFront, float angleLimitBack, float armSpeed, CVector3& localTargetPos) {
	//目標への方向
	CVector3 targetDir = armTargetPos - armTipPos;
	targetDir.Normalize();

	//Z軸を基準方向へ向けるクォータニオンを作成
	CQuaternion LorRrot; LorRrot.MakeLookToUseXYAxis(armAxis);
	
	//基準方向のローカルターゲット方向を算出
	CVector3 zLocalTargetDir = targetDir; 
	LorRrot.InverseMultiply(zLocalTargetDir);
	//正規化
	CVector3 zLocalTargetDirNorm = zLocalTargetDir; 
	zLocalTargetDirNorm.Normalize();

	//Z軸をローカルターゲット方向へ向けるクォータニオンを作成
	CQuaternion toTargetRot;
	toTargetRot.MakeLookToUseXYAxis(zLocalTargetDir);

	//ローカルターゲット方向とZ軸の回転軸と角度を取得
	CVector3 axis; float angle;
	axis = CVector3::AxisZ();
	axis.Cross(zLocalTargetDir); axis.Normalize();
	angle = CVector3::AngleOf2NormalizeVector(zLocalTargetDirNorm, CVector3::AxisZ());

	//角度制限
	if (m_ptrCore->GetFront().Dot(targetDir) > 0.0f) {
		angle = min(angle, CMath::DegToRad(angleLimitFront));//前方方向
	}
	else {
		angle = min(angle, CMath::DegToRad(angleLimitBack));//後方方向
	}
	toTargetRot.SetRotation(axis, angle);

	//ワールド空間のターゲット方向を算出
	CVector3 outDir = CVector3::AxisZ();
	toTargetRot.Multiply(outDir);
	LorRrot.Multiply(outDir);

	//ワールド座標系におけるターゲット座標求める
	CVector3 worldTargetPos = localTargetPos;
	m_ptrCore->GetRot().Multiply(worldTargetPos);
	worldTargetPos += m_ptrCore->GetPos();

	//腕を動かす
	CVector3 armDir = worldTargetPos - armTipPos; armDir.Normalize();

	if ((outDir - armDir).LengthSq() < CMath::Square(armSpeed)) {
		localTargetPos = outDir;
	}
	else {
		localTargetPos = armDir + (outDir - armDir).GetNorm()*armSpeed;
	}

	localTargetPos *= (armTargetPos - armTipPos).Length();
	localTargetPos += armTipPos;

	//ターゲット座標をローカル座標型に変換
	localTargetPos -= m_ptrCore->GetPos();
	CVector3 calcedLocalTargedPos;
	calcedLocalTargedPos.x = m_ptrCore->GetRight().Dot(localTargetPos);
	calcedLocalTargedPos.y = m_ptrCore->GetUp().Dot(localTargetPos);
	calcedLocalTargedPos.z = m_ptrCore->GetFront().Dot(localTargetPos);
	localTargetPos = calcedLocalTargedPos;

	//計算した腕のターゲット座標を返す
	return m_ptrCore->GetPos() + m_ptrCore->GetRot().GetMultiply(localTargetPos);
}
