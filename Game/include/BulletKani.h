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

	//�R���|�[�l���g���L�����ݒ�
	void SetEnable(bool b) {
		m_enable = b;
	}
	bool GetEnable()const {
		return m_enable;
	}

protected:
	bool m_enable = true;
	BulletGO* m_bullet = nullptr;
};

/// <summary>
/// �o���b�g�𓮂����N���X
/// </summary>
class BulletGO : public ILifeObject {
public:
	BulletGO(const CVector3& pos, const CVector3& move, bool isLockable = false);

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

	/// <summary>
	/// ���t���[���̃x���V�e�B�̕ω����v�Z
	/// </summary>
	static void CalcVelocityUpdate(CVector3& velocity, float gravity);

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
	//���a
	float m_radius = 0.0f;
	//�d��
	float m_gravity = 0.0f;
};


/// <summary>
/// �^�C�}�[�R���|�[�l���g
/// ��莞�Ԍ�ɃR���|�[�l���g��L�����E����������
/// </summary>
/// <param name="timer">�ݒ莞�ԁB���s�܂ł̃t���[����</param>
/// <param name="willEnable">true�Őݒ莞�Ԍ�R���|�[�l���g��L�����Bfalse�Ȃ疳����</param>
class BD_Timer : public IBulletComponent {
public:
	BD_Timer(float timer = 0.0f, bool willEnable = true)
		: m_timerf(timer), m_willEnable(willEnable)
	{};

	void Update()override {
		if (m_timerf < FLT_EPSILON) { return; }
		//�^�C�}�[����
		m_timerf = max(0.0f, m_timerf - 1.0f);
		if (m_timerf < FLT_EPSILON) {
			for (auto& com : m_components) {
				com->SetEnable(m_willEnable);
			}
		}
	}

	/// <summary>
	/// �R���|�[�l���g��ǉ�
	/// </summary>
	void AddComponent(IBulletComponent* comp) {
		if (comp) {
			comp->SetEnable(!m_willEnable);//�L���E�����̐ݒ�ύX
			m_components.emplace_back(comp);
		}
	}

private:
	float m_timerf = 0.0f;
	bool m_willEnable = true;
	std::list<IBulletComponent*> m_components;
};

/// <summary>
/// ���b�N�I���\���R���|�[�l���g
/// </summary>
class BD_Lockable :public IBulletComponent {
	void Start()override {
		m_bullet->SetLockable(true);
	}
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
		m_bullet->m_radius = m_radius;
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
/// �r���f���R���|�[�l���g
/// </summary>
class BD_LegModel : public IBulletComponent {
public:
	//scale�ɂ̓z�g�P�̃X�P�[��������΂����Ǝv����
	BD_LegModel(const CQuaternion& rot, const CVector3& scale) : m_rot(rot), m_scale(scale*5.0f) {}

	void Start()override {
		//���f���쐬
		m_model.Init(L"Resource/modelData/leg.cmo");
		m_model.SetPos(m_bullet->GetPos());
		m_model.SetRot(m_rot);
		m_model.SetScale(m_scale);
		//�U������쐬
		float radius = 50.0f*((m_scale.GetMin() + m_scale.GetMax())*0.5f);
		m_bullet->m_col.m_collision.CreateSphere(m_bullet->GetPos(), {}, radius);
		m_bullet->m_radius = radius;
	}
	void PostLoopUpdate()override {
		//���f���̍X�V
		m_model.SetPos(m_bullet->GetPos());
	}

private:
	CQuaternion m_rot;
	CVector3 m_scale = 1.0f;
	GameObj::CSkinModelRender m_model;//���f��
};

/// <summary>
/// �z�[�~���O�R���|�[�l���g
/// </summary>
class BD_Homing : public IBulletComponent {
public:
	BD_Homing(IFu* target = nullptr, float thrust = 0.0f, float noAccelRad = 0.0f, float timer = 0.0f)
		: m_target(target), m_thrust(thrust), m_nonAccelRad(noAccelRad), m_timerf(timer)
	{
		if (target) { target->SetDeleteFlag(isTargetDeath); }
	};

