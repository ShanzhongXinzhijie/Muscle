#pragma once
#include"BeamModel.h"
#include"CSmoke.h"
#include"BulletKani.h"

/// <summary>
/// �r�[�����f���R���|�[�l���g
/// ����ʕ�������ύX�����ꍇ��蒼���K�v������
/// </summary>
class BD_BeamModel : public IBulletComponent {
public:
	BD_BeamModel(float radius = 3.0f, std::wstring_view beamName = L"BLUE") :m_radius(radius) {
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
	void PostLoopUpdate()override {
		//���f���̍X�V(��ʐ���)
		for (int i = 0; i < m_modelNum; i++) {
			CVector3 rootPos, tipPos;
			GameObj::ICamera* screenCamera = m_modelNum == 1 ? GetMainCamera() : ViewCameraList()[i];

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
		if (CMath::RandomZeroToOne() < 1.0f / 5.0f) {
			//�܂�ɂ��񂿂�
			m_model.Init(L"Resource/modelData/unchi.cmo");
			//�m�[�}���}�b�v�K�p
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
			TextureFactory::GetInstance().Load(L"Resource/normalMap/unchi_n.jpg", nullptr, &textureView);// , nullptr, true);
			m_model.GetSkinModel().FindMaterialSetting(
				[&](MaterialSetting* mat) {
					mat->SetNormalTexture(textureView.Get());
					mat->SetShininess(0.5f);
				}
			);
		}
		else {
			m_model.Init(L"Resource/modelData/leg.cmo");
		}
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
/// ���O�ՃR���|�[�l���g
/// </summary>
class BD_SmokeTrail : public IBulletComponent {
public:
	void PostUpdate()override {
		new CSmoke(m_bullet->GetPos(), CVector3::Zero(),
			{ 0.9f,0.88f,0.88f,0.65f },
			100.0f, 1.02f,//1.05f + 0.05f*(0.5f + CMath::RandomZeroToOne()),
			360
		);
	}

private:
};
