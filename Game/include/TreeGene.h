#pragma once

#include"IFu.h"
#include"LOD.h"
#include"StageObjectGenerator.h"

/// <summary>
/// �l��!
/// </summary>
class CHuman : public IGameObject {
public:	
	CHuman() {
		SetName(L"CHuman");
	}

	/// <summary>
	/// �J�n������
	/// </summary>
	bool Start()override {
		m_anim.Load(L"Resource/animation/human/stand.tka");
		m_human.Init(L"Resource/modelData/human.cmo", &m_anim, 1);
		m_human.SetScale(10.0f);

		//���C�Ŕ���
		btVector3 rayStart = btVector3(0.0f, 100000.0f, 0.0f);
		btVector3 rayEnd = btVector3(0.0f, -100000.0f, 0.0f);
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			//�ڐG�_�����W��
			m_human.SetPos(gnd_ray.m_hitPointWorld);
		}

		//m_humanCam.SetViewAngleDeg(25.0f);
		m_humanCam.SetFar(150000.0f);

		return true;
	}

	/// <summary>
	/// ���̐l�Ԃ̎��_�����C���J�����ɐݒ�
	/// </summary>
	/// <param name="targetView">�����_</param>
	/// <param name="cameraNum">�Z�b�g��̃J�����ԍ�</param>
	void EnableHumanCamera(const CVector3& targetView, int cameraNum = 0) {
		m_humanCam.SetPos(m_human.GetBonePos(m_human.FindBoneID(L"Head")) + CVector3::AxisY() * 0);
		m_humanCam.SetTarget(targetView);
		SetMainCamera(&m_humanCam);
		SetCameraToList(cameraNum, &m_humanCam);
		m_human.SetIsDraw(false);
	}
	/// <summary>
	/// ���C���J��������l�Ԏ��_���O���Ƃ��ɌĂԏ���
	/// ���V���ȃJ�����̐ݒ�͕ʂł���Ă�������
	/// </summary>
	void DisableHumanCamera() {
		m_human.SetIsDraw(true);
	}

private:
	AnimationClip m_anim;
	GameObj::CSkinModelRender m_human;
	GameObj::PerspectiveCamera m_humanCam;
};

/// <summary>
/// �w���R�v�^�[
/// </summary>
class CHelicopter : public IStageObject, public IFu {
public:
	using IStageObject::IStageObject;

	//�������֐�
	void Init(const CVector3& pos, const CVector3& normal)override;

private:
	//�O���t�B�b�N
	AnimationClip m_animHeri;
	GameObj::CInstancingModelRender m_heri;
	
public:
	static inline int m_sInstancingMax = 32; //���̃N���X�̍ő�C���X�^���X��
};

/// <summary>
/// �S��
/// </summary>
class TransmissionTower : public IStageObject, public IGameObject, public IFu {
public:	
	using IStageObject::IStageObject;

	//�������֐�
	void Init(const CVector3& pos, const CVector3& normal)override;

	//���f�����擾
	GameObj::CSkinModelRender& GetModel() {
		return m_model;
	}

	//���C���[��ʂ���Ă��邩?
	void SetIsWired(bool b) { m_isWired = b; }
	bool IsWired()const { return m_isWired; }

private:
	//�O���t�B�b�N
	GameObj::CSkinModelRender m_model;
	std::unique_ptr<GameObj::CSkinModelRender> m_wire[6];
	//std::unique_ptr<GameObj::CSkinModelRender> m_dodai[4];

	bool m_isWired = false;//���C���[�ʂ���Ă邩?
};

/// <summary>
/// ��
/// </summary>
class Grass {
public:
	//�R���X�g���N�^
	Grass() :m_model(false) {}

	//�J�n������
	bool Start();
	//���샋�[�v�㏈��
	void PostLoopUpdate();
	//�`��O����
	void Pre3DRender(int);

