#include "stdafx.h"
#include "BodyPartsHeader.h"

void IBodyPart::Start() { InnerStart(); UpdateTRS(); }
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