#include "stdafx.h"
#include "CPlayer.h"

CPlayer::CPlayer(int playernum) :
	//�v���C���[�ԍ�
	m_playerNum(playernum),
	//����ݒ�
	m_assemble(FindGO<HotokeAssembleManager>(L"HotokeAssembleManager")->GetHotokeAssemble(m_playerNum)),
	//�t�H���g
	m_HUDFont(m_HUDColor, 0.5f), m_warningFont(CVector4::Red(), 0.5f), m_japaneseFont(m_HUDColor, 0.5f)
{
	//�p�b�h�E�J����
	if (!m_assemble.ai) {
		m_pad = std::make_unique<IGamePad>(playernum);
		m_cam = std::make_unique<HotokeCameraController>(&m_hotoke, m_pad.get());
		//���C���J�����ݒ�
		m_cam->SetToMainCamera(playernum);
	}

	//�{��
	if (!m_assemble.ai) {
		//�v���C���[
		m_hotoke.Init(playernum, m_pad.get(), true, &m_HUDFont, nullptr);
	}
	else {
		//CPU
		m_hotoke.Init(playernum, nullptr, false, nullptr, std::unique_ptr<IAI>(m_assemble.ai->Create(&m_hotoke)));
	}
	
	//��
	for (auto& grass : m_grass) {
		grass.SetDrawCameraNum(m_playerNum);
	}

	//�t�H���g
	m_japaneseFont.SetUseFont(HUDFont::enJPN);
	m_hotoke.SetFonts(&m_warningFont, &m_japaneseFont);
};

bool CPlayer::Start() {	
	//�@�̂̃p�[�c��ݒ�
	m_hotoke.SetBodyPart(CDeathHotoke::enHead, std::unique_ptr<IBodyPart>(m_assemble.parts[CDeathHotoke::enHead]->Create()));
	m_hotoke.SetBodyPart(CDeathHotoke::enArm, std::unique_ptr<IBodyPart>(m_assemble.parts[CDeathHotoke::enArm]->Create()));
	m_hotoke.SetBodyPart(CDeathHotoke::enWing, std::unique_ptr<IBodyPart>(m_assemble.parts[CDeathHotoke::enWing]->Create()));
	m_hotoke.SetBodyPart(CDeathHotoke::enLeg, std::unique_ptr<IBodyPart>(m_assemble.parts[CDeathHotoke::enLeg]->Create()));

	//CPU�͎��s���Ȃ�
	if (m_assemble.ai) {
		return true;
	}

	//�e�X�g���f��
	m_anim.Load(L"Resource/animation/human/stand.tka");
	m_human.Init(L"Resource/modelData/human.cmo", &m_anim, 1);
	m_human.SetScale(10.0f);

	m_animHeri.Load(L"Resource/animation/herico.tka", true);
	m_heri.Init(L"Resource/modelData/herico.cmo", &m_animHeri, 1);
	m_heri.SetScale(0.3f);

	//���C�Ŕ���
	btVector3 rayStart = btVector3(0.0f, 100000.0f, 0.0f);
	btVector3 rayEnd = btVector3(0.0f, -100000.0f, 0.0f);
	btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
	GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
	if (gnd_ray.hasHit()) {
		//�ڐG�_�����W��
		m_human.SetPos(gnd_ray.m_hitPointWorld);
		m_heri.SetPos(gnd_ray.m_hitPointWorld+CVector3::Back()*25.f);
	}

	//m_humanCam.SetViewAngleDeg(25.0f);
	m_humanCam.SetFar(150000.0f);

	//HUD
	m_guncross.Init(L"Resource/spriteData/gunCross.png");
	m_wMark.Init(L"Resource/spriteData/wMark.png");
	m_velocityVector.Init(L"Resource/spriteData/velocityVector.png");

	return true;
}

