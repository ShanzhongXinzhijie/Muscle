#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"

void CDeathHotoke::CalcDirection() {
	m_front = CVector3::Front(); GetRot().Multiply(m_front); m_back = m_front * -1.0f;
	m_left = CVector3::Left(); GetRot().Multiply(m_left); m_right = m_left * -1.0f;
	m_up = CVector3::Up(); GetRot().Multiply(m_up); m_down = m_up * -1.0f;
}

bool CDeathHotoke::Start() {
	//�X�P�[��
	constexpr float modelScale = 0.0188f*2.0f;
	m_scale = { modelScale };

	//�R�A����
	m_coreModel.Init(L"Resource/modelData/core.cmo");	
	//�m�[�}���}�b�v�K�p
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;	
	TextureFactory::GetInstance().Load(L"Resource/normalMap/ngasi.png", nullptr, &textureView, nullptr, true);
	m_coreModel.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.9f);
			mat->SetTranslucent(0.2f);
		}
	);

	//�����蔻��
	m_col.m_collision.CreateMesh({}, {}, m_scale, m_coreModel.GetSkinModel());//TODO ���ɂ��悤��
	m_col.m_collision.SetIsHurtCollision(true);//����͋�炢����
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (p.EqualName(L"ReferenceCollision")) {
				//�N���X���o��
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				//�_���[�W
				Damage(*H);
			}
		}
	);
	
	//�ʒu������
	m_pos = CVector3::AxisY()*1000.0f;
	m_pos.z += 200.0f;
	
	//�p�[�c����
	m_parts[enWing] = new BP_BirdWing(this);
	m_parts[enLeg] = new BP_HumanLeg(this);
	m_parts[enArm] = new BP_KaniArm(this);
	m_parts[enHead] = new BP_FishHead(this);
	//�p�[�c��Start
	for (auto& part : m_parts) {
		if (part)part->Start();
	}
	

	m_coreModel.GetSkinModel().FindMesh([&](const auto& mesh) {
		ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();
		{
			D3D11_MAPPED_SUBRESOURCE subresource;

			//���_�̃��[�h
			HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
			if (FAILED(hr)) { return; }			

			D3D11_BUFFER_DESC bufferDesc;
			mesh->vertexBuffer->GetDesc(&bufferDesc);
			int vertexCount = bufferDesc.ByteWidth / mesh->vertexStride;//���_��
			char* pData = reinterpret_cast<char*>(subresource.pData);
			VertexBufferPtr vertexBuffer = std::make_unique<VertexBuffer>();
			for (int i = 0; i < vertexCount; i++) {
				vertexBuffer->emplace_back(*reinterpret_cast<DirectX::VertexPositionNormalTangentColorTexture*>(pData));
				vertexBuffer->back().position.x += CMath::RandomZeroToOne()*1000.0f;
				vertexBuffer->back().position.y += CMath::RandomZeroToOne()*1000.0f;
				vertexBuffer->back().position.z += CMath::RandomZeroToOne()*1000.0f;
				//���̒��_�ցB
				pData += mesh->vertexStride;
			}

			//���_�o�b�t�@���A�����b�N
			deviceContext->Unmap(mesh->vertexBuffer.Get(), 0);
			m_vertexBufferArray.push_back(std::move(vertexBuffer));
		}
	}
	);

	return true;
}

void CDeathHotoke::Update() {
	//AI���s
	if (m_ai) { m_ai->Update(); }

	//�ړ��K��
	m_pos += m_move;
	m_rot = m_rotMove * m_rot;
	//����
	m_move *= 0.5f;
	m_rotMove.Slerp(0.5f, m_rotMove, CQuaternion::Identity());
	//�d��
	m_move.y -= 1.0f;

	//�p�[�c��Update
	for (auto& part : m_parts) {
		if (part)part->Update();	
	}	
	//�p�[�c��TRS�X�V
	for (auto& part : m_parts) {
		if (part)part->UpdateTRS();
	}
	//�R�A��TRS�X�V
	m_coreModel.SetPRS(m_pos, m_rot, m_scale);
	//�����x�N�g���X�V
	CalcDirection();
	//�R�A�̃R���W�����X�V
	m_col.SetPos(m_pos); m_col.SetRot(m_rot);
	//�p�[�c�̃��[���h�s��X�V��A�b�v�f�[�g
	for (auto& part : m_parts) {
		if (part)part->PostUTRSUpdate();
	}
}

void CDeathHotoke::PostRender() {
	//�p�[�c��2D�`��
	for (auto& part : m_parts) {
		if (part)part->Draw2D();
	}
	//�X�e�[�^�X�`��
	if (!m_isDrawHUD) {
		CFont font;
		wchar_t output[256];
		swprintf_s(output, L"%.1f", m_hp);
		font.Draw(output, { 0.0f,0.25f });
	}
}

void CDeathHotoke::Damage(const ReferenceCollision& ref) {
	m_hp -= ref.damege;
}

void CDeathHotoke::PostLoopUpdate() {

	DirectX::XMFLOAT3 offset(0,0,0);
	offset.x += CMath::RandomZeroToOne()*10.0f;
	offset.y += CMath::RandomZeroToOne()*100.0f;
	offset.z += CMath::RandomZeroToOne()*1000.0f;
	for (auto& v : m_vertexBufferArray) {
		for (auto& v2 : *v) {
			v2.position.x += offset.x;
		}
	}

	int i = 0;
	m_coreModel.GetSkinModel().FindMesh([&](const auto& mesh) {
			ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();

			//���_�o�b�t�@���쐬�B
			{
				D3D11_MAPPED_SUBRESOURCE subresource;

				//���_�̏�������
				HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
				if (FAILED(hr)) { return; }
				memcpy(subresource.pData, &m_vertexBufferArray[i].get()->front(), sizeof(DirectX::VertexPositionNormalTangentColorTexture) * m_vertexBufferArray[i].get()->size()); //�R�s�[				
				deviceContext->Unmap(mesh->vertexBuffer.Get(), 0);	

				i++;
			}		
		}
	);
}