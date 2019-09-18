#include "stdafx.h"
#include "BP_HumanMantle.h"

using namespace GameObj;

void BP_HumanMantle::InnerStart() {
	//�������[���h�ݒ�
	//TODO�@�ʂ̏��e�J
	GetPhysicsWorld().SetGravity({0.0f,-20.0f,0.0f});	
	//GetPhysicsWorld().GetSoftBodyWorldInfo()->air_density = 1.2f*10.0f;

	//�z���
	btScalar sl = 500.0f;//8710.938 ~ 169.777
	btScalar y = 0.0f;// 1200.0;
	constexpr int res = 9;//������
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

		//�`��
		cloth->m_nodes.at(i).m_x.setX(cloth->m_nodes.at(i).m_x.getX() * max(0.1f,distance));
		cloth->m_nodes.at(i).m_x.setY(cloth->m_nodes.at(i).m_x.getY() * 0.5f);

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
	
	//�\�����f���ʒu����
	m_localPos.y = -150.0f*0.25f;
	m_localPos.z = -65.0f;
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
	//m_model->SetIsShadowCaster(false);//TODO �I�t�Z�b�g�ɂ���(model.fx��
	//TODO �����䉻�����O
	//TODO �|�X�g�h���[�Ŕ������ɂ���?

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
				vertexBuffer->back().position.x *= max(0.1f, distance);
				vertexBuffer->back().position.y *= 0.5f;

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
	
	//�R���g���[���[
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_HumanMantle(this, m_ptrCore);
	}
	else {
		m_controller = new AICon_HumanMantle(this, m_ptrCore);
	}
}

void BP_HumanMantle::Update() {
	//�R���g���[���[�ɑ��삳����
	m_controller->Update();

	//target�Ɍ����Đ���
	CVector3 targetDir = m_ptrCore->GetTargetPos() - m_ptrCore->GetPos(); targetDir.y = 0.f; targetDir.Normalize();
	CVector3 frontDir = m_ptrCore->GetFront(); frontDir.y = 0.f; frontDir.Normalize();
	CVector3 leftDir = m_ptrCore->GetLeft(); leftDir.y = 0.f;
	float rad = CVector3::AngleOf2NormalizeVector(frontDir, targetDir);
	float sign = leftDir.Dot(targetDir);
	if (abs(sign) > 0.0f) { sign /= abs(sign); }
	if (abs(rad) < CMath::DegToRad(20.5f)) { rad = 0.0f; }
	//����
	m_ptrCore->AddRot(CQuaternion(CVector3::AxisY(), sign*-1.0f*min(rad,0.01f)));
}

void BP_HumanMantle::PostUTRSUpdate() {
	//�ړ��ʂ��\�t�g�{�f�B�ɓK�p
	m_cloth->addForce(m_ptrCore->GetMove()*-1000.0f);
	//TODO�@�����ϊ�(�}���g�͌����ς��Ȃ�����)
	//TODO  ��]
	
	//�\�t�g�{�f�B�ړ�����
	int nodenum = m_cloth->m_nodes.size();
	constexpr int res = 9;//������
	for (int i = 0; i < nodenum; ++i) {
		if (m_cloth->m_nodes.at(i).m_x.z() > -50.0f*(res - (i % res) - 1)) {
			m_cloth->m_nodes.at(i).m_x.setZ(-50.0f*(res - (i % res)) - 1);
			m_cloth->m_nodes.at(i).m_v = btVector3(0, 0, 0);// setVelocity(btVector3(0, 0, 0));
		}
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
				//TODO �Čv�Z
				//vertex.normal;
				//vertex.tangent;
				//vertex.textureCoordinate//uv���W?
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

void BP_HumanMantle::Move(const CVector2& dir) {
	//�ړ�
	CVector3 move = CVector3(dir.x, 0.0f, dir.y)*10.0f;
	m_ptrCore->GetRot().Multiply(move);
	m_ptrCore->AddMove(move);
}
void BP_HumanMantle::Step(const CVector2& dir) {}
void BP_HumanMantle::Jump() {
	//�W�����v
	m_ptrCore->AddMove(CVector3::AxisY()*1000.0f);
}
void BP_HumanMantle::Parachute() {}

void HCon_HumanMantle::Update() {

	//�ړ�
	m_ptrBody->Move(m_ptrCore->GetPad()->GetStick(L));

	//�W�����v
	if (m_ptrCore->GetPad()->GetLegDown()) {//TODO ����r�p�[�c����Ȃ�
		m_ptrBody->Jump();
	}
}

void AICon_HumanMantle::Update() {}