void CPlayer::Update() {
	//CPU�͎��s���Ȃ�
	if (m_assemble.ai) {
		return;
	}

	//�f�o�b�O�ړ�
	CVector3 pos = m_hotoke.GetPos();
	CVector3 moveDir;
	if (GetKeyInput('W')) {
		moveDir.z += 2.0f;
	}
	if (GetKeyInput('S')) {
		moveDir.z -= 2.0f;
	}
	if (GetKeyInput('A')) {
		moveDir.x -= 2.0f;
	}
	if (GetKeyInput('D')) {
		moveDir.x += 2.0f;
	}
	moveDir.y = 0.0f;
	moveDir.Normalize();
	if (GetKeyInput(VK_LSHIFT)) {
		pos += moveDir * 1.0f;
	}
	else {
		pos += moveDir * 10.0f;
	}
	if (GetKeyInput(VK_SPACE)) {
		pos.y += 40.0f;
	}
	if (GetKeyInput('C')) {
		pos.y -= 20.0f;
	}
	m_hotoke.SetPos(pos);

	//�J�����؂�ւ�
	if (GetKeyInput('F')) {
		//�q�g�ڐ�
		m_humanCam.SetPos(m_human.GetBonePos(m_human.FindBoneID(L"Head"))+CVector3::AxisY()*0);
		m_humanCam.SetTarget(m_hotoke.GetPos());
		SetMainCamera(&m_humanCam);
		m_human.SetIsDraw(false);		
	}
	else {
		//��
		m_cam->SetToMainCamera();
		m_human.SetIsDraw(true);
	}

	//�����J�������o
	m_cam->SetIsZoomout(m_hotoke.GetIsStun(), m_hotoke.GetZoomoutDirection());

	//�����_
	CVector3 vanisingPoint = m_hotoke.GetPos() + m_hotoke.GetTotalVelocity() + (m_cam->GetTargetPoint() - m_hotoke.GetPos()).GetNorm()*15000.0f*0.125f;
	m_hotoke.SetVanisingPoint(vanisingPoint);

	//���b�N�I��
	bool isLock = false; float minDistance = 0.0f; int lastLockLevel = -1;
	CVector3 outPos; LockableWrapper* outObj = nullptr;
	QueryGOs<LockableWrapper>(L"LockableObject", [&](LockableWrapper* go) {
		if (&m_hotoke == go->GetFu() || &m_hotoke == go->GetOwner()) { return true; }//�����͏���

		CVector3 screenPos = m_cam->CalcScreenPosFromWorldPos(go->GetFu()->GetCollisionPos());
		float distance = CVector3(screenPos.x - 0.5f, screenPos.y - 0.5f, 0.0f).LengthSq();
		//�ʒu����ʓ���?
		if (screenPos.x > 0.0f && screenPos.x < 1.0f && screenPos.y > 0.0f && screenPos.y < 1.0f && screenPos.z > 0.0f && screenPos.z < 1.0f) {
			if (!isLock || lastLockLevel < go->GetPriorityLevel() || minDistance > distance) {
				isLock = true;
				outPos = go->GetFu()->GetCollisionPos();
				outObj = go;
				screenPos.z = 0.0f; minDistance = distance;
			}
		}
		return true; 
	});
	//�z�g�P�̃^�[�Q�b�g�ʒu�ݒ�
	if (isLock) {
		m_hotoke.SetTargetPos(outPos);
		m_hotoke.SetTarget(outObj);
		m_targetHP = m_hotoke.GetTarget()->GetHP();
		m_targetPos = m_hotoke.GetTargetFu()->GetPos();
	}
	else {
		m_hotoke.SetTargetPos(vanisingPoint);
		m_hotoke.SetTarget(nullptr);
	}
	m_isLockon = isLock;
}

void CPlayer::PostUpdate() {
	if (m_cam) {
		m_hotoke.SetIsBackMirror(m_cam->GetIsBackMirror());//�z�g�P�Ƀo�b�N�~���[��Ԃ�ʒm
	}
}

namespace {
	const CVector3 outlineOffset = { 0.001f,0.001f,0.0f };
	constexpr float HPBarLength = 0.4f;
	const CVector3 HPBarMin = { 0.3f, 0.95f, 0.0f };
	const CVector3 HPBarMax = { 0.3f + HPBarLength, 0.95f + 0.005f,0.0f };
}

void CPlayer::PostLoopUpdate() {
	if (!m_isDrawHUD)return;
	if (GetKeyInput('F')) { return; }

	//HealthBar
	DrawQuad2D(HPBarMin - outlineOffset, HPBarMax + outlineOffset, { 1.0f - m_HUDColor.x, 1.0f - m_HUDColor.y, 1.0f - m_HUDColor.z, m_HUDColor.w*0.5f }, m_playerNum);
	DrawQuad2D(HPBarMin, HPBarMax - CVector3::AxisX()*HPBarLength*(1.0f-m_hotoke.GetHPPer()), m_HUDColor, m_playerNum);

	//�G�l�~�[�ւ̃��C��
	QueryGOs<LockableWrapper>(L"LockableObject", [&](LockableWrapper* go) {
		if (&m_hotoke == go->GetFu() || &m_hotoke == go->GetOwner()) { return true; }//�����͏���

		CVector3 screenPos = m_cam->CalcScreenPosFromWorldPos(go->GetFu()->GetCollisionPos());
		
		//�ʒu����ʓ���?
		if (screenPos.x > 0.0f && screenPos.x < 1.0f && screenPos.y > 0.0f && screenPos.y < 1.0f && screenPos.z > 0.0f && screenPos.z < 1.0f) {
			DrawLine2D({0.5f}, screenPos, m_HUDColor*CVector4(1.0f, 1.0f, 1.0f, 0.5f), m_playerNum);
		}
		return true;
	});
	
	//�O���b�h
	CVector3 origin = m_cam->GetPos(); origin += m_hotoke.GetFront()*380.0f;
	//DrawLine3D(origin - m_hotoke.GetLeft()*100.f, origin + m_hotoke.GetLeft()*100.f, m_HUDColor, m_playerNum);
	//DrawLine3D(origin - m_hotoke.GetUp()*100.f, origin + m_hotoke.GetUp()*100.f, m_HUDColor, m_playerNum);//�����
	
	origin = m_cam->CalcScreenPosFromWorldPos(origin);
	if (origin.z > 0.0f && origin.z < 1.0f) {
		DrawLine2D(origin + CVector3(-0.2f, 0.0f, 0.0f), origin + CVector3(0.2f, 0.0f, 0.0f), m_HUDColor*CVector4(1.0f, 1.0f, 1.0f, 0.5f), m_playerNum);
		DrawLine2D(origin + CVector3(0.0f, -0.2f, 0.0f), origin + CVector3(0.0f, 0.2f, 0.0f), m_HUDColor*CVector4(1.0f, 1.0f, 1.0f, 0.5f), m_playerNum);
	}
}

