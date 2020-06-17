#pragma once

//LOD(Level of Detail)

/// <summary>
/// LOD�\���؂�ւ��I�u�W�F�N�g
/// </summary>
class ILODObj {
public:
	virtual void SetIsDraw(bool isDraw) = 0;
};
/// <summary>
/// (LODObj)�����\�����Ȃ�
/// </summary>
class LODNothing : public ILODObj {
public:
	void SetIsDraw(bool isDraw)override {}
};
/// <summary>
/// (LODObj)�C���X�^���V���O���f��
/// </summary>
class LODInstancingModel : public ILODObj {
public:
	LODInstancingModel(bool isRegister):m_model(isRegister){}

	GameObj::CInstancingModelRender& Get() {
		return m_model;
	}
	void SetIsDraw(bool isDraw)override {
		m_model.SetIsDraw(isDraw);
	}
private:
	GameObj::CInstancingModelRender m_model;
};
/// <summary>
/// (LODObj)�C���|�X�^�[
/// </summary>
class LODImposter : public ILODObj {
public:
	LODImposter(bool isRegister) :m_imposter(isRegister) {}
	
	CImposter& Get() {
		return m_imposter;
	}
	void SetIsDraw(bool isDraw)override {
		m_imposter.SetIsDraw(isDraw);
	}
private:
	CImposter m_imposter;
};

/// <summary>
/// LOD�؂�ւ��N���X
/// ���C���X�^���V���O���f���ɂ̂ݑΉ�
/// </summary>
class LODSwitcher : public IGameObject {
public:
	using IGameObject::IGameObject;

	//�ʒu���W��ݒ�
	void SetPos(const CVector3& pos) { m_pos = pos; }

	/// <summary>
	/// �`�敨��o�^
	/// </summary>
	/// <param name="maxSize">������̍ő�T�C�Y</param>
	void AddDrawObject(ILODObj* drawer, float maxSize = FLT_MAX) {
		m_lodObjectList.emplace(maxSize, drawer);
	}

	//����
	//�����̃^�C�~���O��SetISDraw�X�V���Ă����łɃ����_�[�ɓo�^�ς�
	//	�`�掞��GetIsDraw����
	//�@�C���X�^���V���O���f���͑Ή��ς�
	void Pre3DRender(int num)override {
		bool isDecided = false;
		for (auto i = m_lodObjectList.begin(); i != m_lodObjectList.end(); ++i) {
			CVector2 frustum;
			GetMainCamera()->GetFrustumPlaneSize(GetMainCamera()->GetFront().Dot(m_pos - GetMainCamera()->GetPos()), frustum);
			if (!isDecided && (i == std::prev(m_lodObjectList.end()) || abs(frustum.y) < i->first)) {
				i->second->SetIsDraw(true);
				isDecided = true;
			}
			else {
				i->second->SetIsDraw(false);
			}
		}
	}
	void PreLoopUpdate()override {
		//�\��������
		for (auto i = m_lodObjectList.begin(); i != m_lodObjectList.end(); ++i) {
			i->second->SetIsDraw(true);
		}
	}

private:
	CVector3 m_pos;//�I�u�W�F�N�g�̈ʒu
	std::multimap<float, ILODObj*> m_lodObjectList;
};