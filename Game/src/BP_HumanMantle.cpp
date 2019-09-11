#include "stdafx.h"
#include "BP_HumanMantle.h"

using namespace GameObj;

void BP_HumanMantle::InnerStart() {
	//�������[���h�ݒ�
	//TODO�@�ʂ̏��e�J
	GetPhysicsWorld().SetGravity({0.0f,-10.0f,0.0f});	
	//GetPhysicsWorld().GetSoftBodyWorldInfo()->air_density = 1.2f*10.0f;

	//�z���
	btScalar sl = 500.0f;//8710.938 ~ 169.777
	btScalar y = 0.0f;// 1200.0;
	int res = 9;
	btSoftBody* cloth = btSoftBodyHelpers::CreatePatch(*GetPhysicsWorld().GetSoftBodyWorldInfo(),
		btVector3(-sl, -sl, y),    // �l���̍��W 00
		btVector3(-sl, sl, y),    // �l���̍��W 10
		btVector3(sl, -sl, y),    // �l���̍��W 01
		btVector3(sl, sl, y),    // �l���̍��W 11
		res, res, // ������(2����)
		2 + 8, // �l���̌Œ�t���O(1,2,4,8)
		true); // �΂ߕ����̂΂˂�ON/OFF

	cloth->getCollisionShape()->setMargin(0.5);
	cloth->setTotalMass(150); // �S�̂̎���

	cloth->m_cfg.kMT = 0.8f;//���̌`���ۂƂ��Ƃ���͂𓭂�����
	//cloth->m_cfg.kDP = 0.2f;//��C��R
	
	btSoftBody::Material* pm = cloth->appendMaterial();//m_materials[0]
	pm->m_kLST = 0.4f;//�S��(0~1
	cloth->generateBendingConstraints(2, pm);

	//���_�ݒ�
	int nodenum = cloth->m_nodes.size();
	for (int i = 0; i < nodenum; ++i) {
		float distance = abs((-sl) - cloth->m_nodes.at(i).m_x.getY()) / (sl*2.0f);
		distance = 1.0f - distance;

		cloth->m_nodes.at(i).m_x.setX(cloth->m_nodes.at(i).m_x.getX() * max(0.1f,distance));

		////X
		//cloth->m_nodes.at(i).m_x.setX(CMath::Lerp(169.777f, 8710.938f, distance)*sign);
		////Y
		//cloth->m_nodes.at(i).m_x.setY(CMath::Lerp(3093.56f, -4717.89f, distance));
		///Z
		//cloth->m_nodes.at(i).m_x.setZ(CMath::Lerp(2204.786f, 7939.328f, distance)*sign);
		//�Œ�
		if (distance < FLT_EPSILON) { cloth->setMass(i, 0.f); }
	}

	//�\�t�g�{�f�B��o�^
	GetPhysicsWorld().GetDynamicWorld()->addSoftBody(cloth, btBroadphaseProxy::DefaultFilter, 0);

	//btTransform trans;
	//trans.setIdentity();		// �ʒu�p���s��̏�����
	//trans.setOrigin(btVector3(0, 2000, 0));		// ������
	//cloth->transform(trans);

	m_cloth = cloth;
	
	m_localPos.y = -150.0f;
	m_localPos.z = -60.0f;// -65.0f;
	m_localScale *= 15.0f;

	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/manto_test.cmo", enFbxUpAxisY);
	m_model->GetSkinModel().SetCullMode(D3D11_CULL_NONE);//�o�b�N�J�����O���Ȃ�
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetAlbedoScale({1.0f,0.01f,0.08f,1.0f});
			mat->SetIsUseTexZShader(true);//�V���h�E�}�b�v�`��Ƀe�N�X�`���g��
		}
	);
	m_model->SetIsShadowCaster(false);//TODO �I�t�Z�b�g�ɂ���(model.fx��
	//TODO �����䉻�����O
	//TODO �e���������Ȃ�?

	//���_����p�����쐬
	ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();
	m_model->GetSkinModel().FindMesh([&](const auto& mesh) {
			D3D11_MAPPED_SUBRESOURCE subresource;

			//���_�̃��[�h
			HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
			if (FAILED(hr)) { return; }

			D3D11_BUFFER_DESC bufferDesc;
			mesh->vertexBuffer->GetDesc(&bufferDesc);
			int vertexCount = bufferDesc.ByteWidth / mesh->vertexStride;//���_��
			char* pData = reinterpret_cast<char*>(subresource.pData);
			std::unique_ptr<VertexPosArray> vertexBuffer = std::make_unique<VertexPosArray>();
			std::unique_ptr<NodeArray> nodes = std::make_unique<NodeArray>();
			for (int i = 0; i < vertexCount; i++) {
				//���_���̕ۑ�
				vertexBuffer->emplace_back(*reinterpret_cast<DirectX::VertexPositionNormalTangentColorTexture*>(pData));

				float distance = abs((-sl) - vertexBuffer->back().position.y) / (sl*2.0f);
				distance = 1.0f - distance;
				vertexBuffer->back().position.x *=  max(0.1f, distance);

				nodes->emplace_back();
				float closestDistanse = -1.0f;
				for (int i = 0; i < nodenum; ++i) {
					CVector3 disvec = CVector3(vertexBuffer->back().position) - (CVector3)cloth->m_nodes.at(i).m_x;
					float distance = disvec.LengthSq();
					if (closestDistanse < 0.0f || closestDistanse > distance) {
						nodes->back().node = &cloth->m_nodes.at(i);
						nodes->back().offset = disvec;
						closestDistanse = distance;
					}
				}				
				pData += mesh->vertexStride;//���̒��_��
			}
			//�ۑ�
			m_vertexPosArrays.push_back(std::move(vertexBuffer));
			m_nodeArrays.push_back(std::move(nodes));

			//���_�o�b�t�@���A�����b�N
			deviceContext->Unmap(mesh->vertexBuffer.Get(), 0);
		}
	);
	
}

void BP_HumanMantle::Update() {
	if (GetKeyInput(VK_UP)) {
		m_cloth->setVelocity(btVector3(1000, 0, 1000));
	}
	if (GetKeyInput(VK_DOWN)) {
		m_cloth->addForce(btVector3(-1000, 0, 0));
	}
}

void BP_HumanMantle::PostLoopUpdate() {
	//���_�o�b�t�@�X�V
	{
		int i = 0;
		for (auto& Varray : m_vertexPosArrays) {
			int i2 = 0;
			for (auto& vertex : *Varray) {
				auto& nodearray = *m_nodeArrays[i];
				vertex.position.x = (nodearray[i2].node->m_x.x() + nodearray[i2].offset.x)*-1.0f;
				vertex.position.y = nodearray[i2].node->m_x.y() + nodearray[i2].offset.y;
				vertex.position.z = nodearray[i2].node->m_x.z() + nodearray[i2].offset.z;
				i2++;
			}
			i++;
		}
	}

	//���_�o�b�t�@�K�p
	int i = 0;
	ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();
	m_model->GetSkinModel().FindMesh([&](const auto& mesh) {
		//���_�o�b�t�@���쐬�B
		D3D11_MAPPED_SUBRESOURCE subresource;
		//���_�̏�������
		HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
		if (FAILED(hr)) { return; }
		memcpy(subresource.pData, &m_vertexPosArrays[i].get()->front(), sizeof(DirectX::VertexPositionNormalTangentColorTexture) * m_vertexPosArrays[i].get()->size()); //�R�s�[				
		deviceContext->Unmap(mesh->vertexBuffer.Get(), 0);
		i++;
		}
	);
}