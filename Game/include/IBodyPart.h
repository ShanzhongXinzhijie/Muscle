#pragma once
#include"CDeathHotoke.h"

class IBodyPart
{
public:
	IBodyPart(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore) {}
	virtual ~IBodyPart() {}

	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Draw2D() { };

protected:
	CDeathHotoke* m_ptrCore = nullptr;
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