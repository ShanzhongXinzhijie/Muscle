#pragma once

//LOD(Level of Detail)

class ILODObj {
public:
	virtual void SetIsDraw(bool isDraw) = 0;
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

class LODSwitcher : public IGameObject {
public:
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
	//TODO ���̃^�C�~���O��SetISDraw�X�V���Ă����łɃ����_�[�ɓo�^�ς�
	//		�����_�[�o�^���O�ɂ��ׂẴJ������LOD����@or �K���o�^
	//		�`�掞��GetIsDraw����
	//		�C���X�^���Q��Ƃ����������
	//		�C���X�^���Q��f����isdraw�z��������Đݒ�@�C���X�^���V���O���f���̃����o�ɃC���f�b�N�X�i���o�[
	//TODO ��ʕ����Ȃ��ƌĂ΂�Ȃ�
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

private:
	CVector3 m_pos;//�I�u�W�F�N�g�̈ʒu
	std::multimap<float, ILODObj*> m_lodObjectList;
};