#pragma once

//LOD(Level of Detail)

/// <summary>
/// LOD�\���؂�ւ��I�u�W�F�N�g
/// </summary>
class ILODObj {
public:
	virtual void SetIsDraw(bool isDraw) = 0;
};
class LODNothing : public ILODObj {
public:
	void SetIsDraw(bool isDraw)override {}
};
class LODInstancingModel : public ILODObj {
public:
	GameObj::CInstancingModelRender& Get() {
		return m_model;
	}
	void SetIsDraw(bool isDraw)override {
		m_model.SetIsDraw(isDraw);
	}
private:
	GameObj::CInstancingModelRender m_model;
};
class LODImposter : public ILODObj {
public:
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
/// �C���X�^���V���O�`���LOD�������s�N���X
/// </summary>
//class InstancingLOD : public GameObj::InstancingModel::IInstancesData {
//private:
//	//�Ċm��
//	void Reset(int instancingMaxNum) {
//		m_instanceMax = instancingMaxNum;
//		m_isDraw = std::make_unique<bool[]>(instancingMaxNum);
//	}
//public:
//	bool PreCulling(int instanceIndex)override {
//		return m_isDraw[instanceIndex];
//	}
//	void SetInstanceMax(int instanceMax)override {
//		if (instanceMax > m_instanceMax) {
//			Reset(instanceMax);
//		}
//	}
//public:
//	/// <summary>
//	/// �R���X�g���N�^
//	/// </summary>
//	/// <param name="instancingMaxNum">�C���X�^���X�ő吔</param>
//	InstancingLOD(int instancingMaxNum) {
//		Reset(instancingMaxNum);
//	}
//private:
//	//�p�����[�^
//	std::unique_ptr<bool[]> m_isDraw;
//	int m_instanceMax = 0;
//};

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