#pragma once
#include "BodyPartsHeader.h"

/// <summary>
/// �}���g
/// </summary>
class BP_HumanMantle :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()const override {
		return new BP_HumanMantle;
	}

	~BP_HumanMantle() {
		if (m_cloth) {
			btCollisionObject* obj = m_cloth;

			// �I�u�W�F�N�g��Rigid Body�̏ꍇ�̔j��
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState()) {
				delete body->getMotionState();
			}

			// �I�u�W�F�N�g��Soft Body�̏ꍇ�̔j��
			GetPhysicsWorld().GetDynamicWorld()->removeSoftBody(m_cloth);

			GetPhysicsWorld().GetDynamicWorld()->removeCollisionObject(m_cloth);

			delete m_cloth;
		}
	}

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
	void PostLoopUpdate()override;
	void Draw2D()override;

	//�A�N�V����
	void Move(const CVector2& dir);//�ړ�
	void Stop();//��~
	void Step(const CVector2& dir);//�X�e�b�v�ړ�
	void Yaw(float lerp);//����

private:
	//�R���g���[���[
	IBodyController<BP_HumanMantle>* m_controller = nullptr;

	CVector3 m_moveDir;//�ړ�����
	float m_yawInertia = 0.f;//��]����
	int m_invincibleFrame = 0;//���G����
	int m_cooldownFrame = 0;//�N�[���_�E��

	//�\�t�g�{�f�B
	btSoftBody* m_cloth = nullptr;
	
	//���_�X�V�n
	typedef std::vector<DirectX::VertexPositionNormalTangentColorTexture> VertexPosArray;	//���_���̔z��
	std::vector<std::unique_ptr<VertexPosArray>> m_vertexPosArrays;							//���_���̔z��̔z��

	typedef std::vector<unsigned int> IndexBuffer;					//�C���f�b�N�X�o�b�t�@
	std::vector<std::unique_ptr<IndexBuffer>> m_indexBufferArray;	//�C���f�b�N�X�o�b�t�@�̔z��

	struct NodeInfo	//�m�[�h���
	{
		btSoftBody::Node* node;
		CVector3 offset;
	};
	typedef std::vector<NodeInfo> NodeArray;				//�m�[�h���̔z��
	std::vector<std::unique_ptr<NodeArray>> m_nodeArrays;	//�m�[�h���̔z��̔z��
};

/// <summary>
/// �}���g #�l�R���g���[���[
/// </summary>
class HCon_HumanMantle : public IBodyController<BP_HumanMantle> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
private:
	bool m_isStickNeutral = true;
};
/// <summary>
/// �}���g #AI�R���g���[���[
/// </summary>
class AICon_HumanMantle : public IBodyController<BP_HumanMantle> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};