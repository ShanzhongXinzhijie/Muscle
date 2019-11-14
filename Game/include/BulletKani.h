#pragma once
#include"BeamModel.h"
#include"IFu.h"

class BulletGO;

/// <summary>
/// �o���b�g�R���|�[�l���g
/// </summary>
class IBulletComponent {
public:
	virtual ~IBulletComponent() {}

	virtual void Start() {}
	virtual void PreLoopUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate()  {}
	virtual void PostLoopUpdate() {}
	virtual void Pre3DRender(int) {}
	virtual void Contact(SuicideObj::CCollisionObj::SCallbackParam& p) {}

	//�ݒ茳��ݒ�
	void SetBullet(BulletGO* bullet) {
		m_bullet = bullet;
	}
protected:
	BulletGO* m_bullet = nullptr;
};

/// <summary>
/// �o���b�g�𓮂����N���X
/// </summary>
class BulletGO : public IGameObject, public IFu {
public:
	BulletGO(const CVector3& pos, const CVector3& move);

	void PreLoopUpdate()override;
	void Update()override;
	void PostLoopUpdate()override;
	void Pre3DRender(int)override;

	/// <summary>
	/// �R���|�[�l���g��ǉ�����
	/// </summary>
	void AddComponent(std::unique_ptr<IBulletComponent> component) {
		m_components.emplace_back(std::move(component));
		m_components.back()->SetBullet(this);
		m_components.back()->Start();
	}

	/// <summary>
	/// �����W�̎擾
	/// </summary>
	const CVector3& GetOldPos()const { return m_posOld; }

private:
	//�R���|�[�l���g
	std::vector<std::unique_ptr<IBulletComponent>> m_components;	
	//�Í��W
	CVector3 m_posOld;
public:
	//����
	float m_lifeTime = 3.0f;
	//�i�s�x�N�g��
	CVector3 m_vector;
	//�U���R���W����
	DHCollision m_col;
};

/// <summary>
/// �r�[�����f���R���|�[�l���g
/// ����ʕ�������ύX�����ꍇ��蒼���K�v������
/// </summary>
class BD_BeamModel : public IBulletComponent {
public:
	BD_BeamModel(float radius = 3.0f, std::wstring_view beamName = L"BLUE") :m_radius(radius){
		//���f��
		m_modelNum = GetScreenNum();
		m_model = std::make_unique<BeamModel[]>(m_modelNum);//��ʂ̐������쐬
		for (int i = 0; i < m_modelNum; i++) {
			m_model[i].Init(beamName.data());
			m_model[i].SetRadius(radius);
			m_model[i].SetIsDraw(false);
		}
	}
	void Start()override {
		//���W������
		m_lastDrawPos = m_bullet->GetPos();
		//�U������쐬
		m_bullet->m_col.m_collision.CreateSphere(m_bullet->GetPos(), {}, m_radius);
	}
	void PreLoopUpdate()override {
		//�Ō�ɕ`�悵�����W�X�V
		m_lastDrawPos = m_bullet->GetPos();
		//���f���\��������
		for (int i = 0; i < m_modelNum; i++) {
			m_model[i].SetIsDraw(true);
		}
	}
	void PostLoopUpdate()override{
		//���f���̍X�V(��ʐ���)
		for (int i = 0; i < m_modelNum; i++) {
			CVector3 rootPos, tipPos;
			GameObj::ICamera* screenCamera = m_modelNum == 1 ? GetMainCamera() : GetCameraList()[i];

			//�J�����Ƒ��ΓI�ȍ��W�����߂�(���K��)
			CVector3 soutaiPos = m_lastDrawPos;
			CMatrix viewMat = screenCamera->GetViewMatrixOld();
			viewMat.Mul(soutaiPos);
			viewMat = screenCamera->GetViewMatrix();
			viewMat.Inverse();
			viewMat.Mul(soutaiPos);
			rootPos = soutaiPos;

			//��[���̍��W���߂�
			tipPos = m_lastDrawPos + (m_bullet->GetPos() - m_lastDrawPos);

			//blurScale��K��
			constexpr float blurScale = 1.0f;
			rootPos = tipPos + (rootPos - tipPos)*blurScale;

			//���f���X�V
			m_model[i].SetPos(rootPos, tipPos);
		}		
	}
	void Pre3DRender(int screenNum)override {
		//�\�����郂�f���̐؂�ւ�
		for (int i = 0; i < m_modelNum; i++) {
			m_model[i].SetIsDraw(i == screenNum ? true : false);
		}		
	}
private:	
	int m_modelNum = 0;//���f���̐�
	std::unique_ptr<BeamModel[]> m_model;//���f��
	float m_radius = 3.0f;//���a
	CVector3 m_lastDrawPos;//�Ō�ɕ`�悵�����W
};

/// <summary>
/// �z�[�~���O�R���|�[�l���g
/// </summary>
class BD_Homing : public IBulletComponent {
public:
	//TODO IFu�̎Q�Ɛ؂�
	BD_Homing(const IFu* target = nullptr) :m_target(target) {};

	void Update()override {
		if (!m_target) { return; }
		m_bullet->m_vector = (m_target->GetPos() - m_bullet->GetPos()).GetNorm()*m_bullet->m_vector.Length();
	}

private:
	const IFu* m_target = nullptr;
};

/// <summary>
/// �Փ˂ŏ��ŃR���|�[�l���g
/// </summary>
class BD_Contact : public IBulletComponent {
public:
	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//�N���X���o��
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical]) {
				//���������Ȃ玀
				m_bullet->m_lifeTime = 0.0f;
			}
		}
		if (!p.m_isCCollisionObj) {
			//���肪CCollisionObj����Ȃ��Ă���
			m_bullet->m_lifeTime = 0.0f;
		}
	}
};
/// <summary>
/// �Փ˂Ŕ����R���|�[�l���g
/// </summary>
class BD_ContactExplosion : public IBulletComponent {
public:	
	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//�N���X���o��
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical]) {
				//���������Ȃ玀
				m_bullet->m_lifeTime = 0.0f;
				//TODO ��������
			}
		}
		if (!p.m_isCCollisionObj) {
			//���肪CCollisionObj����Ȃ��Ă���
			m_bullet->m_lifeTime = 0.0f;
			//TODO ��������
		}
	}
};

//TODO ����(MHW) �n�`�m�C�Y