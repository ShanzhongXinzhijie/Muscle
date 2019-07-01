#pragma once
#include"CDeathHotoke.h"

class IBodyPart
{
public:
	IBodyPart(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore) {}
	virtual ~IBodyPart() {}

	void Start() { InnerStart(); UpdateTRS(); }
	virtual void Update() = 0;
	virtual void UpdateTRS() {
		m_model->SetPos(m_ptrCore->GetPos() + m_localPos);
		m_model->SetRot(m_localRot*m_ptrCore->GetRot());
		m_model->SetScale(m_ptrCore->GetScale()*m_localScale);
	}
	virtual void Draw2D() { };

private:
	virtual void InnerStart() = 0;
	
protected:
	CDeathHotoke* m_ptrCore = nullptr;

	std::unique_ptr<GameObj::CSkinModelRender> m_model;

	CVector3 m_localPos, m_localScale = 1.0f;
	CQuaternion m_localRot;
};

//������
//���t���[���̋���
	//�A�N�V����
	//�ړ�
//�_���[�W

//��
	//�C���^���N�g����
	//�ړ�(���͂�)
	//�G�t�F�N�g