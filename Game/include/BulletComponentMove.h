#pragma once
#include"BulletKani.h"

/// <summary>
/// �ǔ��R���|�[�l���g
/// </summary>
class BD_Tracking : public IBulletComponent {
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="target">�ǔ��Ώ�</param>
	BD_Tracking(LockableWrapper* target = nullptr)
		: m_target(target)
	{
		if (target) {
			//�^�[�Q�b�g�̏��Œʒm���󂯎���悤�ɂ���
			isTargetDeath = std::make_shared<bool>(false);
			target->GetFu()->SetDeleteFlag(isTargetDeath);
		}
	};

	void Update()override {
		//�i�s�������Z�o
		CVector3 targetDir(m_bullet->m_vector);
		//�ڕW���� �� �ڕW�̃C���X�^���X�����ł��Ă��Ȃ�
		if (m_target && !(*isTargetDeath.get())) {
			targetDir = m_target->GetFu()->GetCollisionPos() - m_bullet->GetPos() + m_target->GetMove();
		}
		targetDir.Normalize();

		//�p�x�����ȉ��Œǐ�
		if (CVector3::AngleOf2NormalizeVector(targetDir, m_bullet->m_vector.GetNorm()) < CMath::DegToRad(25.0f)) {
			//targetDir.Lerp(0.5f, m_bullet->m_vector.GetNorm(), targetDir);
			//targetDir.Normalize();
			m_bullet->m_vector = targetDir * m_bullet->m_vector.Length();
		}
	}

private:
	const LockableWrapper* m_target = nullptr;//�ڕW
	std::shared_ptr<bool> isTargetDeath;//�^�[�Q�b�g�̃C���X�^���X�����݂��邩?
};

/// <summary>
/// �z�[�~���O�R���|�[�l���g
/// </summary>
class BD_Homing : public IBulletComponent {
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="target">�ǐՖڕW</param>
	/// <param name="thrust">1f�̐��i��</param>
	/// <param name="noAccelRad">�ڕW�Ƃ̊p�x������ȉ����Ɖ������Ȃ�</param>
	/// <param name="limitRad">�ڕW��ǐՉ\�Ȋp�x</param>
	/// <param name="timer">�ǐՊJ�n�܂ł̎���</param>
	BD_Homing(LockableWrapper* target = nullptr, float thrust = 0.0f, float noAccelRad = 0.0f, float limitRad = CMath::PI2, float timer = 0.0f)
		: m_target(target), m_thrust(thrust), m_nonAccelRad(noAccelRad), m_limitRad(limitRad), m_timerf(timer)
	{
		if (target) {
			//�^�[�Q�b�g�̏��Œʒm���󂯎���悤�ɂ���
			isTargetDeath = std::make_shared<bool>(false);
			target->GetFu()->SetDeleteFlag(isTargetDeath);
		}

		m_arrowModel.SetEnable(false);
	};

	/// <summary>
	/// ������������ݒ�
	/// </summary>
	void SetInitDirection(const CVector3& dir) {
		m_isInitedDir = true;
		m_initDir = dir.GetNorm();
	}

	void Start()override {
		//�ǔ������̏�����
		if (!m_isInitedDir) {
			SetInitDirection(m_bullet->m_vector);
		}
		if (m_timerf > 0.0f) {
			//�����������\�����f��
			m_arrowModel.SetEnable(true);
			m_arrowModel.Init(L"Resource/modelData/arrow.cmo");
			//m_arrowModel.InitPostDraw(PostDrawModelRender::enBlendMode::enAlpha);
			m_arrowModel.GetSkinModel().FindMaterialSetting(
				[&](MaterialSetting* mat) {
					mat->SetIsMotionBlur(false);
					mat->SetLightingEnable(false);
				}
			);
			m_arrowModel.SetPos(m_bullet->GetPos());
			CQuaternion rot;
			rot.MakeLookToUseXYAxis(m_initDir*-1.0f);
			m_arrowModel.SetRot(rot);
			m_arrowModel.SetScale(2.0f);
		}
	}

	void Update()override {
		//�Ō�Ɉړ������������L�^
		if (m_bullet->m_vector.LengthSq() > FLT_EPSILON) {
			m_lastDir = m_bullet->m_vector.GetNorm();
		}

		//�^�C�}�[����
		if (m_timerf > 0.0f) {
			m_timerf = max(0.0f, m_timerf - 1.0f);
			if (m_timerf > 0.0f) {
				//�����������\��
				//���W
				m_arrowModel.SetPos(m_bullet->GetPos());
				//�_��
				m_isDrawTimer--;
				if (m_isDrawTimer <= 0) {
					m_isDrawTimer = !m_arrowModel.GetIsDraw() ? 10 : 4;
					m_arrowModel.SetIsDraw(!m_arrowModel.GetIsDraw());
				}
				return;
			}
			else {
				//�����������\�����
				m_arrowModel.SetEnable(false);
				//����������
				m_bullet->m_vector = m_initDir * max(1.0f, m_bullet->m_vector.Length());
			}
		}

		//�ڕW����
		CVector3 targetDir(m_bullet->m_vector);
		//�ڕW���� �� �ڕW�̃C���X�^���X�����ł��Ă��Ȃ�
		if (m_target && !(*isTargetDeath.get())) {
			targetDir = m_target->GetFu()->GetCollisionPos() - m_bullet->GetPos() + m_target->GetMove();
		}
		targetDir.Normalize();

		//��ɉ����ݒ� or �ڕW�����Ƃ̊p�x���J���Ă��� and �U���p�x��
		float rad = 0.0f;
		float dirLength = m_bullet->m_vector.LengthSq();
		if (dirLength > FLT_EPSILON) {
			rad = CVector3::AngleOf2NormalizeVector(targetDir, m_bullet->m_vector.GetNorm());
		}
		else {
			targetDir = m_lastDir;
		}
		if (dirLength < FLT_EPSILON || ((m_nonAccelRad < FLT_EPSILON || rad > m_nonAccelRad) && rad < m_limitRad)) {
			//�z�[�~���O����
			CVector3 beforeVec = m_bullet->m_vector;

			if (dirLength > FLT_EPSILON) {
				//�u���[�L���O
				float brakePow = CMath::Saturate(targetDir.Dot(m_bullet->m_vector.GetNorm()));
				m_bullet->m_vector *= brakePow;
				//m_bullet->m_vector -= m_bullet->m_vector.GetNorm()* m_thrust * 2.0f *(1.0f - brakePow);
			}

			//�ڕW�։���
			m_bullet->m_vector += targetDir * m_thrust;
			//��~�͂��Ȃ�
			if (m_bullet->m_vector.LengthSq() < FLT_EPSILON) {
				m_bullet->m_vector = beforeVec;
			}
		}
		else if (m_nonAccelRad < FLT_EPSILON) {
			//�����̉���
			m_bullet->m_vector += m_bullet->m_vector.GetNorm() * m_thrust;
		}
	}

	//���͂�ݒ�
	void SetThrust(float f) {
		m_thrust = f;
	}

private:
	const LockableWrapper* m_target = nullptr;//�ڕW
	std::shared_ptr<bool> isTargetDeath;//�^�[�Q�b�g�̃C���X�^���X�����݂��邩?
	float m_thrust = 0.0f;//����
	float m_nonAccelRad = 0.0f;//�������Ȃ��p�x�͈�
	float m_limitRad = CMath::PI2;//�ǔ��\�Ȋp�x
	float m_timerf = 0.0f;//�z�[�~���O�J�n�܂ł̃t���[����
	CVector3 m_initDir;//����������
	bool m_isInitedDir = false;//�������������ݒ肳��Ă��邩
	CVector3 m_lastDir = CVector3::Down();//�Ō�Ɉړ���������

	//�����������\��
	GameObj::CSkinModelRender m_arrowModel;
	int m_isDrawTimer = 0;
};


/// <summary>
/// �����R���|�[�l���g
/// </summary>
class BD_Brake : public IBulletComponent {
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="brakePow">1f�Ɍ������鑬�x</param>
	BD_Brake(float brakePow) :m_brakePawer(max(0.0f, brakePow)) {}

	void Update()override {
		//����
		m_bullet->m_vector = m_bullet->m_vector.GetNorm()*max(0.0f, m_bullet->m_vector.Length() - m_brakePawer);
	}

private:
	float m_brakePawer = 0.0f;//1�t���[���Ɍ������鑬�x
};

/// <summary>
/// �n�`�Փˉ���R���|�[�l���g
/// </summary>
class BD_ClashAvoidance : public IBulletComponent {
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="thrust">����</param>
	/// <param name="avoidanceFrame">���t���[����̏Փ˂܂ŉ�����邩</param>
	BD_ClashAvoidance(float thrust,float avoidanceFrame = 3.0f): m_thrust(thrust), m_avoidanceFrame(avoidanceFrame){}

	void PostUpdate()override {
		//���C�Ŕ���
		btVector3 rayStart = m_bullet->GetPos();
		btVector3 rayEnd = m_bullet->GetPos() + m_bullet->m_vector * m_avoidanceFrame;

		if ((rayStart - rayEnd).fuzzyZero()) {
			return;
		}

		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		//���̂܂܂ł͏Փ˂���!
		if (gnd_ray.hasHit()) {

			CVector3 Nvec = m_bullet->m_vector.GetNorm();
			//���˃x�N�g�����������_����
			/*CVector3 randamAxis = { CMath::RandomZeroToOne() - 0.5f,CMath::RandomZeroToOne() - 0.5f ,CMath::RandomZeroToOne() - 0.5f };
			randamAxis.Normalize();
			CQuaternion randamRot(randamAxis, CMath::RandomZeroToOne()*0.5f);
			randamRot.Multiply(Nvec);*/
			//���˃x�N�g��
			m_bullet->m_vector = CMath::CalcReflectVector(Nvec, gnd_ray.m_hitNormalWorld).GetNorm() * m_bullet->m_vector.Length();
			
			//�O���ς���
			//CVector3 targetDir = CVector3::Up();
			//m_bullet->m_vector = targetDir * m_bullet->m_vector.Length();
			/*
			//�u���[�L���O
			float brakePow = CMath::Saturate(targetDir.Dot(m_bullet->m_vector.GetNorm()));
			m_bullet->m_vector *= brakePow;
			//�ڕW�։���
			m_bullet->m_vector += targetDir * m_thrust;
			*/
		}
	}

private:
	float m_thrust = 0.0f;//����
	float m_avoidanceFrame = 10.0f;//���t���[����̏Փ˂܂ŉ�����邩
};