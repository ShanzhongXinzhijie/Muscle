#include "stdafx.h"
#include "BP_HumanMantle.h"

using namespace GameObj;

namespace {
	constexpr int INVINCIBLE_FRAME = 15;//�X�e�b�v���G����
	constexpr int COOLDOWN_FRAME = 30;//�X�e�b�v�N�[���_�E��
	constexpr int DONTMOVE_FRAME = 10;//�ړ��s����
}

void BP_HumanMantle::InnerStart() {
	m_name = L"�}���g";

	//�������x�ύX
	m_ptrCore->MulGravity(0.4f);

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

	//�|�C���^�������o�ɃR�s�[
	m_cloth = cloth;
	
	//�\�����f���ʒu����
	m_localPos.y = -150.0f*0.25f;
	m_localPos.z = -65.0f;
	m_localScale *= 15.0f;

	//���f�����[�h
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/manto_test.cmo", enFbxUpAxisY, enFbxRightHanded, false);
	//�e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	TextureFactory::GetInstance().Load(L"Resource/texture/mant.png", nullptr, &textureView, nullptr, true);
	//�}�e���A���ݒ�
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetAlbedoTexture(textureView.Get());
			mat->SetAlbedoScale({1.0f,0.01f,0.08f,0.9f});//������
			mat->SetIsUseTexZShader(true);//�V���h�E�}�b�v�`��Ƀe�N�X�`���g��
			mat->SetShininess(0.08f);
		}
	);
	//m_model->SetIsShadowCaster(false);//TODO �I�t�Z�b�g�ɂ���(model.fx��
	m_model->GetSkinModel().SetIsFrustumCulling(false);//������J�����O���I�t
	//�|�X�g�h���[�ݒ�
	//m_model->InitPostDraw(PostDrawModelRender::enAlpha);

	//���_����p�����쐬
	ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();
	m_model->GetSkinModel().FindMesh([&](const auto& mesh) {
			//���_���̍쐬
			{
				D3D11_MAPPED_SUBRESOURCE subresource;

				//���_�̃��[�h
				HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
				if (FAILED(hr)) { 
					return;
				}

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
			//�C���f�b�N�X�o�b�t�@���쐬			
			{
				D3D11_MAPPED_SUBRESOURCE subresource;

				//�C���f�b�N�X�o�b�t�@�����b�N
				HRESULT hr = deviceContext->Map(mesh->indexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
				if (FAILED(hr)) { 
					return;
				}

				D3D11_BUFFER_DESC bufferDesc;
				mesh->indexBuffer->GetDesc(&bufferDesc);
				std::unique_ptr<IndexBuffer> indexBuffer = std::make_unique<IndexBuffer>();
				int stride = 2;
				int indexCount = bufferDesc.ByteWidth / stride;
				unsigned short* pIndex = reinterpret_cast<unsigned short*>(subresource.pData);
				for (int i = 0; i < indexCount; i++) {
					indexBuffer->push_back(pIndex[i]);
				}
				
				//�C���f�b�N�X�o�b�t�@���A�����b�N
				deviceContext->Unmap(mesh->indexBuffer.Get(), 0);
				
				//�ۑ�
				m_indexBufferArray.push_back(std::move(indexBuffer));
			}
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

	//�ړ�
	CVector3 move = m_moveDir*7.0f;
	if (m_invincibleFrame > 0) {//�X�e�b�v
		move *= 6.0f;
		m_yawInertia = 0.f;//���񖳌�
	}
	if (m_cooldownFrame > COOLDOWN_FRAME - DONTMOVE_FRAME) {
		move *= 0.0f;
	}
	//move.y = move.Length()*1.125f*CalcAirScale(m_ptrCore->GetHeightMeter());
	//m_ptrCore->GetRot().Multiply(move);
	m_ptrCore->AddVelocity(move);

	//����
	if (abs(m_yawInertia) > FLT_EPSILON) {
		m_ptrCore->AddAngularVelocity(CVector3::AxisY(), m_yawInertia);//����
		m_ptrCore->AddVelocity(CVector3::AxisY()*abs(m_yawInertia)*500.0f*CalcAirScale(m_ptrCore->GetHeightMeter()));//�㏸
		m_yawInertia *= 0.57f;//����// (abs(m_yawInertia) - CMath::DegToRad(0.1f)) / abs(m_yawInertia);
	}

	//�N�[���_�E��
	if (m_cooldownFrame > 0) {
		m_cooldownFrame--;
	}
	//���G����
	if (m_invincibleFrame > 0) {
		m_ptrCore->Invincible();//���G��
		m_invincibleFrame--;
		if (m_invincibleFrame <= 0) {
			//�N�[���_�E��
			m_cooldownFrame = COOLDOWN_FRAME;
		}
	}
}

void BP_HumanMantle::PostUTRSUpdate() {
	//�}���g�𓮂���
	//�ړ���
	CVector3 force = m_ptrCore->GetMove()*-1000.0f;
	//�����ϊ�(�}���g�͌����ς��Ȃ�����)
	GetFinalRot().InverseMultiply(force);
	//�ړ��ʂ��\�t�g�{�f�B�ɓK�p
	m_cloth->addForce(force);

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
		//���_�ʒu�X�V
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

		//�@���X�V
		i = 0;
		for (auto& Iarray : m_indexBufferArray) {
			int i2 = 0;
			int indexs[3];
			for (auto& index : *Iarray) {
				indexs[i2] = index;
				i2++;
				if (i2 == 3) {
					//�@���v�Z
					CVector3 A = (*m_vertexPosArrays[i])[indexs[0]].position;
					CVector3 B = (*m_vertexPosArrays[i])[indexs[1]].position;
					CVector3 C = (*m_vertexPosArrays[i])[indexs[2]].position;
					CVector3 AB(B - A);
					CVector3 BC(C - B);
					AB.Cross(BC);
					AB.Normalize();
					AB *= -1.0f;//���]
					(*m_vertexPosArrays[i])[indexs[0]].normal = DirectX::XMFLOAT3(AB.x, AB.y, AB.z);
					(*m_vertexPosArrays[i])[indexs[1]].normal = DirectX::XMFLOAT3(AB.x, AB.y, AB.z);
					(*m_vertexPosArrays[i])[indexs[2]].normal = DirectX::XMFLOAT3(AB.x, AB.y, AB.z);

					//tangent�v�Z
					if (abs(AB.y) > 0.5f) {
						AB.Cross(CVector3::AxisZ());
					}
					else {
						AB.Cross(CVector3::AxisY());
					}
					(*m_vertexPosArrays[i])[indexs[0]].tangent = DirectX::XMFLOAT4(AB.x, AB.y, AB.z, 1.0f);
					(*m_vertexPosArrays[i])[indexs[1]].tangent = DirectX::XMFLOAT4(AB.x, AB.y, AB.z, 1.0f);
					(*m_vertexPosArrays[i])[indexs[2]].tangent = DirectX::XMFLOAT4(AB.x, AB.y, AB.z, 1.0f);

					i2 = 0;
				}
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

void BP_HumanMantle::Draw2D() {
	if (!m_ptrCore->GetIsDrawHUD()) { return; }

	//m_ptrCore->GetFont()->DrawFormat(L"Deg: %.1f", { 0.0f,0.85f },0.0f,CMath::RadToDeg(m_ptrCore->GetPad()->GetStickRollAngle(L)));

	//�X�e�b�v�\��
	if (m_invincibleFrame > 0) {
		m_ptrCore->GetFont()->Draw(L"[INVINCIBLE]", 0.5f, 0.5f);// { 0.3f, 0.90f }, { 0.0f,0.0f });
	}
	if (m_cooldownFrame > 0) {
		m_ptrCore->GetWarningFont()->Draw(L"[COOLDOWN]", 0.5f, 0.5f);//, { 0.3f,0.90f }, { 0.0f,0.0f });
	}

	//���E���x�x��
	if (m_ptrCore->GetHeightMeter() > LIMIT_HEIGHT_METER) {
		m_ptrCore->GetWarningFont()->Draw(L"[WARNING-HEIGHT]", { 0.7f,0.90f }, { 1.0f,0.0f });
	}
}

void BP_HumanMantle::Move(const CVector2& dir) {
	//if (CMath::Square(m_moveDir.Length()-0.1f) < dir.LengthSq()) {
		m_moveDir = CVector3(dir.x, 0.0f, dir.y); 
	//}
}
void BP_HumanMantle::Step(const CVector3& dir) {
	if (m_invincibleFrame > 0 || m_cooldownFrame > 0) {
		return;
	}

	//�X�e�b�v�ړ�
	m_moveDir = dir;

	//���G����
	m_invincibleFrame = INVINCIBLE_FRAME;
}
void BP_HumanMantle::Stop() {
	m_moveDir = CVector3::Zero();
}
void BP_HumanMantle::Yaw(float angle) {
	if (abs(m_yawInertia) < abs(angle) || m_yawInertia > 0.0f && angle < 0.0f || m_yawInertia < 0.0f && angle > 0.0f) {
		m_yawInertia = angle;
	}
}

//�R���g���[���[
void HCon_HumanMantle::InnerUpdate() {
	if (m_ptrCore->GetPad()->GetStick(L).LengthSq() < CMath::Square(0.24f)) {
		m_isStickNeutral = true;
	}

	if (m_ptrCore->GetPad()->IsSmashInput(L)) {
		//�X�e�b�v
		CVector3 viewDir = m_ptrCore->GetVanisingPoint() - m_ptrCore->GetPos(); viewDir.y = 0.0f;
		if (viewDir.LengthSq() > FLT_EPSILON) {
			viewDir.Normalize();
			CVector3 moveDir;
			moveDir += viewDir * m_ptrCore->GetPad()->GetStick(L).y;
			moveDir += viewDir.GetCross(CVector3::AxisY())*-m_ptrCore->GetPad()->GetStick(L).x;
			moveDir.Normalize();
			m_ptrBody->Step(moveDir);
		}
	}
	else if(m_isStickNeutral && m_ptrCore->GetPad()->GetStick(L).LengthSq() > CMath::Square(0.24f)){
		//�ړ�
		//m_ptrBody->Move(m_ptrCore->GetPad()->GetStick(L).GetNorm());
		m_isStickNeutral = false;
	}

	//����
	m_ptrBody->Yaw(m_ptrCore->GetPad()->GetStickRollAngle(L)*0.1f);

	//�X�g�b�v
	if (m_ptrCore->GetPad()->GetWingInput()) {
		m_ptrBody->Stop();
	}
}

void AICon_HumanMantle::InnerUpdate() {}