	void Update()override {
		//�^�C�}�[����
		m_timerf = max(0.0f, m_timerf - 1.0f);
		if (m_timerf > 0.0f) { return; }

		CVector3 targetDir(m_bullet->m_vector);
		
		//�ڕW���� �� �ڕW�̃C���X�^���X�����ł��Ă��Ȃ�
		if (m_target && !isTargetDeath) {
			targetDir = m_target->GetPos() - m_bullet->GetPos();
		}

		targetDir.Normalize();

		//��ɉ����ݒ� or �ڕW�����Ƃ̊p�x���J���Ă���
		if (m_nonAccelRad < FLT_EPSILON || CVector3::AngleOf2NormalizeVector(targetDir,m_bullet->m_vector.GetNorm()) > m_nonAccelRad) {
			CVector3 beforeVec = m_bullet->m_vector;
			m_bullet->m_vector += targetDir * m_thrust;
			if (m_bullet->m_vector.LengthSq() < FLT_EPSILON) {//��~�͂��Ȃ�
				m_bullet->m_vector = beforeVec;
			}
		}
	}

	//���͂�ݒ�
	void SetThrust(float f) {
		m_thrust = f;
	}

private:
	const IFu* m_target = nullptr;//�ڕW
	std::shared_ptr<bool> isTargetDeath = false;//�^�[�Q�b�g�̃C���X�^���X�����݂��邩?
	float m_thrust = 0.0f;//����
	float m_nonAccelRad = 0.0f;//�������Ȃ��p�x�͈�
	float m_timerf = 0.0f;//�z�[�~���O�J�n�܂ł̃t���[����
};


/// <summary>
/// �����R���|�[�l���g
/// </summary>
class BD_Brake : public IBulletComponent {
public:
	BD_Brake(float brakePow):m_brakePawer(max(0.0f,brakePow)){}

	void Update()override {
		m_bullet->m_vector = m_bullet->m_vector.GetNorm()*max(0.0f, m_bullet->m_vector.Length() - m_brakePawer);
	}

private:
	float m_brakePawer = 0.0f;//1�t���[���Ɍ������鑬�x
};

/// <summary>
/// �Փ˂ŏ��ŃR���|�[�l���g
/// </summary>
class BD_Contact : public IBulletComponent {
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="isContactField">�n�ʂƏՓ˂��邩?</param>
	BD_Contact(bool isContactField = true) : m_isContactField (isContactField){}

	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//�N���X���o��
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical] && (m_isContactField || !H->attributes[enGraund])) {
				//���������Ȃ玀
				m_bullet->m_lifeTime = 0.0f;
				m_bullet->SetPos(p.m_collisionPoint);
			}
		}
		if (!p.m_isCCollisionObj) {
			//���肪CCollisionObj����Ȃ��Ă���
			m_bullet->m_lifeTime = 0.0f;
			m_bullet->SetPos(p.m_collisionPoint);
		}
	}
private:
	bool m_isContactField = true;
};
/// <summary>
/// ���e�R���|�[�l���g
/// </summary>
class BD_Reflect : public IBulletComponent {
public:
	void PostUpdate()override {
		//���C�Ŕ���
		btVector3 rayStart = m_bullet->GetOldPos();
		btVector3 rayEnd = m_bullet->GetPos();
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			CVector3 Nvec = m_bullet->m_vector.GetNorm();
			//���˃x�N�g�����������_����
			CVector3 randamAxis = { CMath::RandomZeroToOne() - 0.5f,CMath::RandomZeroToOne() - 0.5f ,CMath::RandomZeroToOne() - 0.5f };
			randamAxis.Normalize();
			CQuaternion randamRot(randamAxis, CMath::RandomZeroToOne()*0.5f);
			randamRot.Multiply(Nvec);
			//���˃x�N�g��
			m_bullet->m_vector = CMath::CalcReflectVector(Nvec, gnd_ray.m_hitNormalWorld) * m_bullet->m_vector.Length();
			m_bullet->SetPos(gnd_ray.m_hitPointWorld + gnd_ray.m_hitNormalWorld * (m_bullet->m_radius+1.0f));
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
				m_bullet->SetPos(p.m_collisionPoint);
				//TODO ��������
			}
		}
		if (!p.m_isCCollisionObj) {
			//���肪CCollisionObj����Ȃ��Ă���
			m_bullet->m_lifeTime = 0.0f;
			m_bullet->SetPos(p.m_collisionPoint);
			//TODO ��������
		}
	}
};