	//�ǂ̃J�����ɕ\�����邩�ݒ�
	void SetDrawCameraNum(int camnum) {
		m_cameraNum = camnum;
	}

	//�\���ݒ�
	void DisableDraw() {
		m_model.SetIsDraw(false);
	}

private:
	//�ʒu���Đݒ�
	void RePos(GameObj::ICamera* mainCamera);

private:
	//�O���t�B�b�N
	GameObj::CInstancingModelRender m_model;//�o�^���Ȃ�

	int m_cameraNum = 0;//�ǂ̃J�����ɕ\�����邩

public:
	static inline constexpr int m_sInstancingMax = 512 * 4 * PLAYER_NUM; //���̃N���X�̍ő�C���X�^���X��
};

/// <summary>
/// Grass�N���X�����s����N���X
/// </summary>
class GrassRunner : public IGameObject {
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	GrassRunner() {
		//�\������J�����ݒ�
		int i = 0, playerNum = 0;
		for (auto& grass : m_grass) {
			if (i > Grass::m_sInstancingMax / PLAYER_NUM * (playerNum+1)) {
				playerNum++;
			}
			grass.SetDrawCameraNum(playerNum);
			i++;
		}
	}

	/// <summary>
	/// ������
	/// </summary>
	void Init() {
		m_isEnable = true;
		int i = 0;
		for (auto& grass : m_grass) {
			if (i <= Grass::m_sInstancingMax / PLAYER_NUM * ViewCameraList().size()) {
				grass.Start();
			}
			else {
				m_enableGrassNum = i; 
				break;
			}
			i++;
		}
	}
	/// <summary>
	/// ������
	/// </summary>
	void Disable() {
		m_isEnable = false;
		for (auto& grass : m_grass) {
			grass.DisableDraw();
		}
		m_enableGrassNum = 0;
	}
	
	/// <summary>
	/// ���샋�[�v�㏈��
	/// </summary>
	void PostLoopUpdate()override {
		if (!m_isEnable) { 
			return;
		}
		int i = 0;
		for (auto& grass : m_grass) {
			if (i >= m_enableGrassNum) {
				break;
			}
			grass.PostLoopUpdate();
			i++;
		}
	}
	
	/// <summary>
	/// �`��O����
	/// </summary>
	void Pre3DRender(int screenNum)override {
		if (!m_isEnable) {
			return;
		}
		int i = 0;
		for (auto& grass : m_grass) {
			if (i >= m_enableGrassNum) {
				break;
			}
			grass.Pre3DRender(screenNum);
			i++;
		}
	}

private:
	bool m_isEnable = false;
	//��
	int m_enableGrassNum = 0;
	Grass m_grass[Grass::m_sInstancingMax];
};

/// <summary>
/// �؂Ł[��
/// </summary>
class Tree : public IStageObject{
public:
	Tree(): m_lodSwitcher(false), m_model(false), m_imposter(false){}

	//�������֐�
	void Init(const CVector3& pos, const CVector3& normal)override;

	/// <summary>
	/// ������
	/// </summary>
	void Disable() {
		m_model.SetIsDraw(false);
		m_imposter.SetIsDraw(false);
	}

	//Update
	void PreLoopUpdate() {
		m_lodSwitcher.PreLoopUpdate();
	}
	void PostLoopPostUpdate() {
		m_model.Get().PostLoopPostUpdate();
		m_imposter.Get().GetInstancingModel().PostLoopPostUpdate();
	}
	void Pre3DRender(int num) {
		m_lodSwitcher.Pre3DRender(num);
	}

private:
	//�O���t�B�b�N
	LODSwitcher m_lodSwitcher;
	LODInstancingModel m_model;
	LODImposter m_imposter;
	LODNothing m_noDraw;

	//���W�Ƃ�
	CVector3 m_pos;
	CQuaternion m_rot, m_rotOffset;

