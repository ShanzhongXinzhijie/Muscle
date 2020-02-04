#pragma once

#include"ShadowMapBaker.h"

/// <summary>
/// �X�e�[�W�I�u�W�F�N�g
/// </summary>
class IStageObject {
public:
	//�R���X�g���N�^
	IStageObject(int id) : m_localID(id), m_worldID(m_s_nextID){
		m_s_nextID++;
	}
	//�f�X�g���N�^
	virtual ~IStageObject() {};

	//�������֐�
	virtual void Init(const CVector3& pos, const CVector3& normal) = 0;	

	//�V���h�E�}�b�v�x�C�J�[�ɒǉ�����
	virtual void AddToShadowMapBaker(ShadowMapBaker& targetBaker) {};

private:
	int m_localID = -1, m_worldID = -1;
	static int m_s_nextID;
};

/// <summary>
/// �X�e�[�W�I�u�W�F�N�g�𐶐�����N���X�D
/// </summary>
class StageObjectGenerator
{
public:
	//�R���X�g���N�^
	StageObjectGenerator() = default;

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="minArea">�����͈�(�ŏ��l)</param>
	/// <param name="maxArea">�����͈�(�ő�l)</param>
	/// <param name="maxnum">��������I�u�W�F�N�g�̍ő吔</param>
	template <typename T>
	void RectangularGenerate(const CVector3& minArea, const CVector3& maxArea, int maxnum, float radius = 80.0f) {
		int start = (int)m_genPoints.size();
		//�����_���
		CMath::GenerateBlueNoise(maxnum, { minArea.x,minArea.z }, { maxArea.x,maxArea.z }, radius, m_genPoints);
		//�X�e�[�W�I�u�W�F�N�g�����
		for (int i = start; i < m_genPoints.size(); i++) {
			//���W����
			CVector3 pos = { m_genPoints[i].x, maxArea.y, m_genPoints[i].y };//{ minArea.x + (maxArea.x - minArea.x)*CMath::RandomZeroToOne(), maxArea.y, minArea.z + (maxArea.z - minArea.z)*CMath::RandomZeroToOne() };
			//���C�Ŕ���
			btVector3 rayStart = pos;
			btVector3 rayEnd = btVector3(pos.x, minArea.y, pos.z);
			btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
			GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
			if (gnd_ray.hasHit()) {
				//�ڐG�_�����W��
				pos = gnd_ray.m_hitPointWorld;
			}
			//���܂��!
			m_objects.emplace_back(std::make_unique<T>(i));
			m_objects.back()->Init(pos, gnd_ray.m_hitNormalWorld);
		}
	}

	/// <summary>
	/// ����(�~)
	/// </summary>
	/// <param name="point">�������S</param>
	/// <param name="area">�����͈�</param>
	/// <param name="height">�����͈�(����)</param>
	/// <param name="maxnum">��������I�u�W�F�N�g�̍ő吔</param>
	template <typename T>
	void CircularGenerate(const CVector3& point, float area, float height, int maxnum, float radius = 80.0f) {
		int start = (int)m_genPoints.size();
		//�����_���
		CMath::GenerateBlueNoise(maxnum, { point.x - area, point.z - area }, { point.x + area, point.z + area }, radius, m_genPoints);
		//�X�e�[�W�I�u�W�F�N�g�����
		for (int i = start; i < m_genPoints.size(); i++) {
			//�����_�͉~����?
			if ((CVector2(point.x, point.z) - m_genPoints[i]).LengthSq() < CMath::Square(area)) {
				//���W����
				CVector3 pos = { m_genPoints[i].x, point.y + height, m_genPoints[i].y };
				//���C�Ŕ���
				btVector3 rayStart = pos;
				btVector3 rayEnd = btVector3(pos.x, point.y - height, pos.z);
				btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
				GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
				if (gnd_ray.hasHit()) {
					//�ڐG�_�����W��
					pos = gnd_ray.m_hitPointWorld;
				}
				//���܂��!
				m_objects.emplace_back(std::make_unique<T>(i));
				m_objects.back()->Init(pos, gnd_ray.m_hitNormalWorld);
			}
		}
	}

	/// <summary>
	/// ����(����)
	/// </summary>
	/// <param name="startPos">�����n�_</param>
	/// <param name="endPos">�����I�_</param>
	/// <param name="height">�����͈�(����)</param>
	/// <param name="maxnum">��������I�u�W�F�N�g�̍ő吔</param>
	template <typename T>
	void LinearGenerate(const CVector2& startPos, const CVector2& endPos, float height, int maxnum, float radius = 80.0f) {
		int start = (int)m_genPoints.size();
		
		//�����_���
		{
			std::unique_ptr<CVector2[]> points = std::make_unique<CVector2[]>(maxnum);
			std::unique_ptr<bool[]> isDead = std::make_unique<bool[]>(maxnum);

			for (int i = 0; i < maxnum; i++) {
				CVector2 pos;
				pos.Lerp((float)i / max(maxnum - 1, 1), startPos, endPos);
				points[i] = pos;
			}

			//�_�̈�苗�����ɑ��̓_�������������
			float radiusSq = CMath::Square(radius);
			for (int i = 0; i < maxnum; i++) {
				for (int i2 = i + 1; i2 < maxnum; i2++) {
					if ((points[i] - points[i2]).LengthSq() < radiusSq) {
						isDead[i] = true;//�_���S
						break;
					}
				}
				//�����œn���ꂽ�_�Ƃ�����
				if (!isDead[i]) {
					for (auto& point : m_genPoints) {
						if ((points[i] - point).LengthSq() < radiusSq) {
							isDead[i] = true;//�_���S
							break;
						}
					}
				}
			}

			//����
			for (int i = 0; i < maxnum; i++) {
				if (!isDead[i]) {//����łȂ��_
					m_genPoints.emplace_back(points[i]);
				}
			}
		}

		//�X�e�[�W�I�u�W�F�N�g�����
		for (int i = start; i < m_genPoints.size(); i++) {
			//���W����
			CVector3 pos = { m_genPoints[i].x, height, m_genPoints[i].y };
			//���C�Ŕ���
			btVector3 rayStart = pos;
			btVector3 rayEnd = btVector3(pos.x, -height, pos.z);
			btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
			GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
			if (gnd_ray.hasHit()) {
				//�ڐG�_�����W��
				pos = gnd_ray.m_hitPointWorld;
			}
			//���܂��!
			m_objects.emplace_back(std::make_unique<T>(i));
			m_objects.back()->Init(pos, gnd_ray.m_hitNormalWorld);
		}
	}

	//�łڂ�
	void Clear();

	//�X�e�[�W�I�u�W�F�N�g�擾
	IStageObject& GetModel(int index) { return *m_objects[index]; }

	//�����擾
	size_t GetNum()const { return m_objects.size(); }

private:
	std::vector<std::unique_ptr<IStageObject>> m_objects;//�X�e�[�W�I�u�W�F�N�g
	std::vector<CVector2> m_genPoints;
};

