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

	btSoftBody* m_cloth = nullptr;
	
	typedef std::vector<DirectX::VertexPositionNormalTangentColorTexture> VertexPosArray;
	std::vector<std::unique_ptr<VertexPosArray>> m_vertexPosArrays;
	struct NodeInfo
	{
		btSoftBody::Node* node;
		CVector3 offset;
	};
	typedef std::vector<NodeInfo> NodeArray;
	std::vector<std::unique_ptr<NodeArray>> m_nodeArrays;
};

/// <summary>
/// �}���g #�l�R���g���[���[
/// </summary>
class HCon_HumanMantle : public IBodyController<BP_HumanMantle> {
public:
	using IBodyController::IBodyController;
	void Update()override;
};
/// <summary>
/// �}���g #AI�R���g���[���[
/// </summary>
class AICon_HumanMantle : public IBodyController<BP_HumanMantle> {
public:
	using IBodyController::IBodyController;
	void Update()override;
};