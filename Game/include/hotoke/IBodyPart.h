#pragma once

class CDeathHotoke;

/// <summary>
/// �{�f�B�p�[�c�R���g���[���[�̐e�N���X
/// </summary>
template <class T> 
class IBodyController {
public:
	//�R���X�g���N�^
	IBodyController(T* ptrbody, CDeathHotoke* ptrCore) : m_ptrBody(ptrbody), m_ptrCore(ptrCore) {}

	//���t���[�����s���鏈��
	void Update() {
		if (!m_ptrCore->GetIsControl()) {
			return;
		}
		InnerUpdate();
	}
	virtual void InnerUpdate() = 0;

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
	IBodyPart() = default;
	IBodyPart(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore) {};
	virtual ~IBodyPart() {};

	//�������g���쐬���鉼�z�֐�
	virtual IBodyPart* Create()const = 0;

	//������
	void Init(CDeathHotoke* ptrCore) {
		m_ptrCore = ptrCore;
	}
	   
	//�Q�[���I�u�W�F�N�g�֐�
	void Start();
	virtual void Update() {}
	virtual void UpdateTRS();
	virtual void PostUTRSUpdate() {}
	virtual void PostLoopUpdate() {}
	virtual void Draw2D() {}

	//�ŏI�I�ȉ�]�N�H�[�^�j�I�����擾
	CQuaternion GetFinalRot()const;

	//�p�[�c�����擾
	const wchar_t* GetName()const {
		return m_name.c_str();
	}

private:
	//Start�֐����Ŏ��s����鉼�z�֐�
	virtual void InnerStart() = 0;
	
protected:
	//�{�̂̃|�C���^
	CDeathHotoke* m_ptrCore = nullptr;

	//���f��
	std::unique_ptr<GameObj::CSkinModelRender> m_model;

	//���W��
	CVector3 m_localPos, m_localScale = 1.0f;
	CQuaternion m_localRot;

	//���O
	std::wstring m_name = L"UNKNOWN";
};