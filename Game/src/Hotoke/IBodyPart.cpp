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
	//�ڕW�ւ̕���
	CVector3 targetDir = armTargetPos - armTipPos;
	targetDir.Normalize();

	//Z����������֌�����N�H�[�^�j�I�����쐬
	CQuaternion LorRrot; LorRrot.MakeLookToUseXYAxis(armAxis);
	
	//������̃��[�J���^�[�Q�b�g�������Z�o
	CVector3 zLocalTargetDir = targetDir; 
	LorRrot.InverseMultiply(zLocalTargetDir);
	//���K��
	CVector3 zLocalTargetDirNorm = zLocalTargetDir; 
	zLocalTargetDirNorm.Normalize();

	//Z�������[�J���^�[�Q�b�g�����֌�����N�H�[�^�j�I�����쐬
	CQuaternion toTargetRot;
	toTargetRot.MakeLookToUseXYAxis(zLocalTargetDir);

	//���[�J���^�[�Q�b�g������Z���̉�]���Ɗp�x���擾
	CVector3 axis; float angle;
	axis = CVector3::AxisZ();
	axis.Cross(zLocalTargetDir); axis.Normalize();
	angle = CVector3::AngleOf2NormalizeVector(zLocalTargetDirNorm, CVector3::AxisZ());

	//�p�x����
	if (m_ptrCore->GetFront().Dot(targetDir) > 0.0f) {
		angle = min(angle, CMath::DegToRad(angleLimitFront));//�O������
	}
	else {
		angle = min(angle, CMath::DegToRad(angleLimitBack));//�������
	}
	toTargetRot.SetRotation(axis, angle);

	//���[���h��Ԃ̃^�[�Q�b�g�������Z�o
	CVector3 outDir = CVector3::AxisZ();
	toTargetRot.Multiply(outDir);
	LorRrot.Multiply(outDir);

	//���[���h���W�n�ɂ�����^�[�Q�b�g���W���߂�
	CVector3 worldTargetPos = localTargetPos;
	m_ptrCore->GetRot().Multiply(worldTargetPos);
	worldTargetPos += m_ptrCore->GetPos();

	//�r�𓮂���
	CVector3 armDir = worldTargetPos - armTipPos; armDir.Normalize();

	if ((outDir - armDir).LengthSq() < CMath::Square(armSpeed)) {
		localTargetPos = outDir;
	}
	else {
		localTargetPos = armDir + (outDir - armDir).GetNorm()*armSpeed;
	}

	localTargetPos *= (armTargetPos - armTipPos).Length();
	localTargetPos += armTipPos;

	//�^�[�Q�b�g���W�����[�J�����W�^�ɕϊ�
	localTargetPos -= m_ptrCore->GetPos();
	CVector3 calcedLocalTargedPos;
	calcedLocalTargedPos.x = m_ptrCore->GetRight().Dot(localTargetPos);
	calcedLocalTargedPos.y = m_ptrCore->GetUp().Dot(localTargetPos);
	calcedLocalTargedPos.z = m_ptrCore->GetFront().Dot(localTargetPos);
	localTargetPos = calcedLocalTargedPos;

	//�v�Z�����r�̃^�[�Q�b�g���W��Ԃ�
	return m_ptrCore->GetPos() + m_ptrCore->GetRot().GetMultiply(localTargetPos);
}