void CPlayer::HUDRender(int HUDNum) {
	if (!m_isDrawHUD || m_playerNum != HUDNum)return;
	if (GetKeyInput('F')) { return; }

	//Health
	m_HUDFont.DrawFormat(L"%.0f/%.0f", HPBarMax + outlineOffset, { 1.0f,0.0f }, m_hotoke.GetHP(), m_hotoke.GetHPMax());

	//���҂̌����Ă�����̈ʒu
	CVector3 tdFrontPos = m_cam->CalcScreenPosFromWorldPos(m_cam->GetPos() + m_hotoke.GetFront()*380.0f);
	
	//���x
	if (tdFrontPos.z > 0.0f && tdFrontPos.z < 1.0f) {
		float kmh;
		//����
		kmh = m_hotoke.GetMove().Length()*FRAME_RATE*60.0f*60.0f / METER / 1000.0f;
		m_HUDFont.DrawFormat(L"%.0f", tdFrontPos - CVector3(0.06f,  0.025f, 0.0f), { 1.0f,1.0f }, kmh);
		
		//�������x
		kmh = -m_hotoke.GetMove().y*FRAME_RATE*60.0f*60.0f / METER / 1000.0f;
		if (abs(kmh) < 0.1f) { 
			kmh = 0.0f; 
		}
		m_HUDFont.DrawFormat(L"%.0f", tdFrontPos - CVector3(0.06f, 0.0f, 0.0f), { 1.0f,0.0f }, kmh);

		//���x
		m_HUDFont.DrawFormat(L"%.0f", tdFrontPos - CVector3(-0.06f, 0.025f, 0.0f), { 0.0f,1.0f }, m_hotoke.GetHeightMeter());
	}
	
	CVector3 pos;

	//�E�C�X�L�[�}�[�N(�@�̂̌���)
	pos = tdFrontPos;
	if (pos.z > 0.0f && pos.z < 1.0f) {
		m_wMark.Draw(pos, 1.0f, 0.5f, 0.0f, m_HUDColor);
	}
	
	//�K���N���X(�Ə�)
	pos = m_cam->CalcScreenPosFromWorldPos(m_hotoke.GetTargetPos());
	if (pos.z > 0.0f && pos.z < 1.0f) { 
		/*if (m_guncrossPosOld.z > 0.0f && m_guncrossPosOld.z < 1.0f) {
			constexpr int loopmax = 8;
			for (int i = 0; i < loopmax; i++) {
				m_guncross.Draw(pos + (m_guncrossPosOld - pos) / loopmax * (i+1), 1.0f, 0.5f, m_isLockon ? 0.0f : CMath::PI_QUARTER, m_HUDColor*CVector4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}*/
		m_guncross.Draw(pos, 1.0f, 0.5f, m_isLockon ? 0.0f : CMath::PI_QUARTER, m_HUDColor);
		if (m_isLockon) {
			//�X�e�[�^�X
			//HP
			if (m_targetHP > 0.0f) {
				m_HUDFont.DrawFormat(L"%.0f", pos - CVector3(0.0f, 0.025f, 0.0f), { 0.5f,1.5f }, m_targetHP);
			}
			//����(m)
			CVector2 motoScale = m_HUDFont.GetScale();
			m_HUDFont.SetScale(motoScale*0.75f);
			m_HUDFont.DrawFormat(L"%.0f", pos - CVector3(0.0f, -0.025f, 0.0f), { 0.5f,0.0f }, (m_targetPos - m_hotoke.GetPos()).Length() / METER);
			m_HUDFont.SetScale(motoScale);
		}
	}
	//m_guncrossPosOld = pos;

	//�x���V�e�B�x�N�g��(�i�s����)
	if (m_hotoke.GetMove().LengthSq() > 1.0f) {
		pos = m_cam->CalcScreenPosFromWorldPos(m_hotoke.GetPos() + m_hotoke.GetMove().GetNorm() * (m_cam->GetFar()*0.5f));
		if (pos.z > 0.0f && pos.z < 1.0f) { m_velocityVector.Draw(pos, 0.75f, 0.5f, 0.0f, m_HUDColor); }
	}
	//m_velocityPosOld = pos;

	//�o�b�N�~���[
	if (m_cam->GetIsBackMirror()) {
		m_HUDFont.Draw(L"((BACK VIEW))", { 0.5f,0.0f }, { 0.5f,0.0f });
	}
}