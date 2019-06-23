#include "stdafx.h"
#include "TreeGene.h"
#include "CDeathHotoke.h"

int Tree::m_sInstancingMax = 512;

Tree::Tree(int id, const CVector3& pos, const CQuaternion& rot) {
	m_id = id;
	m_pos = pos;
	m_rot = rot;

	//�ߌi���f��
	m_model.Init(m_sInstancingMax, L"Resource/modelData/tree_tall.cmo");
	m_model.SetPos(m_pos);
	//m_model.SetRot(m_rot);
	m_model.SetScale(0.8f);
	m_model.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			//me->SetAlbedoTexture(srv);
			me->SetIsUseTexZShader(true); //TODO �t�ɂ̂ݓK�p
		}
	);
	m_model.SetIsDraw(false);

	//���i���f��
	m_imposter.Init(L"Resource/modelData/tree_tall.cmo", { 2048*4, 2048*4 }, { 35,35 }, m_sInstancingMax);
	m_imposter.SetPos(m_pos);
	m_imposter.SetScale(0.8f);
	m_imposter.SetIsDraw(false);

	//�����蔻��
	const float radius = 5.0f;
	m_col.CreateSphere(m_pos+CVector3::AxisY()*radius, {}, radius);
	m_col.SetIsHurtCollision(true);
	m_col.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			//�Ȃ��|�����
			if (m_isHited) { return; }//���łɓ|��Ă�
			if (p.EqualName(L"DH_foot")) {
				CDeathHotoke* H = p.GetClass<CDeathHotoke>();
				CVector3 dir = m_pos - H->GetPos(); dir.y = 0.0f; dir.Normalize();
				if (dir.LengthSq() > FLT_EPSILON) {
					float rad = CVector3::AngleOf2NormalizeVector(dir, CVector3::AxisZ());
					float sign = 1.0f; if (CVector2(dir.x,dir.z).Cross({ 0.0f, 1.0f }) < 0.0f) { sign = -1.0f; }
					m_rotOffset = CQuaternion(CVector3::AxisY(), sign*rad) * CQuaternion(CVector3::AxisX(), CMath::DegToRad(80.0f));
					
					//���C�Ŕ���
					CVector3 topPos = m_pos+CVector3::AxisY()*(100.0f*m_model.GetScale().y);
					btVector3 rayStart = topPos;
					CVector3 topPosRotaed = topPos;
					m_rotOffset.Multiply(topPosRotaed);
					btVector3 rayEnd = topPosRotaed;
					btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
					GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
					if (gnd_ray.hasHit()) {
						CVector3 dir2 = gnd_ray.m_hitPointWorld - m_pos; dir2.Normalize();
						//�p�x
						float rad = CVector3::AngleOf2NormalizeVector(CVector3::AxisY(), dir2);
						if (rad > FLT_EPSILON) {
							//��]��
							CVector3 rotationAxis;
							rotationAxis.Cross(CVector3::AxisY(), dir2);
							rotationAxis.Normalize();
							m_rotOffset.SetRotation(rotationAxis, rad);
						}
						else {
							return;
						}
					}
					
					m_model.SetRot(m_rotOffset*m_rot);
					m_isHited = true;
				}
			}
		}
	);
	m_col.SetEnable(false);
	m_col.IGameObject::SetEnable(false);
}

void Tree::PostLoopUpdate() {
	const float nearDistance = CMath::Square(1200.0f), farDistance = CMath::Square(2500.0f);
	const float distance = (m_pos - GetMainCamera()->GetPos()).LengthSq();

	//if (distance > farDistance) {
	//	m_model.SetIsDraw(false);
	//	m_imposter.SetIsDraw(false);
	//	m_col.SetEnable(false);//�����Ɣ����������
	//	return;
	//}
	/*if (distance < nearDistance|| GetKeyInput(VK_TAB)) {//if(GetKeyInput(VK_TAB)) {//
		m_model.SetIsDraw(true);
		m_imposter.SetIsDraw(false);
		m_col.SetEnable(true);
	}
	else {
		m_model.SetIsDraw(false);
		if (!m_isHited) { m_imposter.SetIsDraw(true); }
		//m_col.SetEnable(false);//�����Ɣ����������
	}*/
	//m_model.SetIsDraw(false);
	

	bool isDraw = false;
	
	//TODO ������J�����O
	float d = GetMainCamera()->GetFront().Dot(m_pos - GetMainCamera()->GetPos());
	if (d > GetMainCamera()->GetNear() && d < GetMainCamera()->GetFar()) {//near��far�̊Ԃ�
		CVector2 frustum;
		GetMainCamera()->GetFrustumPlaneSize(d, frustum); frustum *= 0.5f;

		CVector3 left = GetMainCamera()->GetFront(); left.Cross(GetMainCamera()->GetUp());
		d = left.Dot(m_pos - GetMainCamera()->GetPos());
		if (d > -frustum.x && d < frustum.x) {

			CVector3 up; up.Cross(GetMainCamera()->GetFront(), left);

			d = up.Dot(m_pos - GetMainCamera()->GetPos());
			if (d > -frustum.y && d < frustum.y) {
				isDraw = true;
			}
		}
	}
	if (GetKeyInput(VK_TAB)) { isDraw = true; }

	if (isDraw) {
		if (distance < nearDistance) {
			m_model.SetIsDraw(true);
			m_imposter.SetIsDraw(false);
			//m_col.SetEnable(true);
		}
		else {
			m_model.SetIsDraw(false);
			if (!m_isHited) { m_imposter.SetIsDraw(true); }
			//m_col.SetEnable(false);//�����Ɣ����������
		}
	}
	else {
		m_model.SetIsDraw(false);
		m_imposter.SetIsDraw(false);
	}
}

void TreeGene::Generate(const CVector3& minArea, const CVector3& maxArea, int num) {

	//�C���X�^���V���O�̐��ݒ�
	if (Tree::m_sInstancingMax < num) {
		Tree::m_sInstancingMax = num;
	}

	//�؁X�����
	for (int i = 0; i < num; i++) {
		//���W����
		CVector3 pos = { minArea.x + (maxArea.x - minArea.x)*CMath::RandomZeroToOne(), maxArea.y, minArea.z + (maxArea.z - minArea.z)*CMath::RandomZeroToOne() };
		//��]����
		CQuaternion rot(CVector3::AxisY(), CMath::PI2*CMath::RandomZeroToOne());
		//rot *= CQuaternion(CVector3::AxisX(), CMath::DegToRad(0.5f) - CMath::DegToRad(1.0f)*CMath::RandomZeroToOne());
		//rot *= CQuaternion(CVector3::AxisZ(), CMath::DegToRad(0.5f) - CMath::DegToRad(1.0f)*CMath::RandomZeroToOne());
		//���C�Ŕ���
		btVector3 rayStart = pos;
		btVector3 rayEnd = btVector3(pos.x, minArea.y, pos.z);
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			//�ڐG�_�����W��
			pos = gnd_ray.m_hitPointWorld;

			/*CQuaternion lookTo;
			lookTo.MakeLookTo(gnd_ray.m_hitNormalWorld);
			rot = lookTo * rot;*/
		}		
		//���܂��!
		m_trees.emplace_back(std::make_unique<Tree>(i,pos,rot));
	}
}

void TreeGene::Clear() {
	m_trees.clear();
}