	//�����蔻��
	//DHCollision m_col;
	//bool m_isHited = false;
	//float m_modelHeight = 100.0f, m_modelRadius = 0.0f;

public:
	static inline constexpr int m_sInstancingMax = 4000 + 1000;//���̃N���X�̍ő�C���X�^���X��
};


/// <summary>
/// Tree�N���X�����s����N���X
/// </summary>
class TreeRunner : public IGameObject {
public:
	/// <summary>
	/// ������
	/// </summary>
	void Init(StageObjectGenerator& objGene);
	/// <summary>
	/// ������
	/// </summary>
	void Disable() {
		m_isEnable = false;
		for (auto& tree : m_tree) {
			tree.Disable();
		}
		m_enableTreeNum = 0;
	}

	/// <summary>
	/// ���샋�[�v�O����
	/// </summary>
	void PreLoopUpdate()override {
		if (!m_isEnable) {
			return;
		}
		int i = 0;
		for (auto& tree : m_tree) {
			if (i >= m_enableTreeNum) {
				break;
			}
			tree.PreLoopUpdate();
			i++;
		}
	}

	/// <summary>
	/// ���샋�[�v�㏈��
	/// </summary>
	void PostLoopPostUpdate()override {
		if (!m_isEnable) {
			return;
		}
		int i = 0;
		for (auto& tree : m_tree) {
			if (i >= m_enableTreeNum) {
				break;
			}
			tree.PostLoopPostUpdate();
			i++;
		}
	}

	/// <summary>
	/// �`��O����
	/// </summary>
	void Pre3DRender(int screenNum)override {
		if (!m_isEnable) {
			return;
		}
		int i = 0;
		for (auto& tree : m_tree) {
			if (i >= m_enableTreeNum) {
				break;
			}
			tree.Pre3DRender(screenNum);
			i++;
		}
	}

private:
	bool m_isEnable = false;
	//��
	int m_enableTreeNum = 0;
	Tree m_tree[Tree::m_sInstancingMax];
};

class dammy : public IStageObject {
public:
	using IStageObject::IStageObject;
	void Init(const CVector3& pos, const CVector3& normal)override {
		//LOD������
		CVector2 FrustumSize; GetMainCamera()->GetFrustumPlaneSize(2400.0f / 3.0f, FrustumSize);//TODO �؂�Scale�ɘA��
		m_lodSwitcher.AddDrawObject(&m_noDraw[2], FrustumSize.y);
		m_lodSwitcher.AddDrawObject(&m_noDraw[1], FrustumSize.y * 3.0f);
		m_lodSwitcher.AddDrawObject(&m_noDraw[0]);

		//m_model.Get().Init(Tree::m_sInstancingMax, L"Resource/modelData/realTree_S.cmo");
		//m_model.Get().SetPos(pos);
		//m_model.Get().SetRot({});
		//m_model.Get().SetScale(1.0f);
		//m_model.Get().SetIsDraw(false);
		//m_model.Get().GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);

		//���i���f��
		//if (!m_imposter.Get().Init( L"Resource/modelData/realTree_S.cmo", Tree::m_sInstancingMax)) {
		//	//���񃍁[�h
		//	SkinModel model;
		//	model.Init(L"Resource/modelData/realTree_S.cmo");
		//	m_imposter.Get().Init(L"Resource/modelData/realTree_S.cmo", model, { 2, 2 }, { 1,1 }, Tree::m_sInstancingMax);
		//}
		/*m_imposter.Get().SetPos(pos);
		m_imposter.Get().SetScale(1.0f);
		m_imposter.Get().SetIsDraw(false);
		m_imposter.Get().SetIsShadowCaster(false);*/
	}

	LODSwitcher m_lodSwitcher;
	//LODInstancingModel m_model; LODImposter m_imposter; 
	LODNothing m_noDraw[3];
};

static inline GrassRunner g_grassRunner;
static inline TreeRunner g_treeRunner;