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

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
	void PostLoopUpdate()override;

	//�A�N�V����
	void Move(const CVector2& dir);//�ړ�
	void Step(const CVector2& dir);//�X�e�b�v�ړ�
	void Jump();//�W�����v
	void Parachute();//�p���V���[�g(�������x�y��)

private:
	//�R���g���[���[
	IBodyController<BP_HumanMantle>* m_controller = nullptr;

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
};
/// <summary>
/// �}���g #AI�R���g���[���[
/// </summary>
class AICon_HumanMantle : public IBodyController<BP_HumanMantle> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};