#pragma once
#include "IBodyPart.h"
class BP_HumanMantle :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;

	void InnerStart()override;
	void Update()override;
	void PostLoopUpdate()override;

private:
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

