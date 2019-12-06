#pragma once
#include"IGamePad.h"
#include"AI.h"
#include"BodyPartsHeader.h"
#include"IFu.h"
#include"HUDFont.h"

class CDeathHotoke :
	public ILifeObject
{
public:
	//�R���X�g���N�^
	CDeathHotoke(int playernum, IGamePad* ptrPad, bool isDrawHUD, HUDFont* ptrFont, std::unique_ptr<IAI> AI)
		: m_playerNum(playernum),m_ptrPad(ptrPad),m_isDrawHUD(isDrawHUD), m_ptrHUDFont(ptrFont),m_ai(std::move(AI))
	{
		SetName(L"CDeathHotoke");
	}

	//�{�f�B�p�[�c�̎��
	enum enBodyParts {
		enWing, enLeg, enArm, enHead, enPartsNum,
	};
	//�p�[�c�ݒ�
	void SetBodyPart(enBodyParts partsType, std::unique_ptr<IBodyPart> part);

	//IGameObject�֌W
	bool Start()override;
	void PreUpdate()override;
	void Update()override;
	void PostLoopUpdate()override;
	void HUDRender(int)override;

	//�ړ��ʂ�������
	void AddVelocity(const CVector3& vec) { m_veloxity += vec; }//TODO �����ŏ㏸�ʉ�����?
	//��]�ʂ�������
	void AddAngularVelocity(const CVector3& axis, float radAngle) { 
		m_angularVelocity = CQuaternion(axis,radAngle*GetRotatability()) * m_angularVelocity;
	}

	//��R����Z
	void MulDrag(float mulDrag) { m_drag[enNext] *= mulDrag; }
	void MulAngularDrag(float mulDrag) { m_angularDrag[enNext] *= mulDrag; }

	//��]���₷������Z
	void MulRotatability(float rotatability) { m_rotatability[enNext] *= rotatability; }

	//�_���[�W����������
	void Damage(const ReferenceCollision& ref, const CVector3& pos);

	//�^�[�Q�b�g�ʒu��ݒ�
	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }
	//�^�[�Q�b�g�Ώېݒ�
	void SetTargetFu(IFu* target) { m_target = target; }

	//HUD��\�����邩�ݒ�
	void SetIsDrawHUD(bool enable) { m_isDrawHUD = enable; }
	//�o�b�N�~���[��Ԃ��ݒ�
	void SetIsBackMirror(bool isMirror) { m_isBackMirror = isMirror; }
	
	//�Q�b�^�[//

	//���x���擾(���[�g��)
	float GetHeightMeter()const {
		return GetPos().y / METER;
	}

	//HP���擾
	float GetHP()const {
		return m_hp;
	}
	float GetHPMax()const {
		return HP_MAX;
	}
	//HP���擾(����)
	float GetHPPer()const {
		return m_hp / HP_MAX;
	}

	//�X�^�����Ă��邩?
	bool GetIsStun()const {
		return m_stunTimeSec > 0.0f;
	}

	//�X�P�[�����擾
	[[nodiscard]] const CVector3& GetScale()const { return m_scale; }
	//�ړ������x�N�g�����擾
	[[nodiscard]] CVector3 GetMove() const { return GetPos() - m_posOld; }
	//�x���V�e�B���擾
	[[nodiscard]] const CVector3& GetVelocity() const { return m_veloxity; }
	//��R���擾
	[[nodiscard]] float GetDrag()const { return m_drag[enNow]; }
	//��]���₷�����擾
	[[nodiscard]] float GetRotatability()const { return m_rotatability[enNow]; }

	//�^�[�Q�b�g�ʒu���擾
	[[nodiscard]] const CVector3& GetTargetPos() const { return m_targetPos; }
	//�^�[�Q�b�g���擾
	[[nodiscard]] const IFu* GetTarget() const { return m_target; }
	[[nodiscard]] IFu* GetTarget() { return m_target; }

	//�p�b�h�̎擾
	[[nodiscard]] const IGamePad* GetPad()const { return m_ptrPad; }

	//�t�H���g�̎擾
	[[nodiscard]] const HUDFont* GetFont()const { return m_ptrHUDFont; }

	//HUD��\�����邩�擾
	[[nodiscard]] bool GetIsDrawHUD()const { return m_isDrawHUD; }
	//�o�b�N�~���[��Ԃ��ݒ�
	[[nodiscard]] bool GetIsBackMirror()const { return m_isBackMirror; }

	//AI�̐��ݏo���X�e�[�^�X���擾
	[[nodiscard]] const AIStatus* GetAIStatus()const { if (m_ai) { return &m_ai->GetOutputStatus(); } return nullptr; }


	//�������x���擾
	[[nodiscard]] constexpr float GetGravity()const { return GRAVITY; }

private:
	//�X�^������
	void Stun();

private:
	//�d�͒萔
	static constexpr float GRAVITY = OneG;

	//�R�A�̃��f��
	GameObj::CSkinModelRender m_coreModel;
	
	//�X�P�[��
	CVector3 m_scale;

	//�O�t���[���̈ʒu
	CVector3 m_posOld;
	
	//�ړ���
	CVector3 m_veloxity;
	CQuaternion m_angularVelocity;

	//�X�e�[�^�X
	static constexpr float HP_MAX = 100.0f;
	float m_hp = HP_MAX;//�w���X
	float m_stunTimeSec = 0.0f;//�X�^������
	enum{enNow,enNext};
	float m_drag[2] = { 1.0f,1.0f };//��R
	float m_angularDrag[2] = { 1.0f,1.0f };//��]��R
	float m_rotatability[2] = { 1.0f,1.0f };//��]���₷��

	//�p�[�c
	std::unique_ptr<IBodyPart> m_parts[4];

	//�v���C���[�ԍ�
	int m_playerNum = -1;
	//�Q�[���p�b�h
	IGamePad* m_ptrPad = nullptr;
	//AI
	std::unique_ptr<IAI> m_ai;
		
	bool m_isDrawHUD = false;//HUD��\�����邩
	HUDFont* m_ptrHUDFont = nullptr;//HUD�t�H���g
	bool m_isBackMirror = false;//�o�b�N�~���[���

	//�^�[�Q�b�g
	IFu* m_target = nullptr;
	CVector3 m_targetPos;
};

