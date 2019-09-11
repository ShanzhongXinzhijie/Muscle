#pragma once
#include"CDeathHotoke.h"

/// <summary>
/// �{�f�B�p�[�c�R���g���[���[�̐e�N���X
/// </summary>
template <class T> 
class IBodyController {
public:
	//�R���X�g���N�^
	IBodyController(T* ptrbody, CDeathHotoke* ptrCore) : m_ptrBody(ptrbody), m_ptrCore(ptrCore) {}
	//���t���[�����s���鏈��
	virtual void Update() = 0;

protected:
	//���삷��{�f�B�p�[�c�̃|�C���^
	T* m_ptrBody = nullptr;
	//�{�̂̃|�C���^
	CDeathHotoke* m_ptrCore = nullptr;
};

/// <summary>
/// �{�f�B�p�[�c�̐e�N���X
/// </summary>
class IBodyPart
{
public:
	IBodyPart(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore) {}
	virtual ~IBodyPart() {}

	void Start() { InnerStart(); UpdateTRS(); }
	virtual void Update() {};
	virtual void UpdateTRS() {
		if (m_model) {
			m_model->SetPos(m_ptrCore->GetPos() + m_localPos);
			m_model->SetRot(m_localRot*m_ptrCore->GetRot());
			m_model->SetScale(m_ptrCore->GetScale()*m_localScale);
		}
	}
	virtual void PostUTRSUpdate() {};
	virtual void PostLoopUpdate() {};
	virtual void Draw2D() { };

private:
	virtual void InnerStart() = 0;
	
protected:
	//�{�̂̃|�C���^
	CDeathHotoke* m_ptrCore = nullptr;

	//���f��
	std::unique_ptr<GameObj::CSkinModelRender> m_model;

	//���W��
	CVector3 m_localPos, m_localScale = 1.0f;
	CQuaternion m_localRot